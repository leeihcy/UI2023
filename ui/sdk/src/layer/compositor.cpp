#include "include/inc.h"
#include "compositor.h"
// #include "windowrender.h"

namespace ui {
Compositor::Compositor()
{
	m_pRootLayer = nullptr;
    m_pUIApp = nullptr;
#if defined(OS_WIN)    
    m_hWnd = nullptr;
    m_pWindowRender = nullptr;
#endif
    m_lPostInvalidateMsgRef = 0;
}

Compositor::~Compositor()
{
    // 由每个object自己去销毁
//     while (m_pRootLayer)
//     {
//         Layer* p = m_pRootLayer;
//         m_pRootLayer = m_pRootLayer->GetNext();
// 
//         delete p;
//     }
}

void  Compositor::SetUIApplication(Application* p)
{
    m_pUIApp = p;
}
Application*  Compositor::GetUIApplication()
{
    return m_pUIApp;
}

Layer*  Compositor::CreateLayer()
{
    Layer* pLayer = this->virtualCreateLayer();
	if (pLayer)
	{
		pLayer->SetCompositorPtr(this);
	}

    return pLayer;
}

void  Compositor::SetRootLayer(Layer* pChanged)
{
    m_pRootLayer = pChanged;
}

Layer*  Compositor::GetRootLayer()
{
	return m_pRootLayer;
}

#if defined(OS_WIN)
void Compositor::BindHWND(HWND hWnd)
{
    if (m_hWnd == hWnd)
        return;

    m_hWnd = hWnd;
    this->virtualBindHWND(hWnd);
}

HWND  Compositor::GetHWND()
{
    return m_hWnd;
}
#endif
void  Compositor::RequestInvalidate()
{
#if defined(OS_WIN)
    UIASSERT (m_hWnd);

    if (m_lPostInvalidateMsgRef == 0)
    {
        ::PostMessage(m_hWnd, UI_MSG_POSTMESSAGE, MSG_INVALIDATE, 0);
        m_lPostInvalidateMsgRef++;
    }
#endif
}

void Compositor::DoInvalidate()
{
    m_lPostInvalidateMsgRef = 0;
#if defined(OS_WIN)
    RectArray arrDirtyInWindow;
    this->UpdateDirty(arrDirtyInWindow);

    if (!m_pWindowRender->CanCommit())
        return;

    this->Commit(arrDirtyInWindow);
#endif
}

void  Compositor::Commit(const RectArray& arrDirtyInWindow)
{
#if defined(OS_WIN)
    IWindowCommitListener* pListener = m_pWindowRender->GetCommitListener();

    if (pListener)
    {
        pListener->PreCommit(
            arrDirtyInWindow.GetArrayPtr2(),
            arrDirtyInWindow.GetCount());

        this->virtualCommit(arrDirtyInWindow);

        pListener->PostCommit(
            arrDirtyInWindow.GetArrayPtr2(),
            arrDirtyInWindow.GetCount());
    }
    else
    {
        this->virtualCommit(arrDirtyInWindow);
    }
#endif
}

bool  Compositor::CreateRenderTarget(IRenderTarget** pp)
{
#if defined(OS_WIN)
    return m_pWindowRender->CreateRenderTarget(pp);
#else
    UIASSERT(false);
    return false;
#endif
}

void Compositor::SetWindowRender( WindowRender* p )
{
#if defined(OS_WIN)
    m_pWindowRender = p;
#endif
}
}