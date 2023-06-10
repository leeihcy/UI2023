#include "stdafx.h"
#include "scrollbarmanager.h"
#include "Src\UIObject\Window\window.h"

using namespace UI;

ScrollBarManager::ScrollBarManager():Message(nullptr)
{
    m_pIScrollBarManager = nullptr;
    m_pBindObject = nullptr;
    m_pVScrollBar = nullptr;
    m_pHScrollBar = nullptr;
  
    m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;
    m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;

    m_smoothScroll.SetCallback(static_cast<ISmoothScrollCallback*>(this));
}

ScrollBarManager::~ScrollBarManager()
{
    SAFE_DELETE(m_pIScrollBarManager);
}

IScrollBarManager*  ScrollBarManager::GetIScrollBarMgr()
{
    if (!m_pIScrollBarManager)
    {
        m_pIScrollBarManager = new IScrollBarManager(this);
    }
    return m_pIScrollBarManager;
}
void  ScrollBarManager::SetBindObject(IObject* pBindObj) 
{ 
    m_pBindObject = pBindObj; 
    if (m_pBindObject)
    {
        m_smoothScroll.SetUIApplication(m_pBindObject->GetImpl()->GetUIApplication());
    }
}
IObject*  ScrollBarManager::GetBindObject()
{ 
    return m_pBindObject; 
}

void  ScrollBarManager::Serialize(SERIALIZEDATA* pData)
{
	AttributeSerializer as(pData, TEXT("scrollbar"));
	as.AddEnum(XML_HSCROLLBAR, *(long*)&m_ehScrollbarVisibleType)
		->AddOption(SCROLLBAR_VISIBLE_NONE, XML_SCROLLBAR_NONE)
		->AddOption(SCROLLBAR_VISIBLE_AUTO, XML_SCROLLBAR_AUTO)
		->AddOption(SCROLLBAR_VISIBLE_SHOW_ALWAYS, XML_SCROLLBAR_ALWAYSSHOW)
		->AddOption(SCROLLBAR_VISIBLE_HIDE_ALWAYS, XML_SCROLLBAR_ALWAYSHIDE)
        ->SetDefault(SCROLLBAR_VISIBLE_AUTO);
	
	as.AddEnum(XML_VSCROLLBAR, *(long*)&m_evScrollbarVisibleType)
		->AddOption(SCROLLBAR_VISIBLE_NONE, XML_SCROLLBAR_NONE)
		->AddOption(SCROLLBAR_VISIBLE_AUTO, XML_SCROLLBAR_AUTO)
		->AddOption(SCROLLBAR_VISIBLE_SHOW_ALWAYS, XML_SCROLLBAR_ALWAYSSHOW)
		->AddOption(SCROLLBAR_VISIBLE_HIDE_ALWAYS, XML_SCROLLBAR_ALWAYSHIDE)
        ->SetDefault(SCROLLBAR_VISIBLE_AUTO);
}


// Note that the WM_HSCROLL message carries only 16 bits of scroll box position data. 
// Thus, applications that rely solely on WM_HSCROLL (and WM_VSCROLL) for scroll position 
// data have a practical maximum position value of 65,535. 
void  ScrollBarManager::FireScrollMessage(SCROLLBAR_DIRECTION_TYPE eType, int nSBCode, int nTrackPos)
{
    // 转化为16位的无符号数值，用于WM_SCRLL的hiword
    if (nTrackPos < 0)
    {
        nTrackPos = 0;
    }

    int nCompatibleTrackPos = nTrackPos;
    if (nCompatibleTrackPos > 65535)
    {
        //UI_LOG_WARN(_T("%s nTrackPos > 65535: %d"), FUNC_NAME, nTrackPos);
        nCompatibleTrackPos = 65535;
    }

    UIMSG msg;
    msg.message = eType==HSCROLLBAR ? WM_HSCROLL:WM_VSCROLL;
    msg.nCode = 0;
    msg.wParam = MAKEWPARAM(nSBCode, nCompatibleTrackPos);  // 传递给类似于windowless richedit时，得保持兼容
    msg.lParam = nTrackPos;
    msg.pMsgFrom = eType==HSCROLLBAR ? m_pHScrollBar:m_pVScrollBar;
    msg.pMsgTo = m_pBindObject;
    m_pIScrollBarManager->DoNotify(&msg);
}


void  ScrollBarManager::OnGetScrollOffset(int* pxOffset, int* pyOffset)
{
    GetScrollPos(pxOffset, pyOffset);
}
void  ScrollBarManager::OnGetScrollRange(int* pxRange, int* pyRange)
{
    GetScrollRange(pxRange, pyRange);
}

void  ScrollBarManager::OnVScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
{
    if (nullptr == m_pVScrollBar || m_pVScrollBar != pMsgFrom)
        return;

    nSBCode = LOWORD(nSBCode);
    int nOldPos = m_vScrollInfo.nPos;
    switch (nSBCode)
    {
    case SB_LINEUP:
        SetVScrollPos(m_vScrollInfo.nPos - m_vScrollInfo.nButtonLine);
        break;

    case SB_LINEDOWN:
        SetVScrollPos(m_vScrollInfo.nPos + m_vScrollInfo.nButtonLine);
        break;

    case SB_PAGEUP:
        SetVScrollPos(m_vScrollInfo.nPos - m_vScrollInfo.nPage);
        break;

    case SB_PAGEDOWN:
        SetVScrollPos(m_vScrollInfo.nPos + m_vScrollInfo.nPage);
        break;

    case SB_THUMBTRACK:    // Drag scroll box to specified position. The current position is provided in nPos.
    case SB_THUMBPOSITION: // Scroll to the absolute position. The current position is provided in nPos
        SetVScrollPos(nPos);
        break;
    }

    if (nOldPos != m_vScrollInfo.nPos)
    {
        m_pBindObject->Invalidate();
    }
}

void  ScrollBarManager::OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
{
    if (nullptr == m_pHScrollBar || m_pHScrollBar != pMsgFrom)
        return;

    nSBCode = LOWORD(nSBCode);
    int nOldPos = m_hScrollInfo.nPos;
    switch (nSBCode)
    {
    case SB_LINEUP:
        SetHScrollPos(m_hScrollInfo.nPos - m_hScrollInfo.nButtonLine);
        break;

    case SB_LINEDOWN:
        SetHScrollPos(m_hScrollInfo.nPos + m_hScrollInfo.nButtonLine);
        break;

    case SB_PAGEUP:
        SetHScrollPos(m_hScrollInfo.nPos - m_hScrollInfo.nPage);
        break;

    case SB_PAGEDOWN:
        SetHScrollPos(m_hScrollInfo.nPos + m_hScrollInfo.nPage);
        break;

    case SB_THUMBTRACK:    // Drag scroll box to specified position. The current position is provided in nPos.
    case SB_THUMBPOSITION: // Scroll to the absolute position. The current position is provided in nPos
        SetHScrollPos(nPos);
        break;
    }

    if (nOldPos != m_hScrollInfo.nPos)
    {
        m_pBindObject->Invalidate();
    }
}

void  ScrollBarManager::OnStateChanged(UINT nMask)
{
    SetMsgHandled(FALSE);
	NotifyStateChanged(nMask);
}

void  ScrollBarManager::NotifyStateChanged(UINT nMask)
{
    if (nMask & (OSB_HOVER|OSB_FORCEHOVER|OSB_PRESS))
    {
        bool bHover = m_pBindObject->IsHover()||m_pBindObject->IsForceHover();
        if (m_pHScrollBar)
            UISendMessage(m_pHScrollBar, UI_WM_SCROLLBAR_BINDOBJHOVER_CHANGED, bHover?1:0);
        if (m_pVScrollBar)
            UISendMessage(m_pVScrollBar, UI_WM_SCROLLBAR_BINDOBJHOVER_CHANGED, bHover?1:0);
    }
}

// 请求该控件是否需要触摸消息
LRESULT  ScrollBarManager::OnGestureBeginReq(UINT uMsg, WPARAM, LPARAM)
{
    if (m_pBindObject)
    {
        return 1;
    }

    SetMsgHandled(FALSE);
    return 0;
}

// OnMouseWheel内部会判断滚动条显隐情况分别调用OnHMouseWheel或OnVMouseWheel
// 为了支持hover列表滚动，对WM_MOUSEWHEEL消息流程进行了些调整
// 消息的返回值才表示是否处理了滚动消息。如滚动条不显示时返回0，显示时返回1。
// 但即使滚动条显示了，也不一定需要刷新，例如滚动条在最底端。
BOOL  ScrollBarManager::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
{
    BOOL bHandled = TRUE;
    BOOL bNeedRedraw = FALSE;
    DoMouseWheel(nFlags, zDelta, pt, bHandled, bNeedRedraw);
    return bHandled;
}
void  ScrollBarManager::DoMouseWheel(
        UINT nFlags, 
        short zDelta, 
        POINT pt, 
        BOOL& bHandled,
        BOOL& bNeedRedraw)
{
    // 判断本次要滚动的滚动条
    if (m_evScrollbarVisibleType == SCROLLBAR_VISIBLE_NONE && 
        m_ehScrollbarVisibleType == SCROLLBAR_VISIBLE_NONE)
    {
        bHandled = FALSE;
        bNeedRedraw = FALSE;
        return;
    }

    bNeedRedraw = FALSE;
    bHandled = TRUE;

    enum ScrollAction { None, HScroll, VScroll };
    UINT nAction = None;

    if (m_evScrollbarVisibleType == SCROLLBAR_VISIBLE_NONE)
    {
        nAction = HScroll;
    }
    else if (m_ehScrollbarVisibleType == SCROLLBAR_VISIBLE_NONE)
    {
        nAction = VScroll;
    }
    else
    {
        bool bVScrollBarAvailable = m_vScrollInfo.nRange > m_vScrollInfo.nPage;
        bool bHScrollBarAvailable = m_hScrollInfo.nRange > m_hScrollInfo.nPage;

        if (bVScrollBarAvailable && bHScrollBarAvailable && Util::IsKeyDown(VK_SHIFT))
        {
            nAction = HScroll;
        }
        else if (bVScrollBarAvailable)
        {
            nAction = VScroll;
        }
        else if (bHScrollBarAvailable)
        {
            nAction = HScroll;
        }
    }

    if (nAction == VScroll)
    {
        bNeedRedraw = OnVMouseWheel(zDelta, pt);
    }
    else if (nAction == HScroll)
    {
        bNeedRedraw = OnHMouseWheel(zDelta, pt);
    }
    else
    {
        bHandled = FALSE;
    }
}

// H暂时不支持平滑滚动
BOOL  ScrollBarManager::OnHMouseWheel(short zDelta, POINT pt)
{
    if (nullptr == m_pBindObject)
        return FALSE;

    bool bCtrlDown = Util::IsKeyDown(VK_CONTROL);  // 按下CTRL后，以3倍速进行滚动
    int nWheel = m_hScrollInfo.nWheelLine;
    if (bCtrlDown)
        nWheel *= 3;

    int nOldPos = m_hScrollInfo.nPos;
    if (zDelta < 0)
        SetHScrollPos(m_hScrollInfo.nPos + nWheel);
    else
        SetHScrollPos(m_hScrollInfo.nPos - nWheel);

    if (nOldPos == m_hScrollInfo.nPos)
        return FALSE;

    // 重新发送一个MOUSEMOVE消息给obj，用于定位滚动后的hover对象
    MapWindowPoints(nullptr, m_pBindObject->GetHWND(), &pt, 1);
    UISendMessage(m_pBindObject, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));

    return TRUE;
}


BOOL  ScrollBarManager::OnVMouseWheel(short zDelta, POINT pt)
{
    if (nullptr == m_pBindObject)
        return FALSE;

    bool bCtrlDown = Util::IsKeyDown(VK_CONTROL);  // 按下CTRL后，以3倍速进行滚动
    if (!m_smoothScroll.IsEnable() || bCtrlDown)
    {
        int nWheel = m_vScrollInfo.nWheelLine;
        if (bCtrlDown)
            nWheel *= 3;

        int nOldPos = m_vScrollInfo.nPos;

        if (zDelta < 0)
            SetVScrollPos(m_vScrollInfo.nPos + nWheel);
        else
            SetVScrollPos(m_vScrollInfo.nPos - nWheel);

        if (nOldPos == m_vScrollInfo.nPos)
            return FALSE;

        // 重新发送一个MOUSEMOVE消息给obj，用于定位滚动后的hover对象
        MapWindowPoints(nullptr, m_pBindObject->GetHWND(), &pt, 1);
        UISendMessage(m_pBindObject, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));

        return TRUE;
    }

    // 平滑滚动
    m_smoothScroll.AddPower(zDelta, m_vScrollInfo.nPage);
    return FALSE;
}

// 在平滑滚动过程中点击了列表控件，立即停止滚动
LRESULT  ScrollBarManager::OnCaptureStopInertiaScroll(UINT uMsg, WPARAM, LPARAM)
{
    SetMsgHandled(FALSE);

    // 获取当前hover控件
    if (nullptr == m_pBindObject)
        return 0;

    WindowBase* pWindow = m_pBindObject->GetImpl()->GetWindowObject();
    if (nullptr == pWindow)
        return 0;

    Object* pObjHover = pWindow->GetHoverObject();
    if (nullptr == pObjHover)
        return 0;

    if (m_pBindObject->GetImpl() == pObjHover ||
        m_pBindObject->GetImpl()->IsMyChild(pObjHover, true))
    {
        m_smoothScroll.StopScroll(); 
    }
    return 0;
}

void ScrollBarManager::SmoothScroll_Start()
{
    if (nullptr == m_pBindObject)
        return;

    IUIApplication* pUIApp = m_pBindObject->GetUIApplication();
    if (nullptr == pUIApp)
        return;

    // 监听鼠标事件，用于鼠标点击了列表控件或者滚动条时立即停止平滑滚动
    WindowBase* pWindow = m_pBindObject->GetImpl()->GetWindowObject();
    if (pWindow)
    {
        pWindow->GetMouseMgr()->SetMouseCapture(m_pIScrollBarManager, UI_MSGMAP_ID_INERTIA_MOUSE_CAPTURE);
        pWindow->GetMouseMgr()->SetKeyboardCapture(m_pIScrollBarManager, UI_MSGMAP_ID_INERTIA_KEYBOARD_CAPTURE);
    }

    // 有可能显示了提示条，发送一个消息进行隐藏
    m_pBindObject->GetUIApplication()->HideToolTip();
    // 重新检测列表控件是否hover
    NotifyStateChanged(OSB_HOVER);
}
void ScrollBarManager::SmoothScroll_Stop()
{
    if (nullptr == m_pBindObject)
        return;

    IUIApplication* pUIApp = m_pBindObject->GetUIApplication();
    if (nullptr == pUIApp)
        return;

    WindowBase* pWindow = m_pBindObject->GetImpl()->GetWindowObject();
    if (nullptr == pWindow)
        return;

    pWindow->GetMouseMgr()->ReleaseMouseCapture(m_pIScrollBarManager);
    pWindow->GetMouseMgr()->ReleaseKeyboardCapture(m_pIScrollBarManager);

    // 给窗口重新发送一个MOUSEMOVE消息，刷新当前hover对象(TODO: 不应该发送
	// 了，会导致滚动不连贯，但如何更新hoveritem)
    POINT pt = {0, 0};
    ::GetCursorPos(&pt);
    ::MapWindowPoints(nullptr, pWindow->m_hWnd, &pt, 1);
    ::SendMessage(pWindow->m_hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));

    // 重新检测列表控件是否hover
    NotifyStateChanged(OSB_HOVER);
}

SmoothScrollResult ScrollBarManager::SmoothScroll_Scroll(
		MOUSEWHEEL_DIR eDir, uint nDeltaPos)
{
    SmoothScrollResult bResult = INERTIA_SCROLL_CONTINUE;
	if (nDeltaPos == 0)
		return bResult;

    int nOldPos = m_vScrollInfo.nPos;
    int nNewPos = nOldPos;

	if (eDir == MOUSEWHEEL_UP)
		SetVScrollPos(m_vScrollInfo.nPos - nDeltaPos);
	else if (eDir == MOUSEWHEEL_DOWN)
		SetVScrollPos(m_vScrollInfo.nPos + nDeltaPos);

	int nMaxPos = GetVScrollMaxPos();
	nNewPos = m_vScrollInfo.nPos;

	// 有可能处于bounce edge动画中，因此判断是否STOP时应该多判断一下
	if (nNewPos == nOldPos ||
		(nOldPos > 0 && nNewPos == 0) ||  
		(nOldPos <nMaxPos && nNewPos == nMaxPos))
	{
		bResult = INERTIA_SCROLL_STOP;
	}

    BOOL  bHandled = FALSE;
    UISendMessage(m_pBindObject, UI_MSG_INERTIAVSCROLL, 
        nOldPos, nNewPos, 0, nullptr, 0, &bHandled);

	 // 防止有些控件没有处理该消息导致显示不正确
    if (!bHandled)
        m_pBindObject->Invalidate(); 

    return bResult;
}

SmoothScrollResult ScrollBarManager::SmoothScroll_BounceEdge(
		MOUSEWHEEL_DIR eDir, uint nBounceHeight)
{
	int nOldPos = m_vScrollInfo.nPos;
	
	if (eDir == MOUSEWHEEL_UP)
	{
	    SetVScrollPosIgnoreBoundLimit(-(int)nBounceHeight);
	}
	else
	{
		SetVScrollPosIgnoreBoundLimit(
			m_vScrollInfo.nRange - m_vScrollInfo.nPage + nBounceHeight);
	}

	BOOL  bHandled = FALSE;
	UISendMessage(m_pBindObject, UI_MSG_INERTIAVSCROLL, 
		nOldPos, m_vScrollInfo.nPos, 0, nullptr, 0, &bHandled);

	// 防止有些控件没有处理该消息导致显示不正确
	if (!bHandled)
		m_pBindObject->Invalidate(); 

	return INERTIA_SCROLL_CONTINUE;
}

int  ScrollBarManager::SmoothScroll_GetScrolledBounceHeight()
{
	if (m_vScrollInfo.nPos < 0)
		return m_vScrollInfo.nPos;

	int nMaxPos = GetVScrollMaxPos();
	if (m_vScrollInfo.nPos > nMaxPos)
		return m_vScrollInfo.nPos - nMaxPos;

	return 0;
}

void  ScrollBarManager::SetScrollBarVisibleType(
		SCROLLBAR_DIRECTION_TYPE eDirType, 
		SCROLLBAR_VISIBLE_TYPE eVisType)
{
    if(HSCROLLBAR==eDirType)
        m_ehScrollbarVisibleType = eVisType;
    else 
        m_evScrollbarVisibleType = eVisType; 
}
SCROLLBAR_VISIBLE_TYPE  ScrollBarManager::GetScrollBarVisibleType(
		SCROLLBAR_DIRECTION_TYPE eType)
{
    if(HSCROLLBAR==eType)
        return m_ehScrollbarVisibleType;
    else 
        return m_evScrollbarVisibleType; 
}

//////////////////////////////////////////////////////////////////////////
void  ScrollBarManager::SetVScrollLine(int nLine)
{
    m_vScrollInfo.nButtonLine = nLine;
}
void  ScrollBarManager::SetHScrollLine(int nLine)
{
    m_hScrollInfo.nButtonLine = nLine;
}
void  ScrollBarManager::SetVScrollWheel(int nWheel)
{
    m_vScrollInfo.nWheelLine = nWheel;
}
void  ScrollBarManager::SetHScrollWheel(int nWheel)
{
    m_hScrollInfo.nWheelLine = nWheel;
}

//////////////////////////////////////////////////////////////////////////
LONG_PTR  ScrollBarManager::NotifyHScrollInfoChanged(bool bNeedUpdateVisibleState)
{
	LONG_PTR lRet = 0;
    if (m_pHScrollBar)
    {
        lRet = UISendMessage(m_pHScrollBar, UI_WM_SCROLLBAR_INFO_CHANGED, (WPARAM)&m_hScrollInfo, (LPARAM)bNeedUpdateVisibleState);
    }
    if (m_pBindObject)
    {
        UISendMessage(m_pBindObject, UI_WM_SCROLLBAR_INFO_CHANGED, (WPARAM)HSCROLLBAR);
    }
    return lRet;
}

LONG_PTR  ScrollBarManager::NotifyVScrollInfoChanged(bool bNeedUpdateVisibleState)
{
	LONG_PTR lRet = 0;
    if (m_pVScrollBar)
    {
        lRet = UISendMessage(m_pVScrollBar, UI_WM_SCROLLBAR_INFO_CHANGED, (WPARAM)&m_vScrollInfo, (LPARAM)bNeedUpdateVisibleState);
    }
    if (m_pBindObject)
    {
        UISendMessage(m_pBindObject, UI_WM_SCROLLBAR_INFO_CHANGED, (WPARAM)VSCROLLBAR);
    }
    return lRet;
}

void  ScrollBarManager::NotifyScrollbarVisibleChanged()
{
    UISendMessage(m_pBindObject, UI_WM_SCROLLBAR_VISIBLE_CHANGED);
}

void  ScrollBarManager::SetVScrollInfo(UISCROLLINFO* lpsi)
{
    _SetScrollInfo1(VSCROLLBAR, lpsi);
}
bool  ScrollBarManager::_SetScrollInfo1(SCROLLBAR_DIRECTION_TYPE eType, UISCROLLINFO* lpsi, bool bCheckPosBound)
{
    SETSCROLLINFO_RET lRetSet = _SetScrollInfo2(eType, lpsi, bCheckPosBound);

    if (SETSCROLLINFO_RET_ERROR == lRetSet)
    {
        return false;
    }
    else if (SETSCROLLINFO_RET_OK == lRetSet)
    {
        if (eType == HSCROLLBAR && m_pHScrollBar)
        {
            NotifyHScrollInfoChanged(false);
        }
        else if (eType == VSCROLLBAR && m_pVScrollBar)
        {
            NotifyVScrollInfoChanged(false);
        }
        return true;
    }
    else if (SETSCROLLINFO_RET_NEED_UPDATA_VISIBLE == lRetSet)
    {
		LONG_PTR lNotifyRet = 0;
        if (eType == HSCROLLBAR && m_pHScrollBar)
        {
            lNotifyRet = NotifyHScrollInfoChanged(true);
        }
        else if (eType == VSCROLLBAR && m_pVScrollBar)
        {
            lNotifyRet = NotifyVScrollInfoChanged(true);
        }

        if (1 == lNotifyRet)  // 显隐发生变化，更新客户区域
        {
            UpdateBindObjectNonClientRect();
            m_pBindObject->GetImpl()->notify_WM_SIZE(0,
                m_pBindObject->GetWidth(), 
                m_pBindObject->GetHeight());

            NotifyScrollbarVisibleChanged();
            return false;
        }
    }
    return true;
}

// 2014/8/27 增加bool bCheckPosBound参数。用于支持溢出弹性滚动
SETSCROLLINFO_RET  ScrollBarManager::_SetScrollInfo2(
        SCROLLBAR_DIRECTION_TYPE eType, 
        UISCROLLINFO* lpsi,
        bool bCheckPosBound/*=true*/
    )
{
    if (nullptr == lpsi || 0 == lpsi->nMask)
        return SETSCROLLINFO_RET_ERROR;

    UISCROLLINFO* pInfo = nullptr;
    if (eType == VSCROLLBAR)
        pInfo = &m_vScrollInfo;
    else
        pInfo = &m_hScrollInfo;

    bool bNeedCalcScrollBarVisible = false;
    if (lpsi->nMask & UISIF_RANGE)
    {
        int nRange = lpsi->nRange;
        if (nRange < 0)
            nRange = 0;

         pInfo->nRange = nRange;
         bNeedCalcScrollBarVisible = true;
    }

    if (lpsi->nMask & UISIF_PAGE)
    {
        int nPage = lpsi->nPage;
        if (nPage < 0)
            nPage = 0;

        // 	if (nPage > m_nRange)  // page就是有可能超出range大小.richedit??
        // 	{
        // 		nPage = m_nRange;
        // 	}

        pInfo->nPage = nPage;
        bNeedCalcScrollBarVisible = true;
    }

    // 更新pos位置，不要超出范围
    int nNewPos = pInfo->nPos;
    if (lpsi->nMask & UISIF_POS)
    {
        nNewPos = lpsi->nPos;
    }

    if (bCheckPosBound)
    {
        if (nNewPos > pInfo->nRange-pInfo->nPage)
            nNewPos = pInfo->nRange-pInfo->nPage;
        if (nNewPos < 0)
            nNewPos = 0;
    }

    pInfo->nPos = nNewPos;

    if (lpsi->nMask & UISIF_BUTTONLINE)
        pInfo->nButtonLine = lpsi->nButtonLine;

    if (lpsi->nMask & UISIF_WHEELLINE)
        pInfo->nWheelLine = lpsi->nWheelLine;

    return bNeedCalcScrollBarVisible ? SETSCROLLINFO_RET_NEED_UPDATA_VISIBLE:SETSCROLLINFO_RET_OK;
}

void  ScrollBarManager::SetScrollRange(int nX, int nY)
{
    if (m_hScrollInfo.nRange == nX && m_vScrollInfo.nRange == nY)
        return;

    UISCROLLINFO info_h;
    info_h.nMask = UISIF_RANGE;
    info_h.nRange = nX;
    if (false == _SetScrollInfo1(HSCROLLBAR, &info_h))
        return;

    UISCROLLINFO info_v;
    info_v.nMask = UISIF_RANGE;
    info_v.nRange = nY;
    if (false == _SetScrollInfo1(VSCROLLBAR, &info_v))
        return;
}

void  ScrollBarManager::SetHScrollRange(int nX)
{
    if (m_hScrollInfo.nRange == nX)
        return;

    UISCROLLINFO info_h;
    info_h.nMask = UISIF_RANGE;
    info_h.nRange = nX;
    if (false == _SetScrollInfo1(HSCROLLBAR, &info_h))
        return;
}
void  ScrollBarManager::SetVScrollRange(int nY)
{
    if (m_vScrollInfo.nRange == nY)
        return;

    UISCROLLINFO info_h;
    info_h.nMask = UISIF_RANGE;
    info_h.nRange = nY;
    if (false == _SetScrollInfo1(VSCROLLBAR, &info_h))
        return;
}

void  ScrollBarManager::SetVScrollPage(int ny)
{
    UISCROLLINFO info_v;
    info_v.nMask = UISIF_PAGE;
    info_v.nPage = ny;
    _SetScrollInfo1(VSCROLLBAR, &info_v);

	if (m_pVScrollBar)
		UISendMessage(m_pVScrollBar, UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED);
}

void  ScrollBarManager::SetScrollPageAndRange(const SIZE* pPage, const SIZE* pRange)
{
    if (nullptr == pPage || nullptr == pRange)
        return;

    UISCROLLINFO info_h;
    info_h.nMask = UISIF_RANGE|UISIF_PAGE;
    info_h.nRange = pRange->cx;
    info_h.nPage = pPage->cx;
    if (false == _SetScrollInfo1(HSCROLLBAR, &info_h))
        return;

    UISCROLLINFO info_v;
    info_v.nMask = UISIF_RANGE|UISIF_PAGE;
    info_v.nRange = pRange->cy;
    info_v.nPage = pPage->cy;
    if (false == _SetScrollInfo1(VSCROLLBAR, &info_v))
        return;

    if (m_pHScrollBar)
        UISendMessage(m_pHScrollBar, UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED);
    if (m_pVScrollBar)
        UISendMessage(m_pVScrollBar, UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED);
}

void ScrollBarManager::UpdateBindObjectNonClientRect()
{
    if (!m_pBindObject)
        return;
    m_pBindObject->GetImpl()->UpdateObjectNonClientRegion();

    // 直接修改滚动条的page值
    CRect rcClient;
    this->m_pBindObject->GetClientRectInObject(&rcClient);

    if (m_pHScrollBar)
    {
        m_hScrollInfo.nPage = rcClient.Width();
    }
    if (m_pVScrollBar)
    {
        m_vScrollInfo.nPage = rcClient.Height();
    }
}

void  ScrollBarManager::GetScrollInfo(SCROLLBAR_DIRECTION_TYPE eDirType, UISCROLLINFO* pInfo)
{
    if (pInfo && pInfo->nMask)
    {
        if (pInfo->nMask & UISIF_POS)
            pInfo->nPos = eDirType==HSCROLLBAR ? m_hScrollInfo.nPos : m_vScrollInfo.nPos;

        if (pInfo->nMask & UISIF_RANGE)
            pInfo->nRange = eDirType==HSCROLLBAR ? m_hScrollInfo.nRange : m_vScrollInfo.nRange;  

        if (pInfo->nMask & UISIF_PAGE)
            pInfo->nPage = eDirType==HSCROLLBAR ? m_hScrollInfo.nPage : m_vScrollInfo.nPage;  

        if (pInfo->nMask & UISIF_BUTTONLINE)
            pInfo->nButtonLine = eDirType==HSCROLLBAR ? m_hScrollInfo.nButtonLine : m_vScrollInfo.nButtonLine; 

        if (pInfo->nMask & UISIF_WHEELLINE)
            pInfo->nWheelLine = eDirType==HSCROLLBAR ? m_hScrollInfo.nWheelLine : m_vScrollInfo.nWheelLine; 
    }

}

//////////////////////////////////////////////////////////////////////////
bool  ScrollBarManager::SetHScrollPos(int nPos)
{
    int nOld = m_hScrollInfo.nPos;

    UISCROLLINFO info;
    info.nMask = UISIF_POS;
    info.nPos = nPos;
    _SetScrollInfo1(HSCROLLBAR, &info);

    return m_hScrollInfo.nPos != nOld;
}
bool  ScrollBarManager::SetVScrollPos(int nPos)
{
    int nOld = m_vScrollInfo.nPos;

    UISCROLLINFO info;
    info.nMask = UISIF_POS;
    info.nPos = nPos;
    _SetScrollInfo1(VSCROLLBAR, &info);

    return m_vScrollInfo.nPos != nOld;
}
bool  ScrollBarManager::SetVScrollPosIgnoreBoundLimit(int nPos)
{
    int nOld = m_vScrollInfo.nPos;

    UISCROLLINFO info;
    info.nMask = UISIF_POS;
    info.nPos = nPos;
    _SetScrollInfo1(VSCROLLBAR, &info, false);

    return m_vScrollInfo.nPos != nOld;
}
void  ScrollBarManager::GetScrollPos(int* pX, int* pY)
{
    if (pX)
        *pX = m_hScrollInfo.nPos;
    if (pY)
        *pY = m_vScrollInfo.nPos;
}
int   ScrollBarManager::GetScrollPos(SCROLLBAR_DIRECTION_TYPE eDirType)
{
    if (HSCROLLBAR == eDirType)
        return m_hScrollInfo.nPos;
    else
        return m_vScrollInfo.nPos;
}
int   ScrollBarManager::GetHScrollPos()
{
    return m_hScrollInfo.nPos;
}
int   ScrollBarManager::GetVScrollPos()
{
    return m_vScrollInfo.nPos;
}
int   ScrollBarManager::GetVScrollMaxPos()
{
	return m_vScrollInfo.nRange-m_vScrollInfo.nPage;
}
void  ScrollBarManager::GetScrollPage(int* pX, int* pY)
{
    if (pX)
        *pX = m_hScrollInfo.nPage;
    if (pY)
        *pY = m_vScrollInfo.nPage;
}
int   ScrollBarManager::GetScrollPage(SCROLLBAR_DIRECTION_TYPE eDirType)
{
    if (HSCROLLBAR == eDirType)
        return m_hScrollInfo.nPage;
    else
        return m_vScrollInfo.nPage;
}
int   ScrollBarManager::GetHScrollPage()
{
    return m_hScrollInfo.nPage;
}
int   ScrollBarManager::GetVScrollPage()
{
    return m_vScrollInfo.nPage;
}

void  ScrollBarManager::GetScrollRange(int *pX, int* pY)
{
    if (pX)
        *pX = m_hScrollInfo.nRange;
    if (pY)
        *pY = m_vScrollInfo.nRange;
}
int   ScrollBarManager::GetScrollRange(SCROLLBAR_DIRECTION_TYPE eDirType)
{
    if (HSCROLLBAR == eDirType)
        return m_hScrollInfo.nRange;
    else
        return m_vScrollInfo.nRange;
}
int   ScrollBarManager::GetHScrollRange()
{
    return m_hScrollInfo.nRange;
}
int   ScrollBarManager::GetVScrollRange()
{
    return m_vScrollInfo.nRange;
}

bool  ScrollBarManager::IsVScrollBarAtTop()
{
    if (m_vScrollInfo.nPos == 0)
        return true;

    if (m_vScrollInfo.nRange <= m_vScrollInfo.nPage)
        return true;

    return false;
}
bool  ScrollBarManager::IsVScrollBarAtBottom()
{
    if (m_vScrollInfo.nPos >= m_vScrollInfo.nRange-m_vScrollInfo.nPage)
        return true;

    if (m_vScrollInfo.nRange <= m_vScrollInfo.nPage)
        return true;

    return false;
}

bool  ScrollBarManager::DoPageDown()
{
    if (IsVScrollBarAtBottom())
        return false;

    SetVScrollPos(GetVScrollPos() + GetVScrollPage());
    return true;
}
bool  ScrollBarManager::DoPageUp()
{
    if (IsVScrollBarAtTop())
        return false;

    SetVScrollPos(GetVScrollPos() - GetVScrollPage());
    return true;
}
bool  ScrollBarManager::DoLineDown()
{
    if (IsVScrollBarAtBottom())
        return false;

    SetVScrollPos(GetVScrollPos() + m_vScrollInfo.nButtonLine);
    return true;
}
bool  ScrollBarManager::DoLineUp()
{
    if (IsVScrollBarAtTop())
        return false;

    SetVScrollPos(GetVScrollPos() - m_vScrollInfo.nButtonLine);
    return true;
}
bool ScrollBarManager::DoHome()
{
    if (IsVScrollBarAtTop())
        return false;

    SetVScrollPos(0);
    return true;
}
bool  ScrollBarManager::DoEnd()
{
    if (IsVScrollBarAtBottom())
        return false;

    SetVScrollPos(m_vScrollInfo.nRange);
    return true;
}

// 用于scrollbar hover visible判断
bool  ScrollBarManager::IsSmoothScrolling()
{
    return m_smoothScroll.IsScrolling();
}