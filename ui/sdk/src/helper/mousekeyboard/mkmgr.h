#pragma once

// mouse keyboard manager
namespace ui
{
class Object;
class Application;
class WindowBase;

    template <class T>
    void _SetHoverObject(Object* pNewHoverObj, T* pThis)
    {
        if (nullptr == pThis->m_pUIApplication)
            return;

        Object* pOldHoverObj = pThis->m_pObjHover;
        if (pThis->m_pObjHover == pNewHoverObj)
            return;

        pThis->m_pUIApplication->HideToolTip();
        if (nullptr != pNewHoverObj)
        {
            TOOLTIPITEM  item = {0};
            item.pNotifyObj = pNewHoverObj->GetIObject();
            pThis->m_pUIApplication->ShowToolTip(&item);
        }

        
        // 注意： 先改变所有对象的状态，再发送消息，避免在第一个对象响应消息的时候拿不到第二个对象的状态
        if (pThis->m_pObjHover)
        {
            pThis->m_pObjHover->SetHover(false, false);
        }

        // diable的object仅可以弹出tooltip，不能设置为hover
        if (pNewHoverObj && pNewHoverObj->IsEnable())
        {
            pNewHoverObj->SetHover(true, false);
        }

        pThis->m_pObjHover = pNewHoverObj;

        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_HOVER;
        if (pOldHoverObj)
        {
            msg.pMsgTo = pOldHoverObj->GetIMessage();
            UISendMessage(&msg);
        }
        if (pNewHoverObj)
        {
            msg.pMsgTo = pNewHoverObj->GetIMessage();
            UISendMessage(&msg);
        }
    }

    template <class T>
    void _SetPressObject(Object* pNewPressObj, T* pThis)
    {
        if (pThis->m_pObjPress == pNewPressObj)
            return;

        Object* pOldPressObj = pThis->m_pObjPress;
        if (pThis->m_pObjPress)
        {
            pThis->m_pObjPress->SetPress(false, false);
        }
        if (pNewPressObj)
        {
            pNewPressObj->SetPress(true, false);
        }

        pThis->m_pObjPress = pNewPressObj;
        pThis->m_pUIApplication->HideToolTip();

        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_PRESS;
        if (pOldPressObj)
        {
            msg.pMsgTo = pOldPressObj->GetIMessage();
            UISendMessage(&msg);
        }
        if (pNewPressObj)
        {
            msg.pMsgTo = pNewPressObj->GetIMessage();
            UISendMessage(&msg);
        }
    }

    template <class T>
    void _OnMouseMove(Object* pObj, WPARAM wParam, LPARAM lParam, T* pThis)
    {
        // 将所有的情况都列出来进行判断

        if (nullptr == pThis->m_pObjPress)
        {
            if (nullptr == pThis->m_pObjHover)
            {
                // 鼠标还是在外面移动
                if (pObj == pThis->m_pObjHover)   
                {
                }
                // 鼠标移动到了pObj上
                else                              
                {
                    pThis->SetHoverObject(pObj);
                    ::UISendMessage(pThis->m_pObjHover, WM_MOUSEMOVE, wParam, lParam);
                }
            }
            else
            {
                // 鼠标在A上面移动
                if (pObj == pThis->m_pObjHover)   
                {
                    ::UISendMessage(pThis->m_pObjHover, WM_MOUSEMOVE, wParam, lParam);
                }
                // 鼠标刚才放在A上面，但现在又移出去了，可能移到B上面，也可能没有移动任何对象上
                else                              
                {
                    Object* pSaveHover = pThis->m_pObjHover;
                    pThis->SetHoverObject(pObj); // 先设置状态，再发送消息

                    ::UISendMessage(pSaveHover, WM_MOUSELEAVE, (WPARAM) 0, 0);
                    if (pObj)
                    {
                        ::UISendMessage(pObj, WM_MOUSEMOVE, wParam, lParam);
                    }
                }
            }
        }
        else
        {
            if (nullptr == pThis->m_pObjHover)
            {
                // 鼠标按在了A对象上，但鼠标在A外面。现在又移动回来了
                if (pObj == pThis->m_pObjPress)  
                {
                    pThis->SetHoverObject(pThis->m_pObjPress);
                    ::UISendMessage(pThis->m_pObjPress, WM_MOUSEMOVE, wParam, lParam);

                }	
                // 鼠标在A对象上按下了，但鼠标现在不再A上面
                else                    
                {
                    ::UISendMessage(pThis->m_pObjPress, WM_MOUSEMOVE, wParam, lParam);
                }
            }
            else
            {
                // 鼠标按在了A对象上，现在在A上移动
                if (pObj == pThis->m_pObjPress)  
                {
                    UIASSERT(pThis->m_pObjPress == pThis->m_pObjHover);
                    UIASSERT(pThis->m_pObjHover == pObj);

                    ::UISendMessage(pThis->m_pObjPress, WM_MOUSEMOVE, wParam, lParam);
                }
                // 鼠标刚才按在了A对象上，但现在鼠标移出来了。
                else  
                {
                    pThis->SetHoverObject(nullptr);
                    ::UISendMessage(pThis->m_pObjPress, WM_MOUSEMOVE, wParam, lParam);
                }
            }
        }
    }



    // 注：当有一个HwndHost对象时，鼠标移到它上面时将触发WM_MOUSELEAVE消息，而不是MOUSEMOVE
    //     因此HwndHost对象还没法实现Tooltip;
    //
    template <class T>
    void _OnMouseLeave(T* pThis)
    {
        // 为了防止在对象在处理WM_LBUTTONUP消息时MouseManager的状态发生了改变,先保存状态
        Object*  pSaveObjPress = pThis->m_pObjPress;
        Object*  pSaveObjHover = pThis->m_pObjHover;

        pThis->SetHoverObject(nullptr);
        pThis->SetPressObject(nullptr);

        if (pSaveObjHover)
        {
            UIMSG msg;
            msg.pMsgTo = pSaveObjHover->GetIMessage();
            msg.message = WM_MOUSELEAVE;
            msg.wParam  = 0;
            msg.lParam  = 0;
            ::UISendMessage(&msg);
        }

        if (pSaveObjPress && pSaveObjPress != pSaveObjHover)
        {
            ::UISendMessage(pSaveObjPress, WM_MOUSELEAVE);
        }
    }

    template <class T>
    void _OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL* pbHandled, T* pThis)
    {
        if (pThis->m_pObjHover && pThis->m_pObjHover->IsEnable())
        {
            pThis->SetPressObject(pThis->m_pObjHover);

            Object* pSave = pThis->m_pObjPress;

            // 应该是先setfocus，再lbuttondown/up
            if (pThis->m_pObjPress && pThis->m_pObjPress->CanTabstop())
            {
                pThis->SetFocusObject(pThis->m_pObjPress);
            }

            if (pThis->m_pObjPress && pSave == pThis->m_pObjPress)
            {
                UIMSG msg;
                msg.pMsgTo = pThis->m_pObjHover->GetIMessage();
                msg.message = WM_LBUTTONDOWN;
                msg.wParam  = wParam;
                msg.lParam  = lParam;
                ::UISendMessage(&msg, 0, pbHandled);      // 有可能导致m_pObjPress为nullptr了
            }
        }
    }

    template <class T>
    void _OnLButtonDBClick(WPARAM wParam, LPARAM lParam, BOOL* pbHandled, T* pThis)
    {
        BOOL bHandled = FALSE;
        if (pThis->m_pObjPress)
        {
            UIMSG msg;
            msg.pMsgTo = pThis->m_pObjPress->GetIMessage();
            msg.message = WM_LBUTTONDBLCLK;
            msg.wParam  = wParam;
            msg.lParam  = lParam;

            ::UISendMessage(&msg, 0, pbHandled);
            if (pThis->m_pObjPress && pThis->m_pObjPress->IsTabstop())
            {
                pThis->SetFocusObject(pThis->m_pObjPress);
            }
            bHandled = TRUE;
        }
        else if (pThis->m_pObjHover && pThis->m_pObjHover->IsEnable())
        {
            pThis->SetPressObject(pThis->m_pObjHover);

            UIMSG msg;
            msg.pMsgTo = pThis->m_pObjHover->GetIMessage();
            msg.message = WM_LBUTTONDBLCLK;
            msg.wParam  = wParam;
            msg.lParam  = lParam;

            ::UISendMessage(&msg, 0, pbHandled);
            if (pThis->m_pObjPress && pThis->m_pObjPress->IsTabstop())
            {
                pThis->SetFocusObject(pThis->m_pObjPress);
            }
            bHandled = TRUE;
        }

        if (pbHandled)
            *pbHandled = bHandled;
    }


    template <class T>
    void _OnLButtonUp(WPARAM w, LPARAM l, T* pThis)
    {
        if (pThis->m_pObjPress)
        {
            if (pThis->m_pObjPress)
            {
                Object*  pSavePress = pThis->m_pObjPress;

                UIMSG  msg;
                msg.pMsgTo = pSavePress->GetIMessage();
                msg.message = WM_LBUTTONUP;
                msg.wParam  = w;
                msg.lParam  = l;
                ::UISendMessage(&msg);

				// SetPressObject(nullptr)放在UISendMessage前面，
				// 会导致checkbutton点击后不刷新
				if (pThis->m_pObjPress == pSavePress)
					pThis->SetPressObject(nullptr);
				else
					pSavePress->SetPress(false);
            }
        }
    }

}