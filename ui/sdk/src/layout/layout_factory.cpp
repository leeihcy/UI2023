#include "include/inc.h"
#include "layout_factory.h"

#include "src/layout/layout.h"
#include "src/layout/horzlayout.h"
#include "src/layout/vertlayout.h"
#include "src/layout/canvaslayout.h"
#include "src/layout/cardlayout.h"
#include "src/layout/gridlayout.h"
#include "src/layout/averagelayout.h"

namespace ui {

LayoutFactory::LayoutFactory()
{
	
}

LayoutFactory::~LayoutFactory()
{
	Clear();
}

void  LayoutFactory::Init()
{
	Register(XML_LAYOUT_CANVAS, LAYOUT_TYPE_CANVAS, CanvasLayout::CreateInstance);
	Register(XML_LAYOUT_HORZ,   LAYOUT_TYPE_HORZ, HorzLayout::CreateInstance);
	Register(XML_LAYOUT_VERT,   LAYOUT_TYPE_VERT, VertLayout::CreateInstance);
	Register(XML_LAYOUT_CARD,   LAYOUT_TYPE_CARD, CardLayout::CreateInstance);
	Register(XML_LAYOUT_GRID,   LAYOUT_TYPE_GRID, GridLayout::CreateInstance);
	Register(XML_LAYOUT_AVERAGE, LAYOUT_TYPE_AVERAGE, AverageLayout::CreateInstance);
}

bool  LayoutFactory::Register(const wchar_t* szName, int nType, pfnUICreateLayoutPtr pfun)
{
	if (nullptr == pfun || nullptr == szName)
	{
		UI_LOG_WARN(_T("invalid arg. name=%s, pfun=%x"), szName, pfun);
		return false;
	}

	int nSize = (int)m_vecData.size();
	for (int i = 0; i < nSize; i++)
	{
		UILAYOUT_CREATE_INFO* pData = m_vecData[i];
		if (pData && (pData->m_strName == szName || pData->m_nLayoutType == nType))
		{
			UI_LOG_WARN(_T("register duplicate. name=%s, type=%d, pfun=%x"), szName, nType, pfun);
			return false;
		}
	}

	UILAYOUT_CREATE_INFO* pData = new UILAYOUT_CREATE_INFO;
	pData->m_strName = szName;
	pData->m_nLayoutType = nType;
	pData->m_func = pfun;

	m_vecData.push_back(pData);
	return true;
}


bool  LayoutFactory::Create(int nType, IObject* pObject, ILayout** ppOut)
{
	if (nullptr == ppOut)
		return false;

	pfnUICreateLayoutPtr  pFunc = nullptr;
	UILAYOUT_CREATE_DATA::iterator iter = m_vecData.begin();
	for ( ; iter != m_vecData.end(); iter++ )
	{
		UILAYOUT_CREATE_INFO* pData = *iter;
		if (pData && pData->m_nLayoutType == nType)
		{
			pFunc = pData->m_func;
			break;
		}
	}

	if (nullptr == pFunc)
		return false;

	pFunc(pObject, ppOut);
	return true;
}
bool  LayoutFactory::CreateByName(const wchar_t* szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut)
{
	if (nullptr == ppOut)
		return false;

	pfnUICreateLayoutPtr  pFunc = nullptr;
	if (szName)  // 为nullptr有可能是没有配置layout属性，默认创建一个canvas布局
	{
		UILAYOUT_CREATE_DATA::iterator iter = m_vecData.begin();
		for ( ; iter != m_vecData.end(); iter++ )
		{
			UILAYOUT_CREATE_INFO* pData = *iter;
			if (pData && pData->m_strName == szName)
			{
				pFunc = pData->m_func;
				break;
			}
		}
	}

	if (nullptr == pFunc && bCreateDefault && m_vecData.size() > 0)
		pFunc = m_vecData[0]->m_func;

	if (nullptr == pFunc)
		return false;

	pFunc(pObject, ppOut);
	return true;
}

void  LayoutFactory::EnumLayoutType(pfnEnumLayoutTypeCallback callback, long wParam, long lParam)
{
	if (!callback)
		return;

	UILAYOUT_CREATE_DATA::iterator iter = m_vecData.begin();
	for ( ; iter != m_vecData.end(); iter++ )
	{
		if (!callback((*iter)->m_nLayoutType, (*iter)->m_strName.c_str(), wParam, lParam))
			break;
	}
}

void  LayoutFactory::Clear()
{
	UILAYOUT_CREATE_DATA::iterator iter = m_vecData.begin();
	for ( ; iter != m_vecData.end(); iter++ )   
	{
		SAFE_DELETE(*iter);               
	}
	m_vecData.clear();                          
}

}