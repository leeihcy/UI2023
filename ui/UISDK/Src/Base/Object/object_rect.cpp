#include "stdafx.h"
#include "object.h"
#include "Src\UIObject\Window\windowbase.h"
#include "Inc\Interface\ilayout.h"
#include "Src\Helper\layout\canvaslayout.h"
#include "Src\UIObject\HwndHost\HwndHost.h"
#include "Src\Util\DPI\dpihelper.h"
#include "Src\Layer\layer.h"
#include "object_layer.h"
#include "Inc\Interface\imapattr.h"

namespace UI
{
	

void  Object::ParentClientPoint2ChildPoint(Object* pObjChild, const POINT* pt, POINT* pOut)
{
	pOut->x = pt->x - pObjChild->m_rcParent.left;
	pOut->y = pt->y - pObjChild->m_rcParent.top;
}
void  Object::ObjectPoint2ObjectClientPoint(Object* pObj, const POINT* pt,  POINT* pOut)
{
    REGION4 rcNonClient = {0};
    pObj->GetNonClientRegion(&rcNonClient);

	pOut->x = pt->x - rcNonClient.left;
	pOut->y = pt->y - rcNonClient.top;

	int xOffset = 0, yOffset = 0;
	if (pObj->GetScrollOffset(&xOffset, &yOffset))
	{
		pOut->x += xOffset;
		pOut->y += yOffset;
	}
}
void  Object::ObjectPoint2ObjectNonClientPoint(Object* pObj, const POINT* pt, POINT* pOut)
{
    pOut->x = pt->x - pObj->m_rcBorder.left;
    pOut->y = pt->y - pObj->m_rcBorder.left;
}
void  Object::ObjectRect2ObjectClientRect(Object* pObj, LPCRECT prc, RECT* pOut)
{
	POINT pt = {prc->left, prc->top};
	ObjectPoint2ObjectClientPoint(pObj, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}

void  Object::ObjectClientRect2WindowRect(Object* pObj, LPCRECT prcClient, LPRECT prcWnd)
{
	RECT  rcClientInWnd;
	pObj->GetClientRectInWindow(&rcClientInWnd);

	prcWnd->left = rcClientInWnd.left + prcClient->left;
	prcWnd->top = rcClientInWnd.top + prcClient->top;
	prcWnd->right = prcWnd->left + (prcClient->right-prcClient->left);
	prcWnd->bottom = prcWnd->top + (prcClient->bottom-prcClient->top);
}

void  Object::ObjectRect2WindowRect(Object* pObj, LPCRECT prcObj, LPRECT prcWnd)
{
	RECT rcInWindow;
	pObj->GetWindowRect(&rcInWindow);

	prcWnd->left = rcInWindow.left + prcObj->left;
	prcWnd->top = rcInWindow.top + prcObj->top;
	prcWnd->right = prcWnd->left + (prcObj->right-prcObj->left);
	prcWnd->bottom = prcWnd->top + (prcObj->bottom-prcObj->top);
}

void  Object::ParentClientPoint2ChildClientPoint(Object* pObjChild, const POINT* pt, POINT* pOut)
{
	ParentClientPoint2ChildPoint(pObjChild, pt, pOut);
	if (!pObjChild->IsNcObject())
	{
		ObjectPoint2ObjectClientPoint(pObjChild, pOut, pOut);
	}
}
void  Object::ParentClientRect2ChildClientRect(Object* pObjChild, LPCRECT prc, RECT* pOut)
{
	POINT pt = {prc->left, prc->top};
	ParentClientPoint2ChildClientPoint(pObjChild, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}
void  Object::ParentPoint2ChildPoint(Object* pObjChild, const POINT* pt, POINT* pOut)
{
	POINT ptTemp;
    if (pObjChild->IsNcObject())
    {
        ObjectPoint2ObjectNonClientPoint(pObjChild->m_pParent, pt, &ptTemp);
    }
    else
    {
	    ObjectPoint2ObjectClientPoint(pObjChild->m_pParent, pt, &ptTemp);
    }
	ParentClientPoint2ChildPoint(pObjChild, &ptTemp, pOut);
}

void  Object::ParentRect2ChildRect(Object* pObjChild, LPCRECT prc, RECT* pOut)
{
	POINT pt = {prc->left, prc->top};
	ParentPoint2ChildPoint(pObjChild, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}

void  Object::ChildPoint2ParentClientPoint(Object* pObjChild, const POINT* ptChild, POINT*  ptOut)
{
	ptOut->x = ptChild->x + pObjChild->m_rcParent.left;
	ptOut->y = ptChild->y + pObjChild->m_rcParent.top;
}

void  Object::ChildRect2ParentClientRect(Object* pObjChild, LPCRECT prc, RECT*  pOut)
{
	POINT pt = {prc->left, prc->top};
	ChildPoint2ParentClientPoint(pObjChild, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}

void  Object::ObjectClientPoint2ObjectPoint(Object*  pObj, const POINT* ptChild, POINT*  ptOut)
{
    REGION4 rcNonClient = {0};
    pObj->GetNonClientRegion(&rcNonClient);

	ptOut->x = ptChild->x + rcNonClient.left;
	ptOut->y = ptChild->y + rcNonClient.top;

	int xScroll = 0, yScroll = 0;
	if (pObj->GetScrollOffset(&xScroll, &yScroll))
	{
		ptOut->x -= xScroll;
		ptOut->y -= yScroll;
	}
}

void  Object::ObjectClientRect2ObjectRect(Object*  pObj, LPCRECT prc, RECT*  pOut)
{
	POINT pt = {prc->left, prc->top};
	ObjectClientPoint2ObjectPoint(pObj, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}

void  Object::ChildPoint2ParentPoint(Object* pObjChild, const POINT* ptChild, POINT*  ptOut)
{
	ChildPoint2ParentClientPoint(pObjChild, ptChild, ptOut);
	if (!pObjChild->IsNcObject())
	{
		ObjectClientPoint2ObjectPoint(pObjChild->m_pParent, ptOut, ptOut);
	}
}

void  Object::ChildRect2ParentRect(Object* pObjChild, LPCRECT prc, RECT*  pOut)
{
	POINT pt = {prc->left, prc->top};
	ChildPoint2ParentPoint(pObjChild, &pt, &pt);

	SetRect(pOut, 
		pt.x, 
		pt.y, 
		pt.x+(prc->right-prc->left), 
		pt.y+(prc->bottom-prc->top));
}


// 要绘制该对象之前，获取该对象在窗口中的实际位置，用于设置偏移量和裁剪区
POINT Object::GetWindowPoint()
{
	POINT pt = {0, 0};

	Object* pObjParent = nullptr;
	Object* pObjChild = this;
	while (pObjParent = this->EnumParentObject(pObjParent))
	{
		ChildPoint2ParentPoint(pObjChild, &pt, &pt);
		pObjChild = pObjParent;
	}

	return pt;
}

void Object::GetWindowRect(RECT* lprc)
{
	UIASSERT(lprc);

	POINT pt = this->GetWindowPoint();
	::SetRect(
		lprc,
		pt.x, pt.y, 
		pt.x+GetWidth(), 
		pt.y+GetHeight());
}


//
// 获取该对象的偏移量
//
// ReturnQ
//		返回false表示该对象无滚动数据
//
bool Object::GetScrollOffset(int* pxOffset, int* pyOffset)
{
	if (nullptr == pxOffset || nullptr == pyOffset)
		return false;

	*pxOffset = 0; *pyOffset = 0;

	if (m_objStyle.hscroll || m_objStyle.vscroll)
	{
		::UISendMessage(this, UI_MSG_GETSCROLLOFFSET, (WPARAM)pxOffset, (LPARAM)pyOffset);
		return true;
	}

	return false;
}

bool Object::GetScrollRange(int* pxRange, int* pyRange)
{
	if (nullptr == pxRange || nullptr == pyRange)
		return false;

	*pxRange = 0; *pyRange = 0;

	if (m_objStyle.hscroll || m_objStyle.vscroll)
	{
		::UISendMessage(this, UI_MSG_GETSCROLLRANGE, (WPARAM)pxRange, (LPARAM)pyRange);
		return true;
	}

	return false;
}

// 2014.3.28将窗口坐标转换成对象内部坐标，在有旋转变换等情况下，需要进行坐标变换
void Object::WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform)
{
	if (nullptr == ptObj || nullptr == ptWindow)
		return;

    if (false == bCalcTransform)
    {
	    POINT pt = this->GetWindowPoint();
	    ptObj->x = ptWindow->x - pt.x;
	    ptObj->y = ptWindow->y - pt.y;
    }
    else
    {
        POINT pt = *ptWindow;

        Object* pObjParent = nullptr;

        vector<Object*>  vAncestor;
        while (pObjParent = this->REnumParentObject(pObjParent))
        {
            vAncestor.push_back(pObjParent);
        }
        vAncestor.push_back(this);  // 最后还要在自己的坐标范围内转换一次

        unsigned int nSize = (uint)vAncestor.size();
        for (unsigned int i = 0; i < nSize; i++)
        {
			Object* p = vAncestor[i];
			//assert (0 && "TODO:");
#if 0
            
            if (p->m_pLayer && p->m_pLayer->HasTransform())
            {
                p->m_pRenderLayer->GetTransformPtr()->mappoint_view_2_layer(&pt);
            }
#endif

            if (p != this)
            {
                ParentPoint2ChildPoint(vAncestor[i+1], &pt, &pt);
            }
        }

        *ptObj = pt;
    }
}

void Object::WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptClient, bool bCalcTransform)
{
	if (nullptr == ptClient || nullptr == ptWindow)
		return;

	POINT pt = {0};
	WindowPoint2ObjectPoint(ptWindow, &pt, bCalcTransform);
	ObjectPoint2ObjectClientPoint(this, &pt, ptClient);
}


void  Object::WindowRect2ObjectClientRect(LPCRECT rcWindow, RECT* rcObj)
{
	POINT ptWindow = {rcWindow->left, rcWindow->top};
	POINT ptClient = {0};

	this->WindowPoint2ObjectClientPoint(&ptWindow, &ptClient, false);
	rcObj->left = ptClient.x;
	rcObj->top = ptClient.y;
	rcObj->right = rcObj->left + (rcWindow->right-rcWindow->left);
	rcObj->bottom = rcObj->top + (rcWindow->bottom-rcWindow->top);
}
void  Object::WindowRect2ObjectRect(LPCRECT rcWindow, RECT* rcObj)
{
	POINT ptWindow = {rcWindow->left, rcWindow->top};
	POINT ptObj = {0};

	this->WindowPoint2ObjectPoint(&ptWindow, &ptObj, false);
	rcObj->left = ptObj.x;
	rcObj->top = ptObj.y;
	rcObj->right = rcObj->left + (rcWindow->right-rcWindow->left);
	rcObj->bottom = rcObj->top + (rcWindow->bottom-rcWindow->top);
}

//////////////////////////////////////////////////////////////////////////


// 获取一个对象在窗口上的可视区域。例如用于绘制该对象时的裁剪
bool  Object::GetRectInWindow(RECT* prc, bool bOnlyVisiblePart)
{
	return CalcRectInAncestor(GetWindowObject(), nullptr, bOnlyVisiblePart, prc);
}

// 计算对象在层中的位置（不是层缓存中的位置，缓存也可能有偏移）
/*
bool  Object::GetRectInLayer(RECT* prc, bool bOnlyVisiblePart)
{
	assert(0 && "TODO");
#if 0
	RenderLayer*  pRenderLayer = GetRenderLayer2();
	if (!pRenderLayer)
		return GetRectInWindow(prc, bOnlyVisiblePart);

	if (!CalcRectInAncestor(pRenderLayer->GetCreateObject(),
			nullptr, bOnlyVisiblePart, prc))
	{
		return false;
	}
#endif
	return true;
}


bool  Object::GetVisibleClientRectInLayer(RECT* prc)
{
	assert (0 && TEXT("todo"));
#if 0
    RenderLayer*  pRenderLayer = GetRenderLayer2();
    if (!pRenderLayer)
        return GetRectInWindow(prc, true);

    CRect rcClient;
    this->GetClientRectInObject(&rcClient);
    if (!CalcRectInAncestor(
			pRenderLayer->GetCreateObject(), 
			&rcClient, true, prc))
	{
        return false;
	}
#endif

    return true;
}
*/
//
// prcObjPart
//   [in] 默认是CRect  rcClip(0, 0, GetWidth(), GetHeight());
//        但为了支持只求Object上的某一部分区域在祖先中的可见区域，增加该参数
//     
bool  Object::CalcRectInAncestor(
				__in Object*  pObjAncestor,
				__in LPCRECT prcObjPart,
				bool bCalcVisible, 
				__out RECT* prcOut)
{
	// 从下往上遍历
	if (!pObjAncestor)
		return false;

    // 初始化rcObjPart，有可能给的参数范围就超出了自己的区域
    if (prcOut)
        SetRectEmpty(prcOut);

    RECT  rcObj = {0, 0, GetWidth(), GetHeight()};
    RECT  rcObjPart = {0};   // 规范过的合理区域

    if (nullptr == prcObjPart)
    {
        CopyRect(&rcObjPart, &rcObj);
    }
    else
    {
        if (!::IntersectRect(&rcObjPart, prcObjPart, &rcObj))
            return false;
    }
    
    // 自己
	if (pObjAncestor == this)
	{
		if (prcOut)
		    CopyRect(prcOut, &rcObjPart);
		return true;
	}

    //  向上遍历
	CRect    rcClip(&rcObjPart);
	Object*  pParent = nullptr;
	Object*  pChild = this;
	while (pParent = this->EnumParentObject(pParent))
	{
        // 转换成在父中的区域
		Object::ChildRect2ParentRect(pChild, &rcClip, &rcClip);

		if (bCalcVisible)
		{
			// 与父对象进行求交
			CRect  rcParent(0, 0, pParent->GetWidth(), pParent->GetHeight());
			if (!::IntersectRect(rcClip, &rcClip, &rcParent))
				return false;
		}

		if (pParent == pObjAncestor)
			break;

		pChild = pParent;
	}
	
	::CopyRect(prcOut, &rcClip);
	return true;
}

// 测试窗口上的区域与自己的交集
bool  Object::IntersectWindowRect(LPCRECT prcWindow, RECT* prcIntersectWnd, RECT* prcIntersectObj)
{
	UIASSERT(prcWindow);

	// wnd
	RECT  rcWndMe, rcIntersectWnd;
	GetRectInWindow(&rcWndMe, true);

	if (!::IntersectRect(&rcIntersectWnd, &rcWndMe, prcWindow))
		return false;

	if (prcIntersectWnd)
	{
		CopyRect(prcIntersectWnd, &rcIntersectWnd);
	}

	// obj
	if (prcIntersectObj)
	{
		//         RECT  rcObj = {0};
		//         WindowRect2ObjectRect(prcWindow, &rcObj);
		// 
		//         RECT  rcMy = {0, 0, GetWidth(), GetHeight()};
		//         RECT  rcIntersect = {0};
		//         IntersectRect(&rcIntersect, &rcMy, &rcObj);
		//         ::CopyRect(prcIntersectObj, &rcIntersect);

		RECT rcMe = {0, 0, GetWidth(), GetHeight()};
		rcMe.left += rcIntersectWnd.left - rcWndMe.left;
		rcMe.right -= rcWndMe.right - rcIntersectWnd.right;
		rcMe.top += rcIntersectWnd.top - rcWndMe.top;
		rcMe.bottom += rcWndMe.bottom - rcIntersectWnd.bottom;

		CopyRect(prcIntersectObj, &rcMe);

	}
	return true;
}

// 这里的clientrect 并不是0,0起点的，而是相对于Object左上角而言
void Object::GetClientRectInObject(RECT* prc)
{
	if (nullptr == prc)
		return;

    RECT rcNonClient = {0};
    GetNonClientRegion(&rcNonClient);

	::SetRect(prc,
		rcNonClient.left, 
		rcNonClient.top, 
		this->GetWidth() - rcNonClient.right, 
		this->GetHeight()- rcNonClient.bottom); 
}

// 这里的clientrect 是0,0起点的
void Object::GetObjectClientRect(RECT* prc)
{
	if (nullptr == prc)
		return;

    RECT rcNonClient = {0};
    GetNonClientRegion(&rcNonClient);
    
	::SetRect(prc,
		0, 
		0, 
		this->GetWidth() - rcNonClient.left - rcNonClient.right, 
		this->GetHeight()- rcNonClient.top - rcNonClient.bottom); 
}

void  Object::GetNonClientRegion(REGION4* prc) 
{
    SetRect(prc,
        m_rcExtNonClient.left   + m_rcPadding.left   + m_rcBorder.left,
        m_rcExtNonClient.top    + m_rcPadding.top    + m_rcBorder.top,
        m_rcExtNonClient.right  + m_rcPadding.right  + m_rcBorder.right,
        m_rcExtNonClient.bottom + m_rcPadding.bottom + m_rcBorder.bottom);
}

void  Object::GetClientRectInParent(RECT* prc)
{
	GetClientRectInObject(prc);
	OffsetRect(prc, m_rcParent.left, m_rcParent.top);
}

// clientrect在窗口中的坐标
void Object::GetClientRectInWindow( RECT* prc )
{
	if (nullptr == prc)
		return;

	this->GetWindowRect(prc);

    REGION4 rcNonClient = {0};
    GetNonClientRegion(&rcNonClient);

	prc->left += rcNonClient.left;
	prc->top  += rcNonClient.top;
	prc->right  -= rcNonClient.right;
	prc->bottom -= rcNonClient.bottom;
}



// Panel/Object需要响应WM_SIZE，但有可能控件拦截了WM_SIZE导致内部无法处理到
// 因此再增加一个虚函数，专门让内部处理size改变事件
void  Object::notify_WM_SIZE(UINT nType, UINT nWidth, UINT nHeight)
{
    this->virtualOnSize(nType, nWidth, nHeight);

    UISendMessage(m_pIObject, WM_SIZE, 0, MAKELPARAM(nWidth, nHeight));
}

void  Object::virtualOnSize(UINT nType, UINT nWidth, UINT nHeight)
{
	m_objLayer.OnSize(nWidth, nHeight);
}
void  Object::notify_WM_MOVE(int x, int y)
{
    this->virtualOnMove();
    UISendMessage(this, WM_MOVE, 0, MAKELPARAM(m_rcParent.left,m_rcParent.top));
}

void  Object::virtualOnMove()
{
	Object* p = nullptr;
	while (p = EnumAllChildObject(p))
	{
		p->virtualOnMove();
	}
}

//
//	当对象显示/隐藏，或者大小发生改变时，重新刷新自己所在layout的布局
//
//	1. 获取自己的所在Layout
//	2. 测量Layout的大小是否发生改变，如果这个Layout大小改变，再获取上一层Layout
//
void Object::UpdateLayout()
{
	if (m_pDescription && m_pDescription->GetMajorType() == OBJ_WINDOW)
	{
		ILayout* pLayout = static_cast<WindowBase*>(this)->GetLayout();
		if (pLayout)
		{
			pLayout->Arrange(nullptr);
            this->Invalidate();
		}
		return;
	}

    Object* pParent = this->GetParentObject();
    Object* pObjectToUpdate = this;

    while (pParent)
    {
		OBJ_TYPE nObjType = OBJ_UNKNOWN;
		if (pParent->GetDescription())
			nObjType = pParent->GetDescription()->GetMajorType();

        if (OBJ_PANEL != nObjType && OBJ_WINDOW != nObjType)  // 例如listview中的headerctrl，它的父对象不是panel
        {
            pParent->notify_WM_SIZE(0, pParent->GetWidth(), pParent->GetHeight());
            return;
        }

        ILayout* pLayout = ((Panel*)pParent)->GetLayout();
        if (nullptr == pLayout)
        {
            UI_LOG_WARN(_T("GetLayout failed. id=%s"), pParent->m_strId.c_str());
            break;
        }

        ILayoutParam*  pLayoutParam = pParent->GetLayoutParam();
        if (pLayoutParam && !pLayoutParam->IsSizedByContent())
        {
            pLayout->Arrange(pObjectToUpdate->GetIObject());
            pParent->Invalidate();
            break;
        }

        SIZE sizeOld = {pParent->GetWidth(), pParent->GetHeight()};
        SIZE size = pParent->GetDesiredSize();

        if (sizeOld.cx == size.cx || sizeOld.cy == size.cy)
        {
            pLayout->Arrange(pObjectToUpdate->GetIObject());
            pParent->Invalidate();
            break;
        }
        // pParent的大小发生了变化，继续往上
        if (pParent->GetParentObject())
        {
            pObjectToUpdate = pParent;
            pParent = pParent->GetParentObject();  // TODO: 有点乱... 当是窗口的时候size为window rect，sizeOld为client rect，因此肯定不一样
            continue;                              // 所以在这里如果发现是window( parent==null )则继续往下处理
        }
        else if (
			pParent->GetDescription() && 
			pParent->GetDescription()->GetMajorType() == OBJ_WINDOW)
        {
            HWND hWnd = GetHWND();

            CRect rcWndNow;
            ::GetWindowRect(hWnd, rcWndNow);
            if (rcWndNow.Width() == size.cx && rcWndNow.Height() == size.cy)  // 当大小没有改变时，不会触发WM_SIZE，也就不会更新了
            {
                pLayout->Arrange(pObjectToUpdate->GetIObject());
                pParent->Invalidate();
            }
            else
            {
                
                UINT nFlag = SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE;
//                 if (!bUpdate) 
//                 {
//                     WindowRender*  pRender = static_cast<WindowBase*>(pParent)->GetWindowRender();
//                     pRender->SetCanCommit(false);
//                     SetWindowPos(GetHWND(), 0, 0,0, size.cx, size.cy, nFlag);
//                     pRender->SetCanCommit(true);
//                 }
//                 else
                {
                    SetWindowPos(GetHWND(), 0, 0,0, size.cx, size.cy, nFlag);
                }
            }
            break;
        }
    }
}

// UpdateLayout更新自己在窗口上的布局，一般不修改其自己的内部布局
// UpdateMyLayout只修改自己的内部布局
// void  Object::UpdateMyLayout()
// {
//     IPanel*  pPanel = (IPanel*)m_pIObject->QueryInterface(__uuidof(IPanel));
//     if (pPanel)
//     {
//         ILayout*  pLayout = pPanel->GetLayout();
//         if (pLayout)
//         {
//             pLayout->Arrange();
//             pPanel->Invalidate();
//         }
//     }
// }

//
//	获取对象自己期望的大小
//
SIZE Object::GetDesiredSize()
{
    if (!m_pLayoutParam)
    {
        CreateLayoutParam();

        if (!m_pLayoutParam) // 还是创建不成功，例如Combobox中的Button，它没有父Panel-Layout
        {
            m_pLayoutParam = CanvasLayout::s_CreateLayoutParam(m_pIObject);
        }
    }

    return m_pLayoutParam->CalcDesiredSize();
}


void Object::SetObjectPos(int x, int y, int cx, int cy, int nFlag)
{
	if (cx < 0)
		cx = 0;
	if (cy < 0)
		cy = 0;

    bool bMove = (nFlag&SWP_NOMOVE)?false:true;
    bool bSize = (nFlag&SWP_NOSIZE)?false:true;

    //  TODO: 加上这一段代码会导致SetVisible中的布局出现问题，如果pp -> p -> o，o发生变化
    //  让p去重新布局，然后p再检测自己的desiredsize变化了，再向pp请求重新布局，结果pp的大
    //  小没有发生变化，导致p不会去重新布局，o刷新失败。
    //     if (bMove && 
    //         x == m_rcParent.left &&
    //         y == m_rcParent.top && 
    //         bSize && 
    //         cx == m_rcParent.Width() && 
    //         cy == m_rcParent.Height())
    //         return;

    nFlag |= SWP_NOZORDER;   // 该函数不提供修改ZORDER的功能
    WINDOWPOS wndpos =  {nullptr, nullptr, x, y, cx, cy, nFlag};
    UISendMessage(this, WM_WINDOWPOSCHANGING, 0, (LPARAM)&wndpos);
    x = wndpos.x;
    y = wndpos.y;
    cx = wndpos.cx;
    cy = wndpos.cy;
    nFlag = wndpos.flags;

    if (x == m_rcParent.left && y == m_rcParent.top)
        bMove = false;
    if (cx == m_rcParent.Width() && cy == m_rcParent.Height())
        bSize = false;

    if (bMove&&bSize)
    {

    }
    else if (bMove)
    {
        cx = this->GetWidth();
        cy = this->GetHeight();
    }
    else if (bSize)
    {
        x = m_rcParent.left;
        y = m_rcParent.top;
    }
    else
    {
        if (nFlag & SWP_FORCESENDSIZEMSG)
        {
            notify_WM_SIZE(0, m_rcParent.Width(),m_rcParent.Height());
        }
        if (nFlag & SWP_FORCEUPDATEOBJECT)
        {
            this->Invalidate();
        }
        return;  // DONOTHING
    }

	WindowBase* pWindow = this->GetWindowObject();
	bool bHardComposite = false;
	if (pWindow)
		bHardComposite = pWindow->IsGpuComposite();

    //RECT rcOldVisibleRect = {0};
    if (bMove || bSize) 
    {
        // 刷新移动前的区域位置
        if (!(nFlag & SWP_NOREDRAW))
        {
            //this->GetRectInWindow(&rcOldVisibleRect, true);  // 获取下当前会刷新的区域范围，放在后面进行提交
            this->Invalidate();
        }
    }

	::SetRect(&m_rcParent, x,y,x+cx,y+cy);
    if (!(nFlag&SWP_NOUPDATELAYOUTPOS))
    {
        UpdateLayoutPos();
    }

    // MSDN: MoveWindow sends the WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED, WM_MOVE, WM_SIZE, and WM_NCCALCSIZE messages to the window. 
    // 在这里我们暂时只先发送WM_MOVE/WM_SIZE消息
    if (!(nFlag & SWP_NOSENDCHANGING))
    {
        if (bMove)
        {
            notify_WM_MOVE(m_rcParent.left,m_rcParent.top);
        }
        if (bSize)
        {
            notify_WM_SIZE(0, m_rcParent.Width(),m_rcParent.Height());
        }

        WINDOWPOS wndpos2 =  {nullptr, nullptr, x, y, cx, cy, nFlag};
        UISendMessage(this, WM_WINDOWPOSCHANGED, 0, (LPARAM)&wndpos2);
    }

    if (bMove || bSize)
    {
        // 刷新移动后的区域位置
        if (!(nFlag & SWP_NOREDRAW))
        {
			// 2015.9.22
			// 如果是硬件合成模式，则最终窗口上的缓存并不包含其它
			// 层的内容，直接CommitDoubleBuffer会导致内容丢失。

            this->Invalidate();
			/*if (bHardComposite)
			{
				pWindow->DirectComposite();
			}
			else
			{
                RECT rcArray[2];
                CopyRect(rcArray, &rcOldVisibleRect);
                GetRectInWindow(&rcArray[1], true);
                
				// 将移动之前的区域数据提交到窗口上，避免闪烁
				pWindow->CommitDoubleBuffet2Window(nullptr, rcArray, 2);
			}
            */
        }
    }
}


void Object::SetObjectPos(LPCRECT prc, int nFlag)
{
    if (nullptr == prc)
        return;

    this->SetObjectPos(prc->left, prc->top, RECTW(prc), RECTH(prc), nFlag);
}

// 根据m_rcParent更新 m_nConfigLeft/m_nConfigRight/m_nConfigTop/m_nConfigBottom/m_nConfigLayoutFlags
void Object::UpdateLayoutPos()
{
    if (m_pLayoutParam)
    {
        m_pLayoutParam->UpdateByRect();
    }
}

int Object::GetWidth()
{
	return m_rcParent.Width();
}


int Object::GetHeight()
{
	return m_rcParent.Height();
}


int Object::GetWidthWithMargins()
{
	return m_rcParent.Width() + m_rcMargin.left + m_rcMargin.right;
}


int Object::GetHeightWithMargins()
{
	return m_rcParent.Height() + m_rcMargin.top + m_rcMargin.bottom;
}


//
// 遍历自己的nc object来更新自己的non client region
//
void Object::UpdateObjectNonClientRegion()
{
	CRegion4  rcNonClient(0,0,0,0);

    UIMSG msg;
	msg.message = UI_MSG_CALC_PARENT_NONCLIENTRECT;
	msg.wParam = (WPARAM)&rcNonClient;
	msg.pMsgFrom = this->GetIMessage();

	Object* pNcChild = nullptr;
	while (pNcChild = this->EnumNcChildObject(pNcChild))
	{
		msg.pMsgTo = pNcChild->GetIMessage();
		UISendMessage(&msg);
	}

	this->SetExtNonClientRegion(&rcNonClient);
}

void Object::GetParentRect( RECT* prc )
{
	if (nullptr == prc)
		return;

	::CopyRect(prc, &m_rcParent);
}

ILayoutParam*  Object::GetLayoutParam()
{
    return m_pLayoutParam;
}

ILayout*  Object::GetLayout()
{
	ILayout* p = nullptr;

	Object* obj = m_pParent;
	while (obj)
	{
		p = (ILayout*)UISendMessage(obj, UI_MSG_GETLAYOUT);
		if (p)
			return p;
		obj = obj->m_pParent;
	}
	return nullptr;
}


// 如果没有，则创建一个canvas布局
ILayoutParam*  Object::GetSafeLayoutParam()
{
    if (m_pLayoutParam)
        return m_pLayoutParam;

    CreateLayoutParam();
    if (m_pLayoutParam)
        return m_pLayoutParam;

    CanvasLayout::s_GetObjectLayoutParam(this);

    SERIALIZEDATA data = { 0 };
    data.pMapAttrib = m_pIMapAttributeRemain;
    data.pSkinRes = GetISkinRes();
    data.pUIApplication = GetIUIApplication();
    data.nFlags = SERIALIZEFLAG_LOAD;
    m_pLayoutParam->Serialize(&data);

    return m_pLayoutParam;
}

void  Object::CreateLayoutParam()
{
    if (m_pLayoutParam)
        return;

    if (!m_pParent)
        return;

    ILayout* pLayout = GetLayout();
    if (!pLayout)
        return;

    m_pLayoutParam = pLayout->CreateLayoutParam(m_pIObject);
}

void  Object::DestroyLayoutParam()
{
    SAFE_RELEASE(m_pLayoutParam);
}

void  Object::SetLayoutParam(ILayoutParam* p)
{
    SAFE_RELEASE(m_pLayoutParam);
    m_pLayoutParam = p;
}

int   Object::GetConfigWidth()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigWidth();
    }
    else if (m_pIMapAttributeRemain)
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_WIDTH, false, &nRet);
        return nRet;
    }

	return NDEF;
}

int   Object::GetConfigHeight()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigHeight();
    }
    else if (m_pIMapAttributeRemain)
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_HEIGHT, false, &nRet);
        return nRet;
    }
	return NDEF;
}

int   Object::GetConfigLayoutFlags()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return 0;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigLayoutFlags();
    }
    else
    {
        const TCHAR* szText = m_pIMapAttributeRemain->GetAttr(XML_LAYOUT_ITEM_ALIGN, false);
        if (!szText)
            return 0;

        return CanvasLayoutParam::ParseAlignAttr(szText);
    }
}
int   Object::GetConfigLeft()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigLeft();
    }
    else
    {
        int nRet = NDEF;
		if (m_pIMapAttributeRemain)
			m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_LEFT, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigRight()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigRight();
    }
    else
    {
        int nRet = NDEF;
		if (m_pIMapAttributeRemain)
			m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_RIGHT, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigTop()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigTop();
    }
    else
    {
        int nRet = NDEF;
		if (m_pIMapAttributeRemain)
			m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_TOP, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigBottom()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigBottom();
    }
    else
    {
        int nRet = NDEF;
		if (m_pIMapAttributeRemain)
			m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_BOTTOM, false, &nRet);
        return nRet;
    }
}

void  Object::SetConfigWidth(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigWidth(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigWidth(n);
    }
}
void  Object::SetConfigHeight(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigHeight(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigHeight(n);
    }
}
void  Object::SetConfigLayoutFlags(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigLayoutFlags(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigLayoutFlags(n);
    }
}
void  Object::SetConfigLeft(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigLeft(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigLeft(n);
    }
}
void  Object::SetConfigRight(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigRight(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigRight(n);
    }
}
void  Object::SetConfigTop(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigTop(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigTop(n);
    }
}
void  Object::SetConfigBottom(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigBottom(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigBottom(n);
    }
}

void  Object::LoadBorder(REGION4* prc)
{
    m_rcBorder.left = ScaleByDpi(prc->left);
    m_rcBorder.top = ScaleByDpi(prc->top);
    m_rcBorder.right = ScaleByDpi(prc->right);
    m_rcBorder.bottom = ScaleByDpi(prc->bottom);
}
void  Object::SaveBorder(REGION4* prc)
{
    prc->left = ScaleByDpi(m_rcBorder.left);
    prc->top = ScaleByDpi(m_rcBorder.top);
    prc->right = ScaleByDpi(m_rcBorder.right);
    prc->bottom = ScaleByDpi(m_rcBorder.bottom);
}

void  Object::SavePadding(REGION4* prc)
{
    prc->left = ScaleByDpi(m_rcPadding.left);
    prc->top = ScaleByDpi(m_rcPadding.top);
    prc->right = ScaleByDpi(m_rcPadding.right);
    prc->bottom = ScaleByDpi(m_rcPadding.bottom);
}

void  Object::LoadPadding(REGION4* prc)
{
    m_rcPadding.left = ScaleByDpi(prc->left);
    m_rcPadding.top = ScaleByDpi(prc->top);
    m_rcPadding.right = ScaleByDpi(prc->right);
    m_rcPadding.bottom = ScaleByDpi(prc->bottom);
}

void  Object::LoadMargin(REGION4* prc)
{
    m_rcMargin.left = ScaleByDpi(prc->left);
    m_rcMargin.top = ScaleByDpi(prc->top);
    m_rcMargin.right = ScaleByDpi(prc->right);
    m_rcMargin.bottom = ScaleByDpi(prc->bottom);
}

void  Object::SaveMargin(REGION4* prc)
{
    prc->left = ScaleByDpi(m_rcMargin.left);
    prc->top = ScaleByDpi(m_rcMargin.top);
    prc->right = ScaleByDpi(m_rcMargin.right);
    prc->bottom = ScaleByDpi(m_rcMargin.bottom);
}

}
