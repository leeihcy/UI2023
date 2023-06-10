#include "stdafx.h"
#include "canvaslayout.h"
#include "Src\Base\Object\object.h"
#include "Src\Base\Attribute\attribute.h"
#include "Src\Base\Attribute\long_attribute.h"
#include "Src\Base\Attribute\flags_attribute.h"
#include "Src\Base\Application\uiapplication.h"
#include "Src\Util\DPI\dpihelper.h"

namespace UI
{

SIZE  CanvasLayout::Measure()
{
    SIZE size = {0,0};

    // 通过子控件来获取自己所需要的大小
    Object*  pChild = nullptr;
    while (pChild = this->m_pPanel->EnumChildObject(pChild))
    {
        CanvasLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;
        }
        SIZE  s = pChild->GetDesiredSize();
		s.cx += pChild->GetMarginW(); 
		s.cy += pChild->GetMarginH();
        
        if (NDEF != pParam->GetConfigLeft())
        {
            s.cx += pParam->GetConfigLeft();
        }
        if (NDEF != pParam->GetConfigRight())
        {
            s.cx += pParam->GetConfigRight();
        }
        if (NDEF != pParam->GetConfigTop())
        {
            s.cy += pParam->GetConfigTop();
        }
        if (NDEF != pParam->GetConfigBottom())
        {
            s.cy += pParam->GetConfigBottom();
        }

        if (size.cx < s.cx)
        {
            size.cx = s.cx;
        }
        if (size.cy < s.cy)
        {
            size.cy = s.cy;
        }
    }
    return size;
}
void  CanvasLayout::DoArrage(IObject* pIObjToArrage)
{
    Object* pObjToArrage = nullptr;
    if (pIObjToArrage)
        pObjToArrage = pIObjToArrage->GetImpl();

    // 调用该函数时，自己的大小已经被求出来了
    CRect rcClient;
    m_pPanel->GetClientRectInObject(&rcClient);
    int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

    Object* pChild = nullptr;
    while (pChild = m_pPanel->EnumChildObject(pChild))
    {
        if (pObjToArrage && pObjToArrage != pChild)
            continue;

        if (pChild->IsSelfCollapsed())
		{
			// 隐藏的控件，在编辑器中也需要加载布局属性。
			if (m_pPanel->GetUIApplication()->IsEditorMode())
				s_GetObjectLayoutParam(pChild);

			continue;
		}

        this->ArrangeObject(pChild, nWidth, nHeight);

        if (pObjToArrage && pObjToArrage == pChild)
            break;
    }
}

void  CanvasLayout::ArrangeObject(Object*  pChild, const int& nWidth, const int& nHeight)
{
    int  x = 0, y = 0;                                  // pChild最终在parent中的坐标

    CanvasLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
    if (!pParam)
    {
        return;
    }


    //////////////////////////////////////////////////////////////////////////
    // 计算出 pChild 的 rectP的宽和高
    SIZE s = pParam->CalcDesiredSize();

	// 先CalcDesiredSize，再getconfigleft,防止在CalcDesiredSize中修改config left
	int left = pParam->GetConfigLeft();
	int top = pParam->GetConfigTop();
	int right = pParam->GetConfigRight();
	int bottom = pParam->GetConfigBottom();
	// int nConfigWidth = pParam->GetConfigWidth();
	// int nConfigHeight = pParam->GetConfigHeight();
	int nConfigFlag = pParam->GetConfigLayoutFlags();

    // 如果同时指定了left/right,则忽略width属性
	if (0 == (nConfigFlag & LAYOUT_ITEM_ALIGN_CENTER))
	{
		if (left != NDEF && right != NDEF)
		{
			s.cx = nWidth - left - right;
		}
	}
    // 如果同时指定了top/bottom，则忽略height属性
	if (0 == (nConfigFlag & LAYOUT_ITEM_ALIGN_VCENTER))
	{
		if (top != NDEF && bottom != NDEF)
		{
			s.cy = nHeight - top - bottom;
		}
	}
	if (s.cx < 0)
		s.cx = 0;
	if (s.cy < 0)
		s.cy = 0;

    CRect rcChildObj ;
    int nChildW = s.cx;// - pChild->GetMarginW();
    int nChildH = s.cy;// - pChild->GetMarginH();
    rcChildObj.SetRect(0, 0, nChildW, nChildH );

    // 计算出坐标，若left/right,top/bottom中有一个未指定的，那么取0（但在DesktopLayout中是取居中）
	if (nConfigFlag & LAYOUT_ITEM_ALIGN_CENTER)
	{
		x = (nWidth - s.cx) / 2;  // 居中
		if (left != NDEF)
			x -= left;
		if (right != NDEF)
			x += right;
	}
	else
	{
		if (left != NDEF)
		{
			x = left;
			x += pChild->GetMarginL();
		}
		else
		{
			if (right != NDEF)
			{
				x = nWidth - right - nChildW;
				x -= pChild->GetMarginR();
			}
			else
			{
				x = 0;
				x += pChild->GetMarginL();
			}
		}
	}

	if (nConfigFlag & LAYOUT_ITEM_ALIGN_VCENTER)
	{
		y = (nHeight - s.cy) / 2;  // 居中
		if (top != NDEF)
			y -= top;
		if (bottom != NDEF)
			y += bottom;
	}
	else
	{
		if (top != NDEF)
		{
			y = top;
			y += pChild->GetMarginT();
		}
		else
		{
			if (bottom != NDEF)
			{
				y = nHeight - bottom - nChildH;
				y -= pChild->GetMarginB();
			}
			else
			{
				y = 0;
				y += pChild->GetMarginT();
			}
		}
	}

    rcChildObj.OffsetRect(x, y);
    pChild->SetObjectPos(&rcChildObj, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);
}

void  CanvasLayout::ChildObjectVisibleChanged(IObject* pObj)
{
    UIASSERT (pObj);
	UIASSERT(pObj->GetParentObject());
	UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

    // TODO: 优化为只布局该pObj，并且仅在collapsed的情况下需要布局
    //SetDirty(true);<-- 不能仅调该函数，可能后面没有触发invalidate
    DoArrage(pObj);

	RECT rc = {0};
	pObj->GetParentRect(&rc);
	m_pPanel->Invalidate(&rc);
    
	// pObj隐藏了，无法刷新，通过父对象来刷新 
	// pObj->Invalidate();
}

void  CanvasLayout::ChildObjectContentSizeChanged(IObject* pObj)
{
    UIASSERT(pObj);
    UIASSERT(pObj->GetParentObject());
    UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

    // TODO: 优化为只布局该pObj，并且仅在collapsed的情况下需要布局
    //SetDirty(true);<-- 不能仅调该函数，可能后面没有触发invalidate
    DoArrage(pObj);

    pObj->Invalidate();
}

//////////////////////////////////////////////////////////////////////////

CanvasLayoutParam::CanvasLayoutParam()
{
    m_nConfigWidth = m_nConfigHeight = AUTO;
    m_nConfigLeft = m_nConfigRight = m_nConfigTop = m_nConfigBottom = NDEF;
    m_nConfigLayoutFlags = 0;

}
CanvasLayoutParam::~CanvasLayoutParam()
{

}

SIZE  CanvasLayoutParam::CalcDesiredSize()
{
    SIZE size = {0,0};

	if (IsSizedByContent())
	{
        // 获取子对象所需要的空间
        UISendMessage(m_pObj, UI_MSG_GETDESIREDSIZE, (WPARAM)&size);

        // 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
        if (this->m_nConfigWidth != AUTO)
            size.cx = this->m_nConfigWidth;
        if (this->m_nConfigHeight!= AUTO)
            size.cy = this->m_nConfigHeight;
    }
	else
	{
		size.cx = this->m_nConfigWidth;
		size.cy = this->m_nConfigHeight;
	}

    // 计算 margin 的大小
//     size.cx += m_pObj->GetMarginW();
//     size.cy += m_pObj->GetMarginH();

    return size;
}

void  CanvasLayoutParam::UpdateByRect()
{
    Object* pParent = m_pObj->GetParentObject();
    if (!pParent)
        return;

    CRect  rcParent;
    CRect  rcPanel;
    m_pObj->GetParentRect(&rcParent);
    pParent->GetClientRectInObject(&rcPanel);

    bool  bSetX = false;
    if (m_nConfigLeft >= 0)
    {
        bSetX = true;
        m_nConfigLeft = rcParent.left;
    }
    if (m_nConfigRight >= 0)
    {
        bSetX = true;
        m_nConfigRight = rcPanel.Width() - rcParent.right;
    }
    if (!bSetX)
    {
        m_nConfigLeft = rcParent.left;   // 当left/right都没有指定时，强制设置一个
    }


    bool  bSetY = false;
    if (m_nConfigTop >= 0)
    {
        bSetY = true;
        m_nConfigTop = rcParent.top;
    }
    if (m_nConfigBottom >= 0)
    {
        bSetY = true;
        m_nConfigBottom = rcPanel.Height() - rcParent.bottom;
    }
    if (!bSetY)
    {
        m_nConfigTop = rcParent.top;   // 当top/bottom都没有指定时，强制设置一个
    }

    if (m_nConfigWidth >= 0)
    {
        m_nConfigWidth = rcParent.Width();
    }
    if (m_nConfigHeight >= 0)
    {
        m_nConfigHeight = rcParent.Height();
    }
}
void  CanvasLayoutParam::Serialize(SERIALIZEDATA* pData)
{
    AttributeSerializer s(pData, TEXT("CanvasLayoutParam"));

	// width=""，也支持 layout.width=""
    s.AddLong(
            XML_WIDTH,  this,
            memfun_cast<pfnLongSetter>(&CanvasLayoutParam::LoadConfigWidth), 
            memfun_cast<pfnLongGetter>(&CanvasLayoutParam::SaveConfigWidth)
        )
		->AddAlias(AUTO, XML_AUTO)
		->AddAlias(NDEF, XML_NDEF)
		->SetDefault(AUTO)
		->SetCompatibleKey(XML_LAYOUT_PREFIX XML_WIDTH);

    s.AddLong(
            XML_HEIGHT,  this,
            memfun_cast<pfnLongSetter>(&CanvasLayoutParam::LoadConfigHeight), 
            memfun_cast<pfnLongGetter>(&CanvasLayoutParam::SaveConfigHeight)
        )
		->AddAlias(AUTO, XML_AUTO)
		->AddAlias(NDEF, XML_NDEF)
		->SetDefault(AUTO)
		->SetCompatibleKey(XML_LAYOUT_PREFIX XML_HEIGHT);

    s.AddLong(
            XML_LAYOUT_ITEM_LEFT,  this,
            memfun_cast<pfnLongSetter>(&CanvasLayoutParam::LoadConfigLeft), 
            memfun_cast<pfnLongGetter>(&CanvasLayoutParam::SaveConfigLeft)
        )
		->AddAlias(NDEF, XML_NDEF)
		->SetDefault(NDEF);

    s.AddLong(
            XML_LAYOUT_ITEM_TOP,  this,
            memfun_cast<pfnLongSetter>(&CanvasLayoutParam::LoadConfigTop), 
            memfun_cast<pfnLongGetter>(&CanvasLayoutParam::SaveConfigTop)
        )
		->AddAlias(NDEF, XML_NDEF)
		->SetDefault(NDEF);

    s.AddLong(
            XML_LAYOUT_ITEM_RIGHT,  this,
            memfun_cast<pfnLongSetter>(&CanvasLayoutParam::LoadConfigRight), 
            memfun_cast<pfnLongGetter>(&CanvasLayoutParam::SaveConfigRight)
        )
		->AddAlias(NDEF, XML_NDEF)
		->SetDefault(NDEF);

    s.AddLong(
            XML_LAYOUT_ITEM_BOTTOM,  this,
            memfun_cast<pfnLongSetter>(&CanvasLayoutParam::LoadConfigBottom), 
            memfun_cast<pfnLongGetter>(&CanvasLayoutParam::SaveConfigBottom)
        )
		->AddAlias(NDEF, XML_NDEF)
		->SetDefault(NDEF);

    s.AddFlags(XML_LAYOUT_ITEM_ALIGN, m_nConfigLayoutFlags)
        ->AddFlag(LAYOUT_ITEM_ALIGN_LEFT,        XML_LAYOUT_ITEM_ALIGN_LEFT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_RIGHT,       XML_LAYOUT_ITEM_ALIGN_RIGHT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_TOP,         XML_LAYOUT_ITEM_ALIGN_TOP)
        ->AddFlag(LAYOUT_ITEM_ALIGN_BOTTOM,      XML_LAYOUT_ITEM_ALIGN_BOTTOM)
        ->AddFlag(LAYOUT_ITEM_ALIGN_CENTER,      XML_LAYOUT_ITEM_ALIGN_CENTER)
        ->AddFlag(LAYOUT_ITEM_ALIGN_VCENTER,     XML_LAYOUT_ITEM_ALIGN_VCENTER)
        /*->AddFlag(LAYOUT_FLAG_FORCE_DESIREDSIZE, XML_LAYOUT_ALIGN_FORCE_DESIREDSIZE)*/;
}

long  CanvasLayoutParam::GetConfigLeft()
{
    return m_nConfigLeft;
}
void  CanvasLayoutParam::LoadConfigLeft(long n)
{
    m_nConfigLeft = ScaleByDpi_if_gt0(n);
}
long  CanvasLayoutParam::SaveConfigLeft()
{
    return RestoreByDpi_if_gt0(m_nConfigLeft);
}
void  CanvasLayoutParam::SetConfigLeft(long n)
{
    m_nConfigLeft = n;
}


long  CanvasLayoutParam::GetConfigTop()
{
    return m_nConfigTop;
}
void  CanvasLayoutParam::LoadConfigTop(long n)
{
    m_nConfigTop = ScaleByDpi_if_gt0(n);
}
long  CanvasLayoutParam::SaveConfigTop()
{
    return RestoreByDpi_if_gt0(m_nConfigTop);
}
void  CanvasLayoutParam::SetConfigTop(long n)
{
    m_nConfigTop = n;
}

long  CanvasLayoutParam::GetConfigRight()
{
    return m_nConfigRight;
}
void  CanvasLayoutParam::LoadConfigRight(long n)
{
    m_nConfigRight = ScaleByDpi_if_gt0(n);
}
long  CanvasLayoutParam::SaveConfigRight()
{
    return RestoreByDpi_if_gt0(m_nConfigRight);
}
void  CanvasLayoutParam::SetConfigRight(long n)
{
    m_nConfigRight = n;
}

long  CanvasLayoutParam::GetConfigBottom()
{
    return m_nConfigBottom;
}
void  CanvasLayoutParam::LoadConfigBottom(long n)
{
    m_nConfigBottom = ScaleByDpi_if_gt0(n);
}
long  CanvasLayoutParam::SaveConfigBottom()
{
    return RestoreByDpi_if_gt0(m_nConfigBottom);
}
void  CanvasLayoutParam::SetConfigBottom(long n)
{
    m_nConfigBottom = n;
}

long  CanvasLayoutParam::GetConfigWidth()
{
    return m_nConfigWidth;
}
void  CanvasLayoutParam::SetConfigWidth(long n)
{
    m_nConfigWidth = n;
}
void  CanvasLayoutParam::LoadConfigWidth(long n)
{
    m_nConfigWidth = ScaleByDpi_if_gt0(n);
}
long  CanvasLayoutParam::SaveConfigWidth()
{
    return RestoreByDpi_if_gt0(m_nConfigWidth);
}

long  CanvasLayoutParam::GetConfigHeight()
{
    return m_nConfigHeight;
}
void  CanvasLayoutParam::SetConfigHeight(long n)
{
    m_nConfigHeight = n;
}
void  CanvasLayoutParam::LoadConfigHeight(long n)
{
    m_nConfigHeight = ScaleByDpi_if_gt0(n);
}
long  CanvasLayoutParam::SaveConfigHeight()
{
    return RestoreByDpi_if_gt0(m_nConfigHeight);
}

void  CanvasLayoutParam::SetConfigLayoutFlags(long n)
{
    m_nConfigLayoutFlags = n;
}
long  CanvasLayoutParam::GetConfigLayoutFlags()
{
    return m_nConfigLayoutFlags;
}

int  CanvasLayoutParam::ParseAlignAttr(LPCTSTR szAttr)
{
    Util::ISplitStringEnum*  pEnum = nullptr;
    int nCount = Util::SplitString(szAttr, XML_FLAG_SEPARATOR, &pEnum);

    int nRet = 0;
    for (int i = 0; i < nCount; i++)
    {
        LPCTSTR  szFlag = pEnum->GetText(i);
        if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_LEFT))
            nRet |= LAYOUT_ITEM_ALIGN_LEFT;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_RIGHT))
            nRet |= LAYOUT_ITEM_ALIGN_RIGHT;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_TOP))
            nRet |= LAYOUT_ITEM_ALIGN_TOP;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_BOTTOM))
            nRet |= LAYOUT_ITEM_ALIGN_BOTTOM;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_CENTER))
            nRet |= LAYOUT_ITEM_ALIGN_CENTER;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_VCENTER))
            nRet |= LAYOUT_ITEM_ALIGN_VCENTER;
// 		else if (0 == _tcscmp(szFlag, XML_LAYOUT_ALIGN_FORCE_DESIREDSIZE))
// 			nRet |= LAYOUT_FLAG_FORCE_DESIREDSIZE;
    }
    SAFE_RELEASE(pEnum);

    return nRet;
}

bool  CanvasLayoutParam::IsSizedByContent()
{
    bool  bWidthNotConfiged = m_nConfigWidth == AUTO;
    bool  bHeightNotConfiged = m_nConfigHeight == AUTO;

// 	if (bWidthNotConfiged)
// 	{
// 		if (m_nConfigLeft != NDEF && m_nConfigRight != NDEF)
// 			bWidthNotConfiged = false;
// 	}
// 
// 	if (bHeightNotConfiged)
// 	{
// 		if (m_nConfigTop != NDEF && m_nConfigBottom != NDEF)
// 			bHeightNotConfiged = false;
// 	}

	if (bWidthNotConfiged || bHeightNotConfiged)
		return true;

//     if (m_nConfigLayoutFlags & LAYOUT_FLAG_FORCE_DESIREDSIZE)
//         return true;

    return false;
}



}