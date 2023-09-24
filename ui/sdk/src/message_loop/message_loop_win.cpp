#include "message_loop_win.h"
#include "message_loop.h"
#include "src/util/windows.h"

#include <atlbase.h>
CAppModule _Module;

namespace ui {

MessageLoopPlatformWin::MessageLoopPlatformWin() {}

void MessageLoopPlatformWin::Initialize(MessageLoop *p) { 
    m_message_loop = p;

    //OleInitialize(0);
    //CoInitializeEx(0, 0);
    //_Module.Init(nullptr, GetModuleHandleA(nullptr));

    // 创建一个用于转发消息的窗口，实现post ui message
    m_WndForwardPostMsg.Create(this);    
}

void MessageLoopPlatformWin::Release() {}

void MessageLoopPlatformWin::Run() {
  Run(nullptr);
}
void MessageLoopPlatformWin::Quit() {
    ::PostQuitMessage(0);
}

void MessageLoopPlatformWin::PostTask(PostTaskType &&task) {
    m_WndForwardPostMsg.Post(std::forward<PostTaskType>(task));
}
int MessageLoopPlatformWin::ScheduleTask(ScheduleTaskType &&task,
                                         int delay_ms) {
  return 0;
}

bool MessageLoopPlatformWin::IsDialogMessage(::MSG* pMsg)
{
	if (nullptr == pMsg)
		return false;

	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < (WM_MOUSEFIRST+1) || pMsg->message > WM_MOUSELAST))
		return false;

//	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB )
	{
		// 获取这个消息的窗口所在的顶层窗口，因为导航是针对是整个顶层窗口进行的
        // 2013.6.20 屏蔽掉这些代码。存在子窗口是UI窗口，但顶层窗口是普通窗口的情况
 		HWND hWndTop = pMsg->hwnd;
// 		while (1)
// 		{
// 			if (nullptr == hWndTop)
// 				return false;
// 
// 			LONG lStyle = ::GetWindowLongPtr(hWndTop, GWL_STYLE);
// 			if (lStyle & WS_CHILD)
// 			{
// 				hWndTop = ::GetParent(hWndTop);
// 			}
// 			else
// 			{
// 				break;
// 			}
// 		}

#if 0
		// 判断这个窗口是否属性UI管理的一个顶层窗口
		IWindowBase* pIWindow = m_TopWindowMgr.GetWindowBase(hWndTop);
        if (nullptr == pIWindow)
            return false;

        WindowBase* pWindow = pIWindow->GetImpl();
        if (!pWindow)
            return false;

        return pWindow->GetMouseMgr()->IsDialogMessage(pMsg);
#endif
	}

	return false;
}
    //////////////////////////////////////////////////////////////////////////
    //
    // http://dsdm.bokee.com/6033955.html  如何正确使用PeekMessage
    // 这里要注意的是多重模态的问题，如何保证正常退出
    //
    //////////////////////////////////////////////////////////////////////////
    // 2014.2.27 libo
    // 注意，不要使用MsgWaitForMultipleObjects -- QS_ALLEVENTS标志，会出现系统卡顿现象，例如安装了底层键盘钩子时，一输入就卡了
    //
    // HHOOK  g_hKeyBoardHook = nullptr;
    // LRESULT CALLBACK  LLKeyboardProc(int code, long wParam, long lParam)
    // {
    //     return 0;
    //     return CallNextHookEx(g_hKeyBoardHook, code, wParam, lParam);
    // }
    // hKeyBoardHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, g_hInstance, nullptr);
    //
    //////////////////////////////////////////////////////////////////////////

    //
    //  WaitMessage 使用需知：
    //  A pretty hard rule for WaitMessage() is that the message queue should be empty before you call it.
    //  If it isn't empty then any messages left in the queue are marked as "seen" and WaitMessage() ignores them.
    //
    //  如果没有新的消息，WaitMessage不会返回。PeekMessage/GetMessage等函数会将现有
    //  消息队列中的消息标识为旧消息，因此
    //  PostMessage(xxx);
    //  PeekMessage(PM_NOREMOVE);
    //  WaitMessage();  这里并不会立即返回，xxx消息已被标识为旧消息。除非有一个新的
    //  消息到来才能使WaitMessage返回。
    //
    void MessageLoopPlatformWin::Run(bool *quit_ref)
    {
        unsigned int dwRet = 0;
        unsigned int &nCount = m_WaitForHandlesMgr.m_nHandleCount;
        HANDLE *&pHandles = m_WaitForHandlesMgr.m_pHandles;
        ::MSG msg;

        // 会传递pbQuitLoopRef参数的，有可能是Modal类型的菜单，这种情况下需要更多的条件判断
        // 因此单独用一个分支来优化
        if (quit_ref)
        {
            bool bExit = false;
            bool &bQuitRef = (quit_ref == nullptr ? bExit : *quit_ref);
            while (false == bQuitRef)
            {
                dwRet = ::MsgWaitForMultipleObjects(nCount, pHandles, false, INFINITE, QS_ALLINPUT) - WAIT_OBJECT_0;
                if (nCount > 0 && dwRet < nCount)
                {
                    m_WaitForHandlesMgr.Do(pHandles[dwRet]);
                }
                else
                {
                    while (::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
                    {
                        if (WM_QUIT == msg.message) // 一直传递给最外的Loop，避免在内部Loop中将该消息吃掉了
                            return;

                        if (m_MsgFilterMgr.PreTranslateMessage(&msg)) // 有可能在PreTranslateMessage中修改了pbQuitLoopRef
                        {
                            PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE); // 该消息已被处理过了，移除
                            continue;
                        }

                        if (bQuitRef) // 可能的场景是弹出模态菜单后，然后在主窗口的关闭按钮上点了一下，然后菜单的PopupMenuWindow::PreTranslateMessage就要求退出了
                            return;   // 如果这时还继续处理，就会导致主窗口被销毁，当前用于弹出菜单的堆栈对象也被销毁了

                        PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
                        if (false == this->IsDialogMessage(&msg))
                        {
                            ::TranslateMessage(&msg);
                            ::DispatchMessage(&msg);
                        }

                        if (bQuitRef)
                            return;
                    }
                }
            }
        }
        else
        {
            while (1)
            {
                dwRet = ::MsgWaitForMultipleObjects(nCount, pHandles, false, INFINITE, QS_ALLINPUT) - WAIT_OBJECT_0;
                if (nCount > 0 && dwRet < nCount)
                {
                    m_WaitForHandlesMgr.Do(pHandles[dwRet]);
                }
                else
                {
                    while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
                    {
                        if (WM_QUIT == msg.message)
                        {
                            ::PostMessage(nullptr, WM_QUIT, 0, 0); // 一直传递给最外的Loop，避免在内部Loop中将该消息吃掉了
                            return;
                        }

                        if (false == m_MsgFilterMgr.PreTranslateMessage(&msg) &&
                            false == this->IsDialogMessage(&msg))
                        {
                            ::TranslateMessage(&msg);
                            ::DispatchMessage(&msg);
                        }
                    }
                }
            }
        }
        return;
    }

} // namespace ui
