#include "include/inc.h"
#include "averagelayout.h"
#include "src/object/object.h"
#include "include/interface/imapattr.h"

namespace ui
{

AverageLayout::AverageLayout()
{
    m_nGap = 0;
	this->direction = LAYOUT_AVERAGE_H;   // 默认堆栈是从左到右
}
AverageLayout::~AverageLayout()
{
}

void  AverageLayout::Serialize(SERIALIZEDATA* pData) 
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    if (nullptr == pMapAttrib)
        return;

	const wchar_t* szDirection = pMapAttrib->GetAttr(XML_LAYOUT_AVERAGE_DIRECTION, true);
	if (nullptr == szDirection)
        return;

    if (0 == wcscmp(XML_LAYOUT_AVERAGE_H, szDirection))
    {
        this->direction = LAYOUT_AVERAGE_H;
    }
    else if (0 == wcscmp(XML_LAYOUT_AVERAGE_V, szDirection))
    {
        this->direction = LAYOUT_AVERAGE_V;
    }
    else
    {
        UI_LOG_WARN( _T("AverageLayout::SetAttribute，属性%s=\"%s\"的值不正确"), XML_LAYOUT_AVERAGE_DIRECTION, szDirection);
    }

    pMapAttrib->GetAttr_int(XML_LAYOUT_AVERAGE_GAP, true, &m_nGap);
}

SIZE  AverageLayout::Measure()
{
	SIZE  size = {0,0};

	// 通过子控件来获取自己所需要的大小
	Object*  pChild = nullptr;
	while ((pChild = this->m_pPanel->EnumChildObject(pChild)))
	{
        if (pChild->IsSelfCollapsed())
        {
            continue;
        }

        AverageLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;
        }

		SIZE  s = pChild->GetDesiredSize();
		

		switch (this->direction)
		{
		case LAYOUT_AVERAGE_H:
			size.cx += s.cx + m_nGap;
			if (size.cy < s.cy)
				size.cy = s.cy;
			break;

		case LAYOUT_AVERAGE_V:
			size.cy += s.cy + m_nGap;
			if (size.cx < s.cx)
				size.cx = s.cx;
			break;
		}
	}

	return size;
}

void  AverageLayout::DoArrage(IObject* pIObjToArrage)
{
	int  nConsume1 = 0; // 当前已消耗的高度或宽度（从left/top开始计数）
    int  nChildCount = 0;
    int  nAverageSize = 0;
    int  nDiff = 0;  // 用于解决整除误差。导致没有占满整行。

    // 计算平均宽高
    RECT rcClient;
    m_pPanel->GetClientRectInObject(&rcClient);
    int  nPanelWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nPanelHeight = rcClient.Height(); //m_pPanel->GetHeight();

    Object* pChild = nullptr;
    while ((pChild = m_pPanel->EnumChildObject(pChild)))
    {
        if (!pChild->IsSelfCollapsed())
            nChildCount++;
    }
    if (0 == nChildCount)
        return;

    if (direction == LAYOUT_AVERAGE_H)
    {
        nAverageSize = (nPanelWidth - m_nGap*(nChildCount-1)) / nChildCount;
        nDiff = nPanelWidth%nChildCount;
    }
    else
    {
        nAverageSize = (nPanelHeight - m_nGap*(nChildCount-1)) / nChildCount;
        nDiff = nPanelHeight%nChildCount;
    }    

    int nIndex = 0;
	while ((pChild = m_pPanel->EnumChildObject(pChild)))
	{
		if (pChild->IsSelfCollapsed())
			continue;
        nIndex ++;

		switch (this->direction)
		{    
		case LAYOUT_AVERAGE_H:
            {
                RECT  rcChild;
                rcChild.left = nConsume1;
                rcChild.right = rcChild.left + nAverageSize;
                rcChild.top = 0;
                rcChild.bottom = nPanelHeight;

                if (nIndex == nChildCount)
                {
                    rcChild.right += nDiff;
                }

                ArrangeObject_H(pChild, &rcChild);
                nConsume1 += nAverageSize + m_nGap;
            }
			break;

		case LAYOUT_AVERAGE_V:
            {
                RECT  rcChild;
                rcChild.top = nConsume1;
                rcChild.bottom = rcChild.top + nAverageSize;
                rcChild.left = 0;
                rcChild.right = nPanelWidth;

                if (nIndex == nChildCount)
                {
                    rcChild.bottom += nDiff;
                }

                ArrangeObject_V(pChild, &rcChild);
                nConsume1 += nAverageSize + m_nGap;
            }
			break;
		}
	}
}


void  AverageLayout::ArrangeObject_H(Object* pChildObj, RECT* prc)
{
    pChildObj->SetObjectPos(
        prc->left + pChildObj->GetMarginL(), 
        prc->top + m_pPanel->GetPaddingT(), 
        prc->Width() - pChildObj->GetMarginW(), 
        prc->Height() - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);
}


void  AverageLayout::ArrangeObject_V(Object* pChildObj, RECT* prc)
{
    pChildObj->SetObjectPos(
        prc->left + pChildObj->GetMarginL(), 
        prc->top + m_pPanel->GetPaddingT(), 
        prc->Width() - pChildObj->GetMarginW(), 
        prc->Height() - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS|SWP_FORCESENDSIZEMSG);
}

void  AverageLayout::ChildObjectVisibleChanged(IObject* pObj)
{
    UIASSERT (pObj);
	UIASSERT(pObj->GetParentObject());
	UIASSERT(pObj->GetParentObject()->GetImpl() == m_pPanel);

    SetDirty(true);
    m_pPanel->Invalidate();
}


}