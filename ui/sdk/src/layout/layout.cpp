#include "layout.h"
#include "include/inc.h"

#include "include/interface/iattributemap.h"
#include "src/layout/canvaslayout.h"
#include "src/object/object.h"
#include "src/util/DPI/dpihelper.h"
#include "src/window/window.h"

namespace ui {

#if 0

Size  DockLayout::MeasureChildObject()
{
	Size size = {0,0};

	// 1. 先获取center对象的SIZE
	Object*  pCenterObj = nullptr;
	Object*  pChild = nullptr;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
#if 1
		int nFlag = pChild->GetConfigLayoutFlags();
		if (nFlag & LAYOUT_ITEM_ALIGN_CENTER)
			pCenterObj = pChild;
		else if (0 == (nFlag&LAYOUT_ITEM_ALIGN_LTRB))
			pCenterObj = pChild;
#else
		std::string  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);

		if (XML_LAYOUT_DOCK_DOCK_CENTER == strDock ||
			strDock.empty() )
		{
			pCenterObj = pChild;
			break;
		}
#endif
	}

	if (pCenterObj)
		size = pCenterObj->GetDesiredSize();

	// 2. 从后开始遍历停靠在四周的子对象的大小
	pChild = nullptr;
	while (pChild = this->m_pPanel->REnumChildObject(pChild))
	{
		if (!pChild->IsVisible())
			continue;

		int nFlag = pChild->GetConfigLayoutFlags();

#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_LEFT || LAYOUT_ITEM_ALIGN_RIGHT)
#else
		std::string  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);

		if (XML_LAYOUT_DOCK_DOCK_LEFT == strDock ||
			XML_LAYOUT_DOCK_DOCK_RIGHT == strDock)
#endif
		{
			Size s = pChild->GetDesiredSize();
			if (s.height > size.height)
				size.height = s.height;
			size.width += s.width;
		}
		else
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_TOP || LAYOUT_ITEM_ALIGN_BOTTOM)
#else
		if (XML_LAYOUT_DOCK_DOCK_TOP == strDock ||
			 XML_LAYOUT_DOCK_DOCK_BOTTOM == strDock)
#endif
		{
			Size s = pChild->GetDesiredSize();
			if (s.width > size.width)
				size.width = s.width;
			size.height += s.height;
		}

		else
		{
			continue;
		}

	}

	return size;
}
void  DockLayout::ArrangeChildObject(Object* pObjToArrage)
{
	// 调用该函数时，自己的大小已经被求出来了
	int  nWidth  = m_pPanel->GetWidth();
	int  nHeight = m_pPanel->GetHeight();

	int  nComsumeTop    = this->m_pPanel->GetPaddingT();
	int  nComsumeLeft   = this->m_pPanel->GetPaddingL();
	int  nComsumeBottom = this->m_pPanel->GetPaddingB();
	int  nComsumeRight  = this->m_pPanel->GetPaddingR();

	Object* pCenterObj  = nullptr;  // 记录是否已经设置过 dock布局中的 center 对象了（因为只允许有一个center对象）
	
	// arrange的时候要从前往后开始排布
	Object* pChild = nullptr;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
		if( !pChild->IsVisible() )
			continue;

#if 1
		int nFlag = pChild->GetConfigLayoutFlags();
#else
		std::string  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);
#endif

		// 计算出 pChild 的 rectP
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_LEFT)
#else
		if (XML_LAYOUT_DOCK_DOCK_LEFT == strDock)
#endif
		{
			Size s = pChild->GetDesiredSize();
			
			Rect rcObj( 
				nComsumeLeft             +  pChild->GetMarginL(), 
				nComsumeTop              +  pChild->GetMarginT(), 
				nComsumeLeft + s.width      -  pChild->GetMarginR(), 
				nHeight - nComsumeBottom -  pChild->GetMarginB());
			pChild->SetObjectPos(&rcObj, SWP_NO_REDRAW);

			nComsumeLeft += s.width;
		}
		else
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_RIGHT)
#else
		if (XML_LAYOUT_DOCK_DOCK_RIGHT == strDock)
#endif
		{
			Size s = pChild->GetDesiredSize();
			
			Rect rcObj( 
				nWidth - nComsumeRight - s.width  +  pChild->GetMarginL(), 
				nComsumeTop                    +  pChild->GetMarginT(), 
				nWidth - nComsumeRight         -  pChild->GetMarginR(), 
				nHeight - nComsumeBottom       -  pChild->GetMarginB());
			pChild->SetObjectPos(&rcObj, SWP_NO_REDRAW);

			nComsumeRight += s.width;
		}
		else
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_TOP)
#else
		if (XML_LAYOUT_DOCK_DOCK_TOP == strDock)
#endif
		{
			Size s = pChild->GetDesiredSize();

			Rect rcObj( 
				nComsumeLeft            +  pChild->GetMarginL() , 
				nComsumeTop             +  pChild->GetMarginT() , 
				nWidth - nComsumeRight  -  pChild->GetMarginR() , 
				nComsumeTop + s.height      -  pChild->GetMarginB() );
			pChild->SetObjectPos(&rcObj, SWP_NO_REDRAW);

			nComsumeTop += s.height;
		}
		else
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_BOTTOM)
#else
		if (XML_LAYOUT_DOCK_DOCK_BOTTOM == strDock)
#endif
		{
			Size s = pChild->GetDesiredSize();

			Rect rcObj(
				nComsumeLeft                    +  pChild->GetMarginL() , 
				nHeight - nComsumeBottom - s.height +  pChild->GetMarginT() , 
				nWidth - nComsumeRight          -  pChild->GetMarginR() , 
				nHeight - nComsumeBottom        -  pChild->GetMarginB() );
			pChild->SetObjectPos(&rcObj, SWP_NO_REDRAW);

			nComsumeBottom += s.height;	 
		}
		else  // 其他都归为center对象
		{
			if( nullptr == pCenterObj )
			{
				pCenterObj = pChild;
				continue;
			}
		}
	}

	if( pCenterObj )
	{
		pCenterObj->SetObjectPos(
			nComsumeLeft                            +  pCenterObj->GetMarginL() , 
			nComsumeTop                             +  pCenterObj->GetMarginT() , 
			nWidth  - nComsumeRight  - nComsumeLeft -  pCenterObj->GetMarginR() , 
			nHeight - nComsumeBottom - nComsumeTop  -  pCenterObj->GetMarginB() ,
			SWP_NO_REDRAW);
	}
}

#endif

void LoadConfigWH(const char *szText, int &wh, int &whtype) {
  if (!szText || !szText[0] || 0 == strcmp(szText, XML_AUTO)) {
    whtype = WH_AUTO;
    wh = 0;
    return;
  }

  int nLength = (int)strlen(szText);
  if (szText[nLength - 1] == XML_AVERAGE_CHAR) {
    // 只支持平均分配，不支持平均系数
    whtype = WH_AVG;
    wh = 0;
    return;
  }

  if (szText[nLength - 1] == XML_PERCENT_CHAR) {
    std::string str(szText);
    str[nLength - 1] = 0;

    whtype = WH_PERCENT;
    wh = atoi(str.c_str());
    return;
  }

  wh = ScaleByDpi(atoi(szText));
  whtype = WH_SET;
}

const char *SaveConfigWH(int wh, int whtype) {
  char *szBuffer = GetTempBuffer();

  switch (whtype) {
  case WH_AUTO:
    return nullptr;

  case WH_SET: {
    snprintf(szBuffer, 64, "%d", RestoreByDpi(wh));
    return szBuffer;
  }

  case WH_AVG: {
    szBuffer[0] = XML_AVERAGE_CHAR;
    return szBuffer;
  }

  case WH_PERCENT: {
    snprintf(szBuffer, 64, "%d%%", wh);
    return szBuffer;
  }
  }
  return nullptr;
}

} // namespace ui