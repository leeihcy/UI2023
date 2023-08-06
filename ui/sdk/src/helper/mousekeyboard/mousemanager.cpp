#include "stdafx.h"
#include "mousemanager.h"
#include "mkmgr.h"
#include "src/Helper\tooltip\tooltipmanager.h"
#include "include/interface/iwindow.h"
#include "src/Base\Object\object.h"
#include "src/UIObject\Window\window.h"
#include "src/UIObject\HwndHost\HwndHost.h"
#include "src/Util\Gesture\gesturehelper.h"
#include "src/application/uiapplication.h"

#pragma comment(lib, "Imm32.lib")

namespace ui
{

WindowMouseMgr::WindowMouseMgr(WindowBase& o):
m_oWindow(o)
{
    m_pUIApplication = nullptr;
	
	ClearStateDirect();
}

WindowMouseMgr::~WindowMouseMgr()
{
	m_pObjPress = nullptr;
	m_pObjHover = nullptr;
    m_bMouseTrack = TRUE;
}

void  WindowMouseMgr::ClearStateDirect()
{
	m_pObjPress = nullptr;
	m_pObjHover = nullptr;
	m_pObjRPress = nullptr;
	m_pObjMPress = nullptr;
	m_pObjGesture = nullptr;
	m_pObjDefault = nullptr;
	m_pObjOriginDefault = nullptr;
	m_pFocusObject = nullptr;
	m_pFocusObject = nullptr;
	m_pOldFocusObject = nullptr;
	m_pObjKeyboardCapture  = nullptr;
	m_pObjMouseCapture = nullptr;

	m_bMouseTrack = TRUE;      // 默认需要进行鼠标监视
	m_nKeyboardCaptureNotifyMsgId = 0;
	m_nMouseCaptureNotifyMsgId = 0;
	this->m_bMouseMoveReady = FALSE;
	m_posPrevClick = -1;
}

Object*  WindowMouseMgr::GetHoverObject()
{
	return m_pObjHover;
}
Object*  WindowMouseMgr::GetPressObject()
{
	return m_pObjPress;
}

void WindowMouseMgr::SetHoverObject(Object* pNewHoverObj)
{
    _SetHoverObject(pNewHoverObj, this);
}
void WindowMouseMgr::SetPressObject(Object* pNewPressObj)
{
	_SetPressObject(pNewPressObj, this);
}

// 这两个Direct仅仅是用于防止崩溃的产物。应该是架构不优的问题
// 在listctrl中的内部子控件销毁时，没法通知到listctrl的mousemgr，而是去通知window的mousemgr
// 导致最后在listctrl::removeall中SetHoverObject(nullptr)中崩溃
void  WindowMouseMgr::SetHoverObjectDirectNULL()
{
    m_pObjHover = nullptr;
}
void  WindowMouseMgr::SetPressObjectDirectNULL()
{
    m_pObjPress = nullptr;
}
void WindowMouseMgr::OnObjectVisibleChangeInd(Object* pObj, bool bVisible)
{
    if (nullptr == pObj)
        return;

    // 即使是显示一个对象，也可能是直接显示到了当前hover object之上，如果不更新
    // hover对象，会导致再次点击时，得到响应的是老的hover object.
    // 所以显示和隐藏都要设置m_bMouseMoveReady = FALSE
    m_bMouseMoveReady = FALSE;

    if (!bVisible)
    {
        if (m_pObjHover == pObj || pObj->IsMyChild(m_pObjHover, true))
        {
            SetHoverObject(nullptr);
            m_bMouseMoveReady = FALSE;

            GetUIApplication()->HideToolTip();
        }
        if (m_pObjPress == pObj || pObj->IsMyChild(m_pObjPress, true))
        {
            SetPressObject(nullptr);
            m_bMouseMoveReady = FALSE;
        }

        if (m_pObjDefault == pObj)
        {
            SetDefaultObject(nullptr, true);
        }
        if (m_pObjOriginDefault == pObj)
        {
            m_pObjOriginDefault = nullptr;
        }

        // 切换新的焦点
        if (m_pFocusObject && 
            (pObj == m_pFocusObject || pObj->IsMyChild(m_pFocusObject, true)))
        {
            m_pFocusObject->SetFocus(false);
            ::UISendMessage(m_pFocusObject, WM_KILLFOCUS, (long)pObj, 0);
            m_pFocusObject = nullptr;

            this->Tab_2_NextControl();
        }
    }
}

void WindowMouseMgr::OnObjectRemoveInd(Object* pObj)
{
	if (nullptr == pObj)
		return;

	if (m_pObjHover == pObj || pObj->IsMyChild(m_pObjHover, true))
	{
		m_pObjHover = nullptr;
        m_bMouseMoveReady = FALSE;
	}
	if (m_pObjPress == pObj || pObj->IsMyChild(m_pObjPress, true))
	{
		m_pObjPress = nullptr;
        m_bMouseMoveReady = FALSE;
	}
    if (m_pObjRPress == pObj || pObj->IsMyChild(m_pObjRPress, true))
    {
        m_pObjRPress = nullptr;
    }
    if (m_pObjMPress == pObj || pObj->IsMyChild(m_pObjMPress, true))
    {
        m_pObjMPress = nullptr;
    }
    if (m_pObjGesture == pObj || pObj->IsMyChild(m_pObjGesture, true))
    {
        m_pObjGesture = nullptr;
    }
    if (m_pObjDefault == pObj || pObj->IsMyChild(m_pObjDefault, true))
    {
        m_pObjDefault = nullptr;
    }
    if (m_pObjOriginDefault == pObj || pObj->IsMyChild(m_pObjOriginDefault, true))
    {
        m_pObjOriginDefault = nullptr;
    }

    if (pObj == m_pFocusObject || pObj->IsMyChild(m_pFocusObject, true))
    {
        m_pFocusObject = nullptr;
    }
    if (pObj == m_pOldFocusObject)
    {
        m_pOldFocusObject = nullptr;
    }

    if (pObj->GetIMessage() == m_pObjKeyboardCapture)
    {
        m_pObjKeyboardCapture = nullptr;
        m_nKeyboardCaptureNotifyMsgId = 0;
    }
    if (pObj->GetIMessage() == m_pObjMouseCapture)
    {
        m_pObjMouseCapture = nullptr;
        m_nMouseCaptureNotifyMsgId = 0;
    }
}


//
//	[递归+遍历] [public] [static] 
//
//	获取当前鼠标下的最小单位控件. （ 不可见，不可用，不接受鼠标消息的控件、Panel 这些类型的对象不返回）
//
//	Parameter
//
//		pObjParent
//			[in]
//				递归遍历时，使用的parent object.调用该函数时赋值为窗口对象(Window/)pObj即可。
//		ptParent
//			[in]
//				在父对象中的鼠标位置，初始值为在窗口中的位置
// 
//      bUIEditorInvoke
//          [in]
//              在UIBuilder中只需要一个完整的CONTROL例如不查找SliderCtrl中的button，
//              另外UIBuilder还需要查找不接受鼠标消息类型的控件，如Label, Picture
//
//	Return
//		该位置下的对象，没有则返回空
//
Object* WindowMouseMgr::GetObjectByPos(
		Object* pObjParent, Point* ptParent, __out Point* ptOutInObj)
{
	GetObjectByPosExData  data = {0};
	data.pObjParent = pObjParent;
	data.ptParent = ptParent;
	data.ptOutInObj = ptOutInObj;
	data.bTestDecendant = true;
	data.bTestRejectMouseMsg = true;
	data.bTestDisable = false;
	return GetObjectByPosEx(&data);
}

Object*  WindowMouseMgr::GetObjectByPos_UIEditor(
		Object* pObjParent, Point* ptParent, __out Point* ptOutInObj)
{
	GetObjectByPosExData  data = {0};
	data.pObjParent = pObjParent;
	data.ptParent = ptParent;
	data.ptOutInObj = ptOutInObj;
	data.bTestDecendant = true;
	data.bTestRejectMouseMsg = false;
	data.bTestDisable = false;
	return GetObjectByPosEx(&data);
}


//  将这个不放在GetObjectByPos中，避免影响GetObjectByPos的效率
//  注: 鼠标检测到一个Panel时，并且没有命中该panel的子结点时，应该结点去检测panel的
//        下一个结点，而不是直接返回Null。
Object*  WindowMouseMgr::GetObjectByPosEx(GetObjectByPosExData* pData)
{
	Object*  pObjParent = pData->pObjParent;
	Point*  ptParent = pData->ptParent;
	Point*  ptOutInObj = pData->ptOutInObj;

    Object*  pHitObj = nullptr;
    Point  ptHitTest = *ptParent;
    Point  ptInChild = {0};

    Object* pChild = nullptr;

	bool  bEnumNcChinding = true;
    while (pChild = pObjParent->REnumAllChildObject(pChild))
    {
		if (bEnumNcChinding && !pChild->IsNcObject())
		{
			bEnumNcChinding = false; 

			// 开始枚举客户区对象，纠正偏移量，再继续搜索客户区的子对象
			REGION4 rcNonClient = {0};
			pObjParent->GetNonClientRegion(&rcNonClient);

			ptHitTest.x -= rcNonClient.left;
			ptHitTest.y -= rcNonClient.top;

			Point ptOffset = {0,0};
			if (pObjParent->GetScrollOffset((int*)&ptOffset.x, (int*)&ptOffset.y))
			{
				ptHitTest.x += ptOffset.x;
				ptHitTest.y += ptOffset.y;
			}
		}

        if (!pChild->IsVisible())
            continue;

		if (pData->bTestDisable)
		{
			if (!pChild->IsEnable())  // 注：disable控件应该允许tooltips
				continue;
		}

		if (pData->bTestRejectMouseMsg)
		{
			if (pChild->IsRejectMouseMsgAll())
				continue;
		}

        LONG_PTR nHitTest = UISendMessage(pChild, UI_MSG_HITTEST, (long)&ptHitTest, (long)&ptInChild);
        if (HTNOWHERE == nHitTest)
			continue;
        
		pHitObj = pChild;

		if (pData->bTestDecendant)
		{
			Point  ptInHitChild = {0};

			GetObjectByPosExData data = {0};
			data.pObjParent = pHitObj;
			data.ptParent = &ptInChild;
			data.ptOutInObj = &ptInHitChild;
			data.bTestDecendant = pData->bTestDecendant;
			data.bTestDisable = pData->bTestDisable;
			data.bTestRejectMouseMsg = pData->bTestRejectMouseMsg;
			Object* pRetObj = GetObjectByPosEx(&data);

			// 若子控件中没有处于该位置的，直接返回本对象
			if (pRetObj)
			{
				if (ptOutInObj)
					*ptOutInObj = ptInHitChild;
				return pRetObj; 
			}
		}

		if (pHitObj && pData->bTestRejectMouseMsg)
		{
			if (pHitObj->IsRejectMouseMsgSelf() || pHitObj->IsRejectMouseMsgAll())
			{
				pHitObj = nullptr;
				continue;
			}
		}

		if (pHitObj && ptOutInObj)
			*ptOutInObj = ptInChild;

		break;
    }
    
    return pHitObj;
}


void  WindowMouseMgr::SetUIApplication(Application* p)
{
    m_pUIApplication = p;
}


//////////////////////////////////////////////////////////////////////////

Application*  WindowMouseMgr::GetUIApplication()
{
	if (nullptr == m_oWindow)
		return nullptr;

	return m_oWindow.GetUIApplication();
}

WindowBase* WindowMouseMgr::GetWindowObject() 
{ 
    return &m_oWindow; 
}


long  WindowMouseMgr::HandleMessage(unsigned int msg, long w, long l, BOOL* pbHandled)
{
    switch (msg)
    {
    case WM_KILLFOCUS:
        this->OnKillFocus((HWND)w);
        break;

    case WM_SETFOCUS:
        this->OnSetFocus();
        break;

    case WM_NCDESTROY:
        this->OnNcDestroy();
        break;

    case WM_SETCURSOR:
        return this->OnSetCursor(w, l);
        break;
    }
    return 0L;
}
//
//The low-order word specifies the x-coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area. 
//The high-order word specifies the y-coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area. 
//
long WindowMouseMgr::HandleMouseMessage(unsigned int msg, long w, long l, BOOL* pbHandled)
{
    if (this->m_pObjMouseCapture)
    {
        BOOL bHandled = FALSE;
        long lRet = UISendMessage(m_pObjMouseCapture, msg, w, l, 0,0, m_nMouseCaptureNotifyMsgId, &bHandled);
        if (bHandled)
        {
            if (pbHandled)
                *pbHandled = TRUE;

            return lRet;
        }
    }

	int vkFlag = (int)w;
	int xPos = GET_X_LPARAM ( l );
	int yPos = GET_Y_LPARAM ( l );

	switch( msg )
	{
	case WM_MOUSEMOVE:
		{
			if (m_bMouseTrack)    //若允许追踪，则。
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof( TRACKMOUSEEVENT );
				tme.dwFlags = TME_LEAVE;
				tme.dwHoverTime = HOVER_DEFAULT;
				tme.hwndTrack = this->m_oWindow.GetHWND();
				::TrackMouseEvent( &tme );
				m_bMouseTrack = FALSE;
			}

			Object* pOldHover = m_pObjHover;
			Object* pOldPress = m_pObjPress;
			long lRet = this->OnMouseMove( vkFlag, xPos, yPos );

			if (pOldPress==m_pObjPress && pOldHover!=m_pObjHover)  // 鼠标下的对象发生变化，需要重置鼠标样式
            {
                HWND hWnd = m_oWindow.GetHWND();
				if (GetCapture() != hWnd)             // 鼠标拖拽时不变，例如从EDIT中拖拽出来
			    {
				    ::PostMessage(hWnd, WM_SETCURSOR, (long)hWnd, MAKELPARAM(HTCLIENT,1));  // hiword 0表示弹出菜单
			    }
            }
			return lRet;
		}
		break;

	case WM_MOUSELEAVE:
		return this->OnMouseLeave( vkFlag, xPos, yPos );
		break;

	case WM_CANCELMODE:
		return this->OnCancelMode(w,l);

	case WM_LBUTTONDOWN:
		return this->OnLButtonDown(w, l, pbHandled);
		break;

	case WM_LBUTTONUP:
		return this->OnLButtonUp(w, l);
		break;

	case WM_RBUTTONDOWN:
		return this->OnRButtonDown(w,l);
		break;

	case WM_RBUTTONUP:
		return this->OnRButtonUp(w,l);
		break;

	case WM_LBUTTONDBLCLK:
		//this->OnLButtonDown( vkFlag, xPos, yPos );
		return this->OnLButtonDBClick(w,l, pbHandled);
		break;

    case WM_MBUTTONDBLCLK:
        return this->OnMButtonDBClick(w,l);
        break;
    case WM_MBUTTONDOWN:
        return this->OnMButtonDown(w, l);
        break;
    case WM_MBUTTONUP:
        return this->OnMButtonUp(w, l);
        break;

    case WM_MOUSEWHEEL:
        return this->OnMouseWheel(w,l);  // 该消息可能需要上抛给主窗口处理
	}
	return 0L;
}

long  WindowMouseMgr::HandleKeyboardMessage(unsigned int msg, long w, long l, BOOL* pbHandled)
{
    if (this->m_pObjKeyboardCapture)
    {
        BOOL bHandled = FALSE;
        long lRet = UISendMessage(m_pObjKeyboardCapture, msg, w, l, 0,0, m_nKeyboardCaptureNotifyMsgId, &bHandled);
        if (bHandled)
        {
            if (pbHandled)
                *pbHandled = TRUE;

            return lRet;
        }
    }

    switch( msg )
    {
    case WM_CHAR:
        return this->OnChar(w, l);
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        return this->OnKeyDown(msg, w,l );
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        return this->OnKeyUp(w, l);
        break;

    case WM_IME_STARTCOMPOSITION:
    case WM_IME_ENDCOMPOSITION:
    case WM_IME_COMPOSITION:
    case WM_IME_SETCONTEXT:
    case WM_IME_NOTIFY:
    case WM_IME_CONTROL:
    case WM_IME_COMPOSITIONFULL:
    case WM_IME_SELECT:
    case WM_IME_CHAR:
    case WM_IME_REQUEST:
    case WM_IME_KEYDOWN:
    case WM_IME_KEYUP:
        {
            return this->OnImeMsg(msg, w, l, pbHandled);
        }
    }
    return 0L;
}

long  WindowMouseMgr::HandleTouchMessage(unsigned int msg, long wParam, long lParam, BOOL& bHandled)
{
    bHandled = FALSE;
	
    if (msg == WM_GESTURE)
    {
        // ::OutputDebugStringA("WindowMouseMgr::HandleTouchMessage  WM_GESTURE\r\n");
        bHandled = OnGesture(lParam);
        return 0;
    }
    else if (msg == WM_GESTURENOTIFY)
    {
        if (!GetGestureInstance()->IsValid())
            return 0;

        // Gives you a chance to set the gesture configuration. 
        // This message should always be bubbled up using the DefWindowProc function.
        bHandled = FALSE;
        //::OutputDebugStringA("WindowMouseMgr::HandleTouchMessage  WM_GESTURENOTIFY\r\n");

        UI::GESTURENOTIFYSTRUCT* pStruct = (UI::GESTURENOTIFYSTRUCT*)lParam;  pStruct;

		unsigned int  panWant = GC_PAN |
						 GC_PAN_WITH_SINGLE_FINGER_VERTICALLY |   						 
						 GC_PAN_WITH_GUTTER |                     
						 GC_PAN_WITH_INERTIA;
		unsigned int  panBlock = GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY;  // 暂不开启横向触摸PAN，用于支持ListItemDragDrop
        // TODO: 扩展，其它控件可能需要横向pan

		UI::GESTURECONFIG  gc[] = //{0, GC_ALLGESTURES, 0};
		{
			{ GID_PAN, panWant, panBlock },
			{ GID_ZOOM, GC_ZOOM, 0 },
			{ GID_ROTATE, GC_ROTATE, 0 },
			{ GID_TWOFINGERTAP, GC_TWOFINGERTAP, 0 },
			{ GID_PRESSANDTAP, GC_PRESSANDTAP, 0}
		};
        BOOL bResult = GetGestureInstance()->pSetGestureConfig(m_oWindow.m_hWnd, 0, 5, gc, sizeof(UI::GESTURECONFIG));

        if (!bResult)
        {
            // an error
        }
        return 0;
    }
    return 0;
}

Object*  WindowMouseMgr::GetGestureTargetObject(Point ptScreen, long wParam)
{
    MapWindowPoints(nullptr, m_oWindow.GetHWND(), &ptScreen, 1);
    Object* p = GetObjectByPos(&m_oWindow, &ptScreen, nullptr);
    while (p)
    {
        if (UISendMessage(p->GetIMessage(), UI_WM_GESTURE_BEGIN_REQ, wParam))
        {
            return p;
        }
        p = p->GetParentObject();
    }
    return nullptr;
}

BOOL  WindowMouseMgr::OnGesture(long lParam)
{
	// 本次消息通知滚动距离
	static Point lastPoint = {0};
	// 这一次手势到目前为止的总滚动距离
	static Size overpan = {0};
	static ULONGLONG lastArguments = 0;

	HWND hWnd = m_oWindow.GetHWND();
    UI::HGESTUREINFO  hGestureInfo = (UI::HGESTUREINFO)lParam;

    UI::GESTUREINFO gi = {0}; 
    gi.cbSize = sizeof(UI::GESTUREINFO);
    GetGestureInstance()->pGetGestureInfo(hGestureInfo, &gi);
	
    BOOL bHandled = FALSE;

    bool bFirstGesture = (gi.dwFlags&GF_BEGIN) ? true:false;
    bool bLastGesture = (gi.dwFlags&GF_END) ? true:false;
    switch (gi.dwID)
    {
    case GID_PAN:
	case GID_PRESSANDTAP:
        {
			unsigned int msg = UI_WM_GESTURE_PAN;
			if (gi.dwID == GID_PRESSANDTAP)				
				msg = UI_WM_GESTURE_PRESSANDTAP;	

			int xOffset = 0;
			int yOffset = 0;

            if (bFirstGesture)
            {
                Point pt = {gi.ptsLocation.x, gi.ptsLocation.y};
                m_pObjGesture = GetGestureTargetObject(pt, (long)&gi);
            }
			else
			{
				xOffset = (gi.ptsLocation.x - lastPoint.x);
				yOffset = (gi.ptsLocation.y - lastPoint.y);
			}

			if (bFirstGesture)
			{
				GetGestureInstance()->pBeginPanningFeedback(hWnd);
				overpan.cx = overpan.cy = 0;
			}
			else if (bLastGesture)
			{
				GetGestureInstance()->pEndPanningFeedback(hWnd, TRUE);
				overpan.cx = overpan.cy = 0;
			}

			overpan.cx += xOffset;
			overpan.cy += yOffset;

			if (m_pObjGesture)
            {
				LONG_PTR lRet = UISendMessage(
					m_pObjGesture->GetIMessage(),
					msg,
					MAKEWPARAM((short)xOffset, (short)yOffset),
					(long)&gi);

				if (lRet > 0)
					bHandled = TRUE;

				if (lRet == GESTURE_RETURN_NEED_BOUNCE_EDGE)
				{
					GetGestureInstance()->pUpdatePanningFeedback(
						hWnd, 0, overpan.cy, gi.dwFlags & GF_INERTIA);
				}

                if (bLastGesture)
                    m_pObjGesture = nullptr;       
            }
        }
        break;
    }

	//Remember last values for delta calculations
	lastPoint.x = gi.ptsLocation.x;
	lastPoint.y = gi.ptsLocation.y;
	lastArguments = gi.ullArguments;

    GetGestureInstance()->pCloseGestureInfoHandle(hGestureInfo);
    return bHandled;
}

//
//	Parameter
//		vkFlag
//			[in]
//
//		xPos, yPos
//			[in]	当前鼠标位置，以窗口左上角为(0,0)起点
//
long WindowMouseMgr::OnMouseMove( int vkFlag, int xPos, int yPos )
{	
    this->m_bMouseMoveReady = TRUE;

	// 1. 判断当前鼠标位置
	Point pt = { xPos, yPos };
    Point ptInObj = {0};

	Object* pObj = this->GetObjectByPos(&m_oWindow, &pt, &ptInObj);

	_OnMouseMove(pObj, vkFlag, MAKELPARAM(pt.x, pt.y), this);
	return 0L;
}


long WindowMouseMgr::OnMouseLeave( int vkFlag, int xPos, int yPos )
{
	_OnMouseLeave(this);

	this->m_bMouseTrack = TRUE;// 继续开启TRACKMOUSEEVENT
    this->m_bMouseMoveReady = FALSE;

	if (GetCapture() == m_oWindow.GetHWND())  // 有可能是其它对象直接发送过来WM_MOUSELEAVE，例如COMBOBOX.button.onlbuttondown
	{
		::ReleaseCapture();
	}
	return TRUE;
}

long WindowMouseMgr::OnCancelMode(long w, long l)
{
	if (m_pObjPress != nullptr)
	{
        UIMSG msg;
		msg.pMsgTo = m_pObjPress->GetIMessage();
		msg.message = WM_CANCELMODE;
		msg.wParam  = w;
		msg.lParam  = l;
		::UISendMessage(&msg);
	}

	return 0;
}


long WindowMouseMgr::OnLButtonDown( long w, long l, BOOL* pbHandled)
{
    // 保存本次点击位于，用于过滤双击事件
    m_posPrevClick = l;

    if (FALSE == this->m_bMouseMoveReady)
    {
        HandleMouseMessage(WM_MOUSEMOVE, 0, l, nullptr);  // 注：要取消wParam中的鼠标按下标记
    }

	if (m_pObjHover)
		::SetCapture(this->m_oWindow.GetHWND());

	_OnLButtonDown(w, l, pbHandled, this);
	return 0L;
}

long WindowMouseMgr::OnLButtonUp( long w, long l)
{
    // 为了防止在对象在处理WM_LBUTTONUP消息时MouseManager的状态发生了改变,先保存状态
    Object*  pSaveObjPress = m_pObjPress;
    Object*  pSaveObjHover = m_pObjHover;

    // 先释放capture。up的响应函数中（如按钮click）可能弹出
    // 菜单，进入阻塞模式。但capture却一直还是按钮窗口上面。
    //
    // 并且不能放在if (m_pObjPress)中。m_pObjPress有可能中途被清空
    if (GetCapture() == m_oWindow.GetHWND()) 
        ::ReleaseCapture();

	if (m_pObjPress)
	{
        _OnLButtonUp(w, l, this);

        // 如果这个时候鼠标离开了控件，发送MOUSELEAVE消息. 这个时候鼠标位置可能已经重新变化了，需要重新获取
        Point ptCursorNow;
        GetCursorPos(&ptCursorNow);
        ::MapWindowPoints(nullptr, m_oWindow.GetHWND(), &ptCursorNow, 1);

        Object* pNowHover = this->GetObjectByPos(&m_oWindow, &ptCursorNow, nullptr);
        this->SetHoverObject(pNowHover);
        if (pNowHover != pSaveObjPress && nullptr != pSaveObjPress)
            ::UISendMessage(pSaveObjPress, WM_MOUSELEAVE);
        if (pNowHover != pSaveObjHover && nullptr != pNowHover)
            ::UISendMessage(pNowHover, WM_MOUSEMOVE, w, l );
	}
	 
	return 0L;
}

long WindowMouseMgr::OnLButtonDBClick( long w,long l, BOOL* pbHandled )
{
    if (!AdjustDoubleClickMessage(l))
        return OnLButtonDown(w, l, pbHandled);

    if (FALSE == this->m_bMouseMoveReady)
    {
        HandleMouseMessage(WM_MOUSEMOVE, 0, l, nullptr);  // 注：要取消wParam中的鼠标按下标记
    }

	if (m_pObjPress || m_pObjHover)
	{
		::SetCapture(this->m_oWindow.GetHWND());
	}
	
    _OnLButtonDBClick(w, l, pbHandled, this);
	return 0;
}

long WindowMouseMgr::OnRButtonDown( long w,long l )
{
    if (FALSE == this->m_bMouseMoveReady)
    {
        HandleMouseMessage(WM_MOUSEMOVE, 0, l, nullptr);
    }

    Object* p = m_pObjHover;
    if (nullptr == p)
        return 0;

    m_pObjRPress = p;
    if (p && p->CanTabstop())
    {
        SetFocusObject(p);
    }

	if (p)
	{
		GetUIApplication()->HideToolTip();
		return ::UISendMessage(p, WM_RBUTTONDOWN, w, l);
	}
	
	return 0;
}
long WindowMouseMgr::OnRButtonUp( long w,long l )
{
	if (m_pObjRPress)
	{
		long lRet = ::UISendMessage(m_pObjRPress, WM_RBUTTONUP, w, l);
        m_pObjRPress = nullptr;
        return lRet;
	}
// 	else if (m_pObjHover)  // 仅hover时不发送up消息，因为有可能是鼠标在别的地方点击，然后移到hover上释放
// 	{
// 		return ::UISendMessage(m_pObjHover, WM_RBUTTONUP, w, l);
// 	}
	return 0;
}

long  WindowMouseMgr::OnMButtonDown(long w,long l)
{
    if (FALSE == this->m_bMouseMoveReady)
        HandleMouseMessage(WM_MOUSEMOVE, 0, l, nullptr);

    if (m_pObjHover)
    {
        m_pObjMPress = m_pObjHover;
        GetUIApplication()->HideToolTip();
        ::UISendMessage(m_pObjMPress, WM_MBUTTONDOWN, w, l);
    }

    return 0;
}
long  WindowMouseMgr::OnMButtonDBClick(long w,long l)
{
    if (FALSE == this->m_bMouseMoveReady)
        HandleMouseMessage(WM_MOUSEMOVE, 0, l, nullptr);

    if (m_pObjHover)
    {
        m_pObjMPress = m_pObjHover;
        GetUIApplication()->HideToolTip();
        ::UISendMessage(m_pObjMPress, WM_MBUTTONDBLCLK, w, l);
    }
    return 0;
}
long  WindowMouseMgr::OnMButtonUp(long w,long l)
{
    if (m_pObjMPress)
    {
        ::UISendMessage(m_pObjMPress, WM_MBUTTONUP, w, l);
        m_pObjMPress = 0;
    }
    return 0;
}

// 该函数同时还支撑着WM_CANCELMODE的作用
void WindowMouseMgr::OnKillFocus(HWND hFocusWnd)
{
	if (this->m_pObjHover)
	{
		::UISendMessage(m_pObjHover, WM_MOUSELEAVE);
	}
	if (this->m_pObjPress)
	{
        HWND hWndCapture = GetCapture();
        HWND hWnd = m_oWindow.GetHWND();
        if (hWndCapture == hWnd) 
        {
            ::ReleaseCapture();
        }

		if (this->m_pObjPress != this->m_pObjHover)  // 避免将WM_MOUSELEAVE发送两次
			::UISendMessage(m_pObjPress, WM_MOUSELEAVE);
	}
    else
    {
        HWND hWndCapture = GetCapture();
        if (hWndCapture) 
        {
            // 子窗口正setcapture，然后外部调用EnableWindow(主窗口,FALSE)
            // 将导致主窗口收到killfocus消息。子窗口的capture没有被release
            HWND hWnd = m_oWindow.GetHWND();
            if (hWndCapture == hWnd || ::IsChild(hWnd, hWndCapture))
            {
                ::ReleaseCapture();
            }
        }
    }

	this->SetHoverObject(nullptr);
	this->SetPressObject(nullptr);
    this->m_bMouseMoveReady = FALSE;  // 失焦点后直接触发鼠标按下会导致状态不正确

    m_pObjKeyboardCapture = nullptr;
    m_nKeyboardCaptureNotifyMsgId = 0;
    m_pObjMouseCapture = nullptr;
    m_nMouseCaptureNotifyMsgId = 0;

    // TODO: KEYBOARD
    Object* p = this->GetOldFocusObject();
    if (nullptr == p)
        p = m_pFocusObject;

    if (p)  // 给Focus对象发送焦点消息，但不重置m_pFocusObject。例如鼠标点到桌面上导致的失焦
    {
        p->SetFocus(false);
        ::UISendMessage(p, WM_KILLFOCUS, (long)nullptr, (long)0);
    }

    // 当鼠标是点在了本窗口上面的一个HOSTWND里的子窗口时，重置m_pFocusObject，否则鼠标再点回m_pFocusObject时将没反应
    if (m_oWindow)
    {
        HWND hWnd = m_oWindow.GetHWND();
        if (IsChild(hWnd, hFocusWnd))
            this->SetFocusObjectDirect(nullptr);
    }
}

void WindowMouseMgr::OnSetFocus()
{
    if (m_pFocusObject && !m_pFocusObject->IsFocus())
    {
        m_pFocusObject->SetFocus(true);
        ::UISendMessage(m_pFocusObject, WM_SETFOCUS, (long)GetOldFocusObject(), (long)0);  // 这里的GetOldFocusObject对应于
    }
    else
    {
        this->Tab_2_NextControl();
    }
	updateImeStatus();
}

BOOL WindowMouseMgr::OnSetCursor( long w,long l )
{
    Object* pObj = m_pObjPress;
    if (nullptr == pObj)
        pObj = m_pObjHover;

    BOOL bHandled = FALSE;
	if (pObj)
	{
		::UISendMessage(pObj->GetIMessage(), WM_SETCURSOR, w, l, 0, 0, 0, &bHandled);
	}
	return bHandled;
}

void WindowMouseMgr::OnNcDestroy()
{
	m_pObjHover = m_pObjPress = nullptr;  // 不调用SetHoverObject/SetPressObject
    m_pFocusObject = nullptr;	// 不调用SetFocusObject，因为不涉及刷新了
    m_pOldFocusObject = nullptr;
    m_pObjKeyboardCapture = nullptr;
}

/*
Code                   Meaning
------------------------------------------------------------------------

DLGC_BUTTON            Control is a button (of any kind).
DLGC_DEFPUSHBUTTON     Control is a default push button.
DLGC_HASSETSEL         Windows will send an EM_SETSEL message to the control to select its contents.
DLGC_RADIOBUTTON       Control is an option (radio) button.
DLGC_STATIC            Control is a static control.
DLGC_UNDEFPUSHBUTTON   Control is a push button but not the default push button.(是一个按钮，但不是default)
DLGC_WANTALLKEYS       Control processes all keyboard input.
DLGC_WANTARROWS        Control processes arrow keys.
DLGC_WANTCHARS         Control processes WM_CHAR messages.
DLGC_WANTMESSAGE       Control processes the message in the MSG structure that lParam points to.
DLGC_WANTTAB           Control processes the TAB key.
*/

void WindowMouseMgr::CheckDefPushButton(Object* pNewObj)
{
    if (nullptr == pNewObj)
        return;

	LONG_PTR codeNewFocus = UISendMessage(pNewObj->GetIObject(), WM_GETDLGCODE);

    if (pNewObj == m_pFocusObject)
    {
        if (codeNewFocus & DLGC_UNDEFPUSHBUTTON)
        {
            SetDefaultObject(pNewObj, true);
        }
        return;
    }

    if (codeNewFocus & DLGC_DEFPUSHBUTTON)
        return;

    Object* pLastDefaultObj = m_pObjDefault; // 用于计算最终defbtn

    /*
     * If the focus is changing to or from a pushbutton, then remove the
     * default style from the current default button
     */
    if ((m_pFocusObject != nullptr && (UISendMessage(m_pFocusObject, WM_GETDLGCODE) & (DLGC_DEFPUSHBUTTON | DLGC_UNDEFPUSHBUTTON))) ||
        (pNewObj != nullptr && (codeNewFocus & (DLGC_DEFPUSHBUTTON | DLGC_UNDEFPUSHBUTTON))))
    {
        pLastDefaultObj = nullptr;
    }

    /*
     * If moving to a button, make that button the default.
     */
    if (codeNewFocus & DLGC_UNDEFPUSHBUTTON)
    {
        pLastDefaultObj = pNewObj;
    }
    else
    {
        /*
         * Otherwise, make sure the original default button is default
         * and no others.
         */
        pLastDefaultObj = m_pObjOriginDefault;
    }

    SetDefaultObject(pLastDefaultObj, true);
}

BOOL WindowMouseMgr::IsDialogMessage(MSG* pMsg)
{
    if (pMsg->message == WM_LBUTTONDOWN)
    {
        CheckDefPushButton(m_pObjHover);
    }

    if (pMsg->message == WM_KEYDOWN)
    {
        if (VK_TAB == pMsg->wParam)
        {
			if (!IsWindowEnabled(m_oWindow.m_hWnd))
				return FALSE;

            long lRet = SendMessage(m_oWindow.m_hWnd, WM_QUERYUISTATE, 0, 0);
            if (lRet & UISF_HIDEFOCUS)
            {
                lRet &= ~UISF_HIDEFOCUS;
                SendMessage(m_oWindow.m_hWnd, WM_UPDATEUISTATE, lRet|UIS_SET, 0);
            }
            Object* pFocusObj = GetFocusObject();
            if (pFocusObj)
            {
                long lr = UISendMessage(pFocusObj->GetIMessage(), WM_GETDLGCODE, pMsg->wParam, (long)pMsg);
                if (lr & DLGC_WANTTAB)
                    return FALSE;
            }

            // 导航
            if (util::IsKeyDown(VK_SHIFT))
            {
                Tab_2_PrevControl();
            }
            else
            {
                Tab_2_NextControl();
            }
            return TRUE;
        }
        else if (VK_LEFT == pMsg->wParam)
        {

        }
        else if (VK_RIGHT == pMsg->wParam)
        {

        }
        else if (VK_UP == pMsg->wParam)
        {

        }
        else if (VK_DOWN == pMsg->wParam)
        {

        }
        else if (VK_RETURN == pMsg->wParam || VK_EXECUTE == pMsg->wParam)
        {
            if (m_pFocusObject)
            {
                // 例如在多行编辑框中回车
				LONG_PTR codeFocus = UISendMessage(m_pFocusObject, WM_GETDLGCODE, 0, (long)pMsg);
                if (codeFocus & (DLGC_WANTMESSAGE|DLGC_WANTALLKEYS))
                    return FALSE;
            }
            
            // Windows的做法是给窗口发送一个WM_COMMAND( id, hwnd )的消息
            // 查找default button，发送给defpushbutton
            if (m_pObjDefault && 
                m_pObjDefault->IsEnable() && 
                m_pObjDefault->IsVisible())
            {
                UIMSG   msg;
                msg.message = UI_MSG_DEFAULTBUTTON_VKRETURN_EVENT;
                msg.pMsgTo = m_pObjDefault->GetIMessage();
                UISendMessage(&msg);
                return TRUE;
            }
        }
        else if (VK_ESCAPE == pMsg->wParam || VK_CANCEL == pMsg->wParam)
        {
            // 查找取消按钮，发送消息
        }
    }
    else if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_MENU)
    {
        long lRet = SendMessage(m_oWindow.m_hWnd, WM_QUERYUISTATE, 0, 0);
        if (lRet & UISF_HIDEFOCUS)
        {
            lRet &= ~UISF_HIDEFOCUS;
            SendMessage(m_oWindow.m_hWnd, WM_UPDATEUISTATE, lRet|UIS_SET, 0);

            if (m_pFocusObject)
            {
                m_pFocusObject->Invalidate();
            }
        }
    }
    
    return FALSE;
}


void WindowMouseMgr::SetKeyboardCapture(IMessage* pObj, int nNotifyMsgId)
{
    m_pObjKeyboardCapture = pObj;
    m_nKeyboardCaptureNotifyMsgId = nNotifyMsgId;
}
void WindowMouseMgr::ReleaseKeyboardCapture(IMessage* pObj)
{
    if (pObj == m_pObjKeyboardCapture)
    {
        m_pObjKeyboardCapture = nullptr;
        m_nKeyboardCaptureNotifyMsgId = 0;
    }
}
void  WindowMouseMgr::SetMouseCapture(IMessage* pObj, int nNotifyMsgId)
{
    m_pObjMouseCapture = pObj;
    m_nMouseCaptureNotifyMsgId = nNotifyMsgId;
}
void  WindowMouseMgr::ReleaseMouseCapture(IMessage* pObj)
{
    if (pObj == m_pObjMouseCapture)
    {
        m_pObjMouseCapture = nullptr;
        m_nMouseCaptureNotifyMsgId = 0;
    }
}

void WindowMouseMgr::Tab_2_NextControl()
{
    Object* p = m_pFocusObject;
    if (!p)
        p = &m_oWindow;

    if (!p)
        return;

    p = p->GetNextTabObject();

    if (p)
    {
        CheckDefPushButton(p);
        SetFocusObject(p);
    }
}
void WindowMouseMgr::Tab_2_PrevControl()
{
    Object* p = m_pFocusObject;
    if (nullptr == p)
    {
        p = &m_oWindow;
    }
    p = p->GetPrevTabObject();

    if (p)
    {
        CheckDefPushButton(p);
        SetFocusObject(p);
    }
}


Object*  WindowMouseMgr::GetOldFocusObject(bool bRemove)
{
    if (bRemove)
    {
        Object* p = m_pOldFocusObject;
        m_pOldFocusObject = nullptr;
        return p;
    }
    else
    {
        return m_pOldFocusObject;
    }
}

void WindowMouseMgr::SetFocusObjectDirect(Object* pObj )
{
    m_pFocusObject = pObj;
}

Object*  WindowMouseMgr::GetFocusObject()
{
    return m_pFocusObject; 
}

void  WindowMouseMgr::SetDefaultObject(Object* pObj, bool bUpdate)
{
    if (pObj == m_pObjDefault)
        return;

    if (m_pObjDefault)
        m_pObjDefault->SetDefault(false, bUpdate);

    m_pObjDefault = pObj;

    if (m_pObjDefault)
        m_pObjDefault->SetDefault(true, bUpdate);
}


Object*  WindowMouseMgr::GetOriginDefaultObject()
{
    return m_pObjOriginDefault;
}
void  WindowMouseMgr::SetOriginDefaultObject(Object* pObj)
{
    m_pObjOriginDefault = pObj;
}
Object*  WindowMouseMgr::GetDefaultObject()
{
    return m_pObjDefault;
}


//
//	该函数只会在两个地方触发:
//		a. 鼠标点击选中一个UI控件
//		b. 使用TAB切换控件
//
//	因此当鼠标点击选中一个系统控件时，需要在HWNDHOST中调用一次SetFocusObjectDirect
//
void WindowMouseMgr::SetFocusObject(Object* pObj)
{
    // TODO: 这个条件会导致焦点从系统的combobox.edit点击到ui combobox.edit时没反应
    //       但屏蔽这个条件会导致UI COMBOBOX中的EDIT无法拖拽选中（因为一直在KILLFOCUS/SETFOCUS）
    if (m_pFocusObject == pObj)  
        return;

    if (pObj)
    {

        if (pObj->GetDescription() && OBJ_HWNDHOST == pObj->GetDescription()->GetMajorType())
        {
            HWND hWndFocus = ::GetFocus();
            HWND hWndHost = ((HwndHost*)pObj)->m_hWnd;
            if (hWndFocus != hWndHost && FALSE == ::IsChild(hWndHost, hWndFocus))  // 有可能点击了combobox中的edit,edit不是一个HOSTWND，但是HOSTWND的子窗口
            {
                m_pOldFocusObject = m_pFocusObject;   // 在HwndHost::WndProc WM_SETFOCUS中使用
                m_pFocusObject = pObj;

                if (GetWindowLongPtr(hWndHost, GWL_EXSTYLE)&WS_EX_NOACTIVATE)
                {}
                else
                    ::SetFocus(hWndHost);  
                return;
            }
        }
        else
        {
            if (m_oWindow && ::GetFocus() != m_oWindow.GetHWND())
            {
                m_pOldFocusObject = m_pFocusObject;   // 在KeyboardManager::SetFocus中使用
                m_pFocusObject = pObj;

                HWND hWnd = m_oWindow.GetHWND();
                if (!IsWindowVisible(hWnd) || GetWindowLongPtr(hWnd, GWL_EXSTYLE)&WS_EX_NOACTIVATE)
                {

                }
                else
                {
                    ::SetFocus(hWnd);  
                }

                return;
            }
        }
    }

    if (m_pFocusObject)
    {
        m_pFocusObject->SetFocus(false);
        ::UISendMessage(m_pFocusObject, WM_KILLFOCUS, (long)pObj, 0);
    }
    if (pObj)
    {
        pObj->SetFocus(true);
        ::UISendMessage(pObj, WM_SETFOCUS, (long)m_pFocusObject, 0);
    }
    m_pFocusObject = pObj;

	updateImeStatus();
}

void  WindowMouseMgr::updateImeStatus()
{
	// 根据焦点对象切换输入法状态
	// 窗口的输入法，用于区分一个控件是否需要启用输入法。
	// 如果一个控件不启用输入法，即使切换到中文输入法，也不会显示输入法的状态栏的。
	OBJSTYLE s = {0};
	s.enable_ime = 1;
	if (m_pFocusObject && m_pFocusObject->TestObjectStyle(s))
	{
		//ImmAssociateContext(m_oWindow.m_hWnd, m_hIMC);
		ImmAssociateContextEx(m_oWindow.m_hWnd, nullptr, IACE_DEFAULT);
	}
	else
	{
		ImmAssociateContextEx(m_oWindow.m_hWnd, nullptr, IACE_IGNORENOCONTEXT);
		//ImmAssociateContext(m_oWindow.m_hWnd, nullptr);
	}
}

BOOL WindowMouseMgr::OnChar(long w, long l)
{	
    if (this->m_pFocusObject)
    {
        UISendMessage(m_pFocusObject, WM_CHAR, w, l);
        return TRUE;
    }
    return FALSE;
}

BOOL WindowMouseMgr::OnKeyDown(unsigned int nMsg, long w,long l)
{
    if (this->m_pFocusObject)
    {
        UISendMessage(m_pFocusObject, WM_KEYDOWN, w, l );
        return TRUE;
    }

    return FALSE;
}
BOOL WindowMouseMgr::OnKeyUp(long w,long l)
{
    if (this->m_pFocusObject)
    {
        UISendMessage(m_pFocusObject, WM_KEYUP, w, l);
        return TRUE;
    }

    return FALSE;
}

long WindowMouseMgr::OnMouseWheel(long w, long l)
{
    // 先询问当前的press obj，能否进行mouse wheel分发
    if (m_pObjPress)
    {
        BOOL  bHandled = (BOOL)UISendMessage(m_pObjPress->GetIMessage(), UI_MSG_MOUSEWHEEL_REQ, w, l);
        if (bHandled)
            return 0;
    }

    // 先交给鼠标下的hover对象处理
    if (m_pObjHover)
    {
        BOOL  bHandled = (BOOL)UISendMessage(m_pObjHover->GetIMessage(), WM_MOUSEWHEEL, w, l);
        if (bHandled)
            return 0;
    }

    // 如果焦点对象没处理，则抛给焦点对象的父对象。
    // 例如一个位于ScrollPanel中的Edit，在Edit
    // 中进行MouseWheel的话，则应该滚动ScrollPanel

    Object*  pObj = m_pFocusObject;
	if (!pObj)
	{
		// 没有focus时，直接给hover对象进行冒泡
		if (m_pObjHover)
			pObj = m_pObjHover->GetParentObject();
	}

    while (pObj)
    {
        BOOL  bHandled = (BOOL)UISendMessage(pObj->GetIMessage(), WM_MOUSEWHEEL, w, l);
        if (bHandled)
            break;

        pObj = pObj->GetParentObject();
    }
    return 0;
}

long  WindowMouseMgr::OnImeMsg(unsigned int uMsg, long w, long l, BOOL* pbHandled)
{
    if (this->m_pFocusObject)
    {
        return UISendMessage(m_pFocusObject->GetIMessage(), uMsg, w, l, 0, nullptr, 0, pbHandled);
    }
    return 0;
}

// 系统发出来的双击事件是基于窗口客户区域的。
// 但在dui中，每个控件都是无窗口的，有可能在控件A上单击了下，然后立即
// 跑到控件B上双击了。因此对双击事件做一个过滤。
//
// return: true是一个双击事件, false做为单击事件处理
bool  WindowMouseMgr::AdjustDoubleClickMessage(long l)
{
    if (m_posPrevClick == l)
        return true;

    return false;
}

}