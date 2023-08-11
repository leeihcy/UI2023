#include "include/inc.h"
#include "vertlayout.h"
#include "src/object/object.h"
#include "src/attribute/attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/attribute/flags_attribute.h"
#include "src/util/DPI/dpihelper.h"

namespace ui
{

VertLayout::VertLayout()
{
    m_nSpace = 0;
//	this->m_eDirection = LAYOUT_Vert_LEFTTORIGHT;   // 默认堆栈是从左到右
}
VertLayout::~VertLayout()
{
}
void  VertLayout::Serialize(SERIALIZEDATA* pData)
{
//     AttributeSerializer s(pData, TEXT("VertLayout"));
//
//     s.AddLong(XML_LAYOUT_VERT_GAP, this,
//         memfun_cast<pfnLongSetter>(&VertLayout::LoadGap),
//         memfun_cast<pfnLongGetter>(&VertLayout::SaveGap));
// 
}

void  VertLayout::LoadGap(long n)
{
    m_nSpace = ScaleByDpi_if_gt0(n);
}
long  VertLayout::SaveGap()
{
    return RestoreByDpi_if_gt0(m_nSpace);
}

void  VertLayout::SetSpace(int n)
{
    m_nSpace = n;
}

Size  VertLayout::Measure()
{
	Size s = { 0 };

	int nMaxWidth = 0;

	Object* pChild = nullptr;
	while ((pChild = m_pPanel->EnumChildObject(pChild)))
	{
		// 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
		VertLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
		if (!pParam)
			continue;

		if (pChild->IsSelfCollapsed())
			continue;

		if (pParam->m_eHeightType == AUTO)
		{
			s.height += pChild->GetDesiredSize().height;
		}
        else
		{
			s.height += pParam->m_nConfigHeight;
		} 

		Rect rMargin = { 0 };
		pChild->GetMarginRegion(&rMargin); 
		s.height += rMargin.top + rMargin.bottom;

		int nWidth = rMargin.left + rMargin.right;
        if (pParam->m_eWidthType == AUTO)
		{
			nWidth += pChild->GetDesiredSize().width;
		}
		else 
		{
			nWidth += pParam->m_nConfigWidth;
		}
        

        nMaxWidth = std::max(nMaxWidth, nWidth);
	}
	s.width = nMaxWidth;

	return s;
}

struct ObjLayoutInfo
{
	Object*  pObj;
	int width;  // 不包含margin
	int height; // 不包含margin
};

void  VertLayout::DoArrage(IObject* /*pIObjToArrage*/)
{
	// 计算每个子控件需要的宽度及其布局顺序
	int nChildCount = m_pPanel->GetChildCount();
	if (nChildCount <= 0)
		return;

	// 父控件内间距
	Rect rcPadding = { 0 };
	m_pPanel->GetPaddingRegion(&rcPadding);

	Rect rcParent;
	m_pPanel->GetObjectClientRect(&rcParent);

	std::vector<ObjLayoutInfo>  vecInfo(nChildCount);
	memset(&vecInfo[0], 0, sizeof(ObjLayoutInfo)*nChildCount);

	int nTopCursor = 0;  // 用于指定这个控件的布局顺序，居右对齐的得反着排序
	int nBottomCursor = nChildCount-1;

	int nNeedHeight = 0;
	int nAvgCount = 0;

	Object* pChild = nullptr;
	while ((pChild = m_pPanel->EnumChildObject(pChild)))
	{
		// 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
		VertLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
		if (!pParam)
			continue;

		if (pChild->IsSelfCollapsed())
			continue;

		//  计算布局顺序
		int nIndex = nTopCursor;
		if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_BOTTOM)
		{
			nIndex = nBottomCursor;
			nBottomCursor--;
		}
		else
		{
			nIndex = nTopCursor;
			nTopCursor++;
		}

		// 计算所需要的宽高
		int nObjWidth = 0;
		int nObjHeight = 0;

		if (pParam->IsSizedByContent())
		{
			Size s = pChild->GetDesiredSize();

			nObjWidth = s.width;// - pChild->GetMarginW();
			nObjHeight = s.height;// - pChild->GetMarginH();
		}

        if (pParam->m_eHeightType == WH_AVG)
		{
			// 本次循环结束后再计算
			nAvgCount++;
			nObjHeight = 0;
		}
		else if (pParam->m_eHeightType == WH_PERCENT)
		{
#if 0 // 比例中包含margin
            nObjHeight = rcParent.Height() * pParam->m_nConfigHeight / 100;
            nObjHeight -= pChild->GetMarginH();

#else // 比例中不包含margin
            nObjHeight = rcParent.Height() * pParam->m_nConfigHeight / 100;
#endif
		}
		else if (pParam->m_eHeightType != WH_AUTO)
		{
			nObjHeight = pParam->m_nConfigHeight;
		}
         

        if (pParam->m_eWidthType == WH_SET)
		{
			nObjWidth = pParam->m_nConfigWidth;
		}
        else if (pParam->m_eWidthType == WH_PERCENT)
        {
            nObjWidth = rcParent.Width() * pParam->m_nConfigWidth / 100;
        }

		ObjLayoutInfo& info = vecInfo[nIndex];
		info.pObj = pChild;
		info.width = nObjWidth;
		info.height = nObjHeight;

		nNeedHeight += nObjHeight + pChild->GetMarginH();
	}

	// 计算平均宽度
	int nAvgHeight = 0;
	int nAvgDiff = 0;  // 误差补足
	if (nAvgCount > 0)
	{
		int nTotal = (rcParent.Height()- nNeedHeight);
		nAvgHeight = ((rcParent.Height()-nNeedHeight) / nAvgCount);
		nAvgDiff = nTotal - (nAvgHeight * nAvgCount);
	}

	// 开始布局
	int nTopConsume = 0;
	int nBottomConsume = 0;

	for (int i = 0; i < nChildCount; i++)
	{
		ObjLayoutInfo& info = vecInfo[i];
		pChild = info.pObj;
		if (!pChild)
			continue;

		// 放在IsSelfCollapsed之前。editor中也需要加载隐藏对象的布局属性
		VertLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
		if (!pParam)
			continue;

		if (pChild->IsSelfCollapsed())
			continue;

		Rect rMargin = { 0 };
		pChild->GetMarginRegion(&rMargin);

		Rect rcObj;

		// 如果是平均宽度，为其宽度赋值
        if (pParam->m_eHeightType == WH_AVG)
		{
			info.height = nAvgHeight;
			info.height += nAvgDiff;
			nAvgDiff = 0;
		}

		// 计算y坐标
		if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_BOTTOM)
		{
			nBottomConsume += rMargin.bottom;

			rcObj.bottom = rcParent.bottom - nBottomConsume;
			rcObj.top = rcObj.bottom - info.height;

			nBottomConsume += info.height + rMargin.top;
		}
		else // if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_TOP)
		{
			nTopConsume += rMargin.top;

			rcObj.top = rcParent.top + nTopConsume;
			rcObj.bottom = rcObj.top + info.height;

			nTopConsume += info.height + rMargin.bottom;
		}

		// 计算x坐标
		if ((pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_LEFT) &&
			(pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_RIGHT))
		{
			rcObj.left = rcParent.left + rMargin.left;
			rcObj.right = rcParent.right - rMargin.right;
		}
		else if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_RIGHT)
		{
			rcObj.right = rcParent.right - rMargin.right;
			rcObj.left = rcObj.right - info.width;
		}
		else if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_CENTER)
		{
			rcObj.left = rcParent.left + (rcParent.Width() - info.width) / 2;
			rcObj.left = rcObj.left + rMargin.left - rMargin.right;
			rcObj.right = rcObj.left + info.width;
		}
		else // if (pParam->m_nConfigLayoutFlags & LAYOUT_ITEM_ALIGN_LEFT)
		{
			rcObj.left = rcParent.left + rMargin.left;
			rcObj.right = rcObj.left + info.width;
		}

		pChild->SetObjectPos(
			&rcObj,
			SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);
	}

}

void  VertLayout::ChildObjectVisibleChanged(IObject* pObj)
{
    UIASSERT (pObj);
	UIASSERT(pObj->GetParentObject());
	UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

    SetDirty(true);
    m_pPanel->Invalidate();
}

void  VertLayout::ChildObjectContentSizeChanged(IObject* pObj)
{
    UIASSERT(pObj);
    UIASSERT(pObj->GetParentObject());
    UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

    SetDirty(true);
    m_pPanel->Invalidate();
}

//////////////////////////////////////////////////////////////////////////

VertLayoutParam::VertLayoutParam()
{
    m_nConfigWidth = AUTO;
    m_nConfigHeight = AUTO;
    m_nConfigLayoutFlags = 0;
	m_eWidthType = WH_SET;
	m_eHeightType = WH_SET;
}
VertLayoutParam::~VertLayoutParam()
{

}

void  VertLayoutParam::UpdateByRect()
{
    Rect  rcParent;
    m_pObj->GetParentRect(&rcParent);

    if (m_nConfigWidth >= 0)
    {
        m_nConfigWidth = rcParent.Width();
    }
    if (m_nConfigHeight >= 0)
    {
        m_nConfigHeight = rcParent.Height();
    }
}

void  VertLayoutParam::Serialize(SERIALIZEDATA* pData)
{
    AttributeSerializer s(pData, TEXT("VertLayoutParam"));

    s.AddString(
            XML_WIDTH,
            Slot(&VertLayoutParam::LoadConfigWidth, this),
            Slot(&VertLayoutParam::SaveConfigWidth, this)
        )
		->SetDefault(XML_AUTO)
        ->SetCompatibleKey(XML_LAYOUT_PREFIX XML_WIDTH);

    s.AddString(
            XML_HEIGHT,
            Slot(&VertLayoutParam::LoadConfigHeight, this),
            Slot(&VertLayoutParam::SaveConfigHeight, this)
        )
		->SetDefault(XML_AUTO)
		->SetCompatibleKey(XML_LAYOUT_PREFIX XML_HEIGHT);;

    s.AddFlags(XML_LAYOUT_ITEM_ALIGN, m_nConfigLayoutFlags)
        ->AddFlag(LAYOUT_ITEM_ALIGN_LEFT, XML_LAYOUT_ITEM_ALIGN_LEFT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_RIGHT, XML_LAYOUT_ITEM_ALIGN_RIGHT)
        ->AddFlag(LAYOUT_ITEM_ALIGN_TOP, XML_LAYOUT_ITEM_ALIGN_TOP)
        ->AddFlag(LAYOUT_ITEM_ALIGN_BOTTOM, XML_LAYOUT_ITEM_ALIGN_BOTTOM)
        ->AddFlag(LAYOUT_ITEM_ALIGN_CENTER, XML_LAYOUT_ITEM_ALIGN_CENTER)
        ->AddFlag(LAYOUT_ITEM_ALIGN_VCENTER, XML_LAYOUT_ITEM_ALIGN_VCENTER)
        ->AddFlag(LAYOUT_ITEM_ALIGN_FILL, XML_LAYOUT_ITEM_ALIGN_FILL);
}

long  VertLayoutParam::GetConfigWidth()
{
    return m_nConfigWidth;
}
void  VertLayoutParam::SetConfigWidth(long n)
{
    m_nConfigWidth = n;
}
void  VertLayoutParam::LoadConfigWidth(const wchar_t* szText)
{
    LoadConfigWH(szText, m_nConfigWidth, m_eWidthType);
}
const wchar_t*  VertLayoutParam::SaveConfigWidth()
{
    return SaveConfigWH(m_nConfigWidth, m_eWidthType);
}

long  VertLayoutParam::GetConfigHeight()
{
    return m_nConfigHeight;
}
void  VertLayoutParam::SetConfigHeight(long n)
{
    m_nConfigHeight = n;
}
void  VertLayoutParam::LoadConfigHeight(const wchar_t* szText)
{
    LoadConfigWH(szText, m_nConfigHeight, m_eHeightType);
}
const wchar_t*  VertLayoutParam::SaveConfigHeight()
{
    return SaveConfigWH(m_nConfigHeight, m_eHeightType);
}

void  VertLayoutParam::SetConfigLayoutFlags(long n)
{
    m_nConfigLayoutFlags = n;
}
long  VertLayoutParam::GetConfigLayoutFlags()
{
    return m_nConfigLayoutFlags;
}

Size  VertLayoutParam::CalcDesiredSize()
{
    Size size = {0,0};

    if (IsSizedByContent())
    {
        // 获取子对象所需要的空间
        m_pObj->SendMessage(UI_MSG_GETDESIREDSIZE, (long)&size);

        // 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
        if (this->m_eWidthType != AUTO)
            size.width = this->m_nConfigWidth;
        if (this->m_eHeightType != AUTO)
            size.height = this->m_nConfigHeight;
    }
    else
    {
        size.width = this->m_nConfigWidth;
        size.height = this->m_nConfigHeight;
    }

    // 计算 margin 的大小
//     size.width += m_pObj->GetMarginW();
//     size.height += m_pObj->GetMarginH();

    return size;
}

bool  VertLayoutParam::IsSizedByContent()
{
    if (m_eWidthType != AUTO && m_eHeightType != AUTO)
        return false;

    return true;
}
}