#include "include/inc.h"
#include "cursorres.h"
#include "src/resource/res_bundle.h"
#include "src/resource/uicursor.h"
#include "include/interface/iuires.h"
#include "include/interface/imapattr.h"
//#include "other/resource.h"

namespace ui
{

//////////////////////////////////////////////////////////////////////////
//
//	CursorResItem
//
// 
// UICursor::UICursor()
// {
// 	m_dwRef = 0;
// 	m_hCursor = nullptr;
// 
// 	m_pOutRef = nullptr;
// }
// UICursor::~UICursor()
// {
// 	*m_pOutRef = nullptr;
// 	m_pOutRef = nullptr;
// 
// 	UIASSERT(nullptr == m_hCursor);
// }
// 
// void UICursor::CreateInstance(const std::string strCurFilePath, UICursor** ppOutRef)
// {
// 	if (nullptr == ppOutRef)
// 		return;
// 
// 	UICursor* pCursor = new UICursor();
// 	
// 	bool bRet = pCursor->Load(strCurFilePath.c_str());
// 	if (false == bRet)
// 	{
// 		UI_LOG_WARN("%s LoadCursor Failed. filepath=%s", FUNC_NAME, strCurFilePath.c_str());
// 		SAFE_DELETE(pCursor);
// 		*ppOutRef = nullptr;
// 	}
// 	else 
// 	{
// 		*ppOutRef = pCursor;
// 	}
// }
// void UICursor::CreateInstance(LPTSTR nCursorID, HINSTANCE hInstance, UICursor** ppOutRef)
// {
// 	if (nullptr == ppOutRef)
// 		return;
// 
// 	UICursor* pCursor = new UICursor();
// 	bool bRet = pCursor->Load(nCursorID, hInstance);
// 	if (false == bRet)
// 	{
// 		UI_LOG_WARN("%s LoadCursor Failed. cursor id=%s, hInstance=%x"), FUNC_NAME, nCursorID, hInstance);
// 		SAFE_DELETE(pCursor);
// 		*ppOutRef = nullptr;
// 	}
// 	else
// 	{
// 		*ppOutRef = pCursor;
// 	}
// }
// HCURSOR UICursor::GetCursor()
// { 
// 	return m_hCursor; 
// }
// 
// bool UICursor::Load(const char* szCursorFile)
// {
// 	if (nullptr == szCursorFile)
// 		return false;
// 
// 	if (m_hCursor)
// 	{
// 		DestroyCursor(m_hCursor);
// 		m_hCursor = nullptr;
// 	}
// 
// 	m_hCursor = (HCURSOR)::LoadImage(nullptr, szCursorFile, IMAGE_CURSOR, 0,0, LR_DEFAULTSIZE|LR_LOADFROMFILE);
// 	return m_hCursor!=nullptr ? true:false;
// }
// 
// bool  UICursor::Load(LPTSTR nCursorID, HINSTANCE hInstance)
// {
// 	if (m_hCursor)
// 	{
// 		DestroyCursor(m_hCursor);
// 		m_hCursor = nullptr;
// 	}
// 
// 	m_hCursor = ::LoadCursor(hInstance, nCursorID );
// 	return m_hCursor!=nullptr ? true:false;
// }
// void  UICursor::Attach(HCURSOR hCursor)
// {
// 	UIASSERT( nullptr != hCursor );
// 	m_hCursor = hCursor;
// }
// HCURSOR  UICursor::Detach()
// {
// 	UIASSERT( nullptr!= m_hCursor );
// 	HCURSOR hOldCursor = m_hCursor;
// 	m_hCursor = nullptr;
// 
// 	return hOldCursor;
// }
// long  UICursor::AddRef()
// {
// 	m_dwRef++;
// 	return m_dwRef;
// }
// long  UICursor::Release()
// {
// 	m_dwRef--;
// 	if (0 >= m_dwRef )
// 	{
// 		if (m_hCursor)
// 		{
// 			DestroyCursor(m_hCursor);
// 			m_hCursor = nullptr;
// 		}
// 		m_dwRef = 0;
// 
// 		delete this;
// 		return 0;
// 	}
// 	return m_dwRef;
// }

//////////////////////////////////////////////////////////////////////////
CursorResItem::CursorResItem()
{
	m_pCursor = nullptr;
	m_pICursorResItem = nullptr;
};
CursorResItem::~CursorResItem()
{
	SAFE_DELETE(m_pICursorResItem);
}

IUICursor*  CursorResItem::GetCursor()
{
	if (nullptr == m_pCursor)
	{
		if (m_strCurFilePath.empty())
		{
			if (m_strID == XML_CURSOR_IDC_ARROW)
				UICursor::CreateInstance2(IDC_ARROW, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_IBEAM)
				UICursor::CreateInstance2(IDC_IBEAM, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_WAIT)
				UICursor::CreateInstance2(IDC_WAIT, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_CROSS)
				UICursor::CreateInstance2(IDC_CROSS, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_UPARROW)
				UICursor::CreateInstance2(IDC_UPARROW, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZE)
				UICursor::CreateInstance2(IDC_SIZE, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_ICON)
				UICursor::CreateInstance2(IDC_ICON, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZENWSE)
				UICursor::CreateInstance2(IDC_SIZENWSE, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZENESW)
				UICursor::CreateInstance2(IDC_SIZENESW, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZEWE)
				UICursor::CreateInstance2(IDC_SIZEWE, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZENS)
				UICursor::CreateInstance2(IDC_SIZENS, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_SIZEALL)
				UICursor::CreateInstance2(IDC_SIZEALL, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_NO)
				UICursor::CreateInstance2(IDC_NO, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_HAND)
				UICursor::CreateInstance2(IDC_HAND, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_APPSTARTING)
				UICursor::CreateInstance2(IDC_APPSTARTING, nullptr, &m_pCursor);
			else if (m_strID == XML_CURSOR_IDC_HELP)
				UICursor::CreateInstance2(IDC_HELP, nullptr, &m_pCursor);
// 			else if (m_strID == XML_CURSOR_IDC_DRAG_SEPERATOR)
// 				UICursor::CreateInstance2(MAKEINTRESOURCE(IDC_CURSOR_DRAG_SEPERATOR), /*Application::GetModuleInstance()*/g_hInstance, &m_pCursor);
// 			else if (m_strID == XML_CURSOR_IDC_SPLITBAR_H)
// 				UICursor::CreateInstance2(MAKEINTRESOURCE(IDC_CURSOR_SPLITBAR_H), /*Application::GetModuleInstance()*/g_hInstance, &m_pCursor);
// 			else if (m_strID == XML_CURSOR_IDC_SPLITBAR_V)
// 				UICursor::CreateInstance2(MAKEINTRESOURCE(IDC_CURSOR_SPLITBAR_V), /*Application::GetModuleInstance()*/g_hInstance, &m_pCursor);
			else
				return false;
		}
		else
		{
			UICursor::CreateInstance2(m_strCurFilePath, &m_pCursor);
		}
		m_pCursor->SetOutRef(&m_pCursor);
	}
	else
	{
		m_pCursor->AddRef();
	}

	return static_cast<IUICursor*>(m_pCursor);
}
bool  CursorResItem::ModifyCursor( const std::string& strCurFilePath )
{
	UIASSERT( false && "TODO: CursorResItem::ModifyCursor not implement");
	return false;
}
ICursorResItem*  CursorResItem::GetICursorResItem()
{
	if (nullptr == m_pICursorResItem)
	{
		m_pICursorResItem = new ICursorResItem(this);
	}
	return m_pICursorResItem;
}

CursorRes::CursorRes()
{
	m_pICursorRes = nullptr;

	// 添加默认的系统图标
	this->InsertCursor( XML_CURSOR_IDC_ARROW, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_IBEAM, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_WAIT, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_CROSS, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_UPARROW, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_SIZE, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_ICON, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_SIZENWSE, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_SIZENESW, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_SIZEWE, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_SIZENS, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_SIZEALL, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_NO, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_HAND, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_APPSTARTING, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_HELP, _T(""), nullptr);

	// UIDLL 资源
	this->InsertCursor( XML_CURSOR_IDC_DRAG_SEPERATOR, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_SPLITBAR_H, _T(""), nullptr);
	this->InsertCursor( XML_CURSOR_IDC_SPLITBAR_V, _T(""), nullptr);
}
CursorRes::~CursorRes()
{
	this->Clear();
	SAFE_DELETE (m_pICursorRes);
}

ICursorRes*  CursorRes::GetICursorRes()
{
	if (nullptr == m_pICursorRes)
		m_pICursorRes = new ICursorRes(this);

	return m_pICursorRes;
}

CursorResItem* CursorRes::GetCursorItem( int nIndex )
{
	if (nIndex < 0)
		return nullptr;
	if (nIndex >= (int)m_vCursors.size())
		return nullptr;

	return m_vCursors[nIndex];
}	

CursorResItem*  CursorRes::GetCursorItem(UICursor* p)
{
	if (!p)
		return nullptr;

	vector<CursorResItem*>::iterator  iter = m_vCursors.begin();
	for (; iter != m_vCursors.end(); iter++)
	{
		if ((*iter)->m_pCursor == p)
			return (*iter);
	}

	return nullptr;
}

CursorResItem* CursorRes::GetCursorItem(const char* szId)
{
	vector<CursorResItem*>::iterator  iter = m_vCursors.begin();
	for (; iter != m_vCursors.end(); ++iter)
	{
		CursorResItem* p = *iter;
		if (0 == strcmp(p->GetId(), szId))
		{
			return p;
		}
	}
	return nullptr;
}
int   CursorRes::GetCursorCount() 
{
	return (int)m_vCursors.size();
}

void CursorRes::GetCursor(const char* szCursorId, IUICursor** pp)
{
	if (nullptr == szCursorId  || nullptr == pp)
		return;

	CursorResItem* p = this->GetCursorItem(szCursorId);
	if (nullptr == p)
	{
		UI_LOG_ERROR("failed, id=%s", szCursorId);
		return;
	}

	*pp = p->GetCursor();
}

const char*  CursorRes::GetCursorId(IUICursor* p)
{
	if (!p)
		return nullptr;

	CursorResItem*  pItem = GetCursorItem(static_cast<UICursor*>(p));
	if (pItem)
		return pItem->GetId();

	return nullptr;
}

CursorResItem*  CursorRes::LoadItem(IAttributeMap* attribute_map, const char* szFullPath)
{
	if (nullptr == szFullPath)
		return nullptr;

	const char* szId = attribute_map->GetAttr(XML_ID, true);

	CursorResItem* pItem = nullptr;
	if (this->InsertCursor(szId, szFullPath, &pItem))
	{
		pItem->SetAttribute(attribute_map);
		return pItem;
	}
	else
	{
		UI_LOG_WARN("insert cursor m_strID=%s, path=%s failed.", szId, szFullPath);
		return nullptr;
	}
}

bool  CursorRes::InsertCursor(const char* szId, const char* szCurFilePath, CursorResItem** ppItem)
{
	CursorResItem* p = this->GetCursorItem(szId);
	if (p)
	{
		UI_LOG_WARN("CursorRes::InsertCursor failed, insert item=%s, cursor=%s", szId, szCurFilePath);
		return false;
	}

	CursorResItem*  pCursorItem = new CursorResItem;
	pCursorItem->SetId( szId );
	pCursorItem->SetCurFilePath(szCurFilePath);

	this->m_vCursors.push_back(pCursorItem);  
	if (ppItem)
	{
		*ppItem = pCursorItem;
	}
	return true;
}
bool  CursorRes::ModifyCursor( const char* szId, const char* szCursor )
{
	CursorResItem* p = this->GetCursorItem(szId);
	if (p)
	{
		p->ModifyCursor(szCursor);
		return true;
	}

	return false;
}

bool  CursorRes::RemoveCursor( const char* szId )
{
	if (nullptr == szId)
		return false;

	vector<CursorResItem*>::iterator  iter = m_vCursors.begin();
	vector<CursorResItem*>::iterator  iterEnd = m_vCursors.end();

	for (; iter != iterEnd; iter++)
	{
		CursorResItem* p = *iter;
		if (0 == strcmp(p->GetId(), szId))
		{
			delete p;
			p = nullptr;
			m_vCursors.erase(iter);
			return true;
		}
	}

	return false;
}
void  CursorRes::Clear()
{
	vector<CursorResItem*>::iterator  iter = m_vCursors.begin();
	vector<CursorResItem*>::iterator  iterEnd = m_vCursors.end();

	for (; iter != iterEnd; iter++)
	{
		CursorResItem* p = *iter;
		delete p;
		p = nullptr;
	}

	m_vCursors.clear();
}


}