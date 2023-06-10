#include "StdAfx.h"
#include "skinmanager.h"
#include "Inc\Interface\iuires.h"
#include "skinres.h"
#include "Inc\Interface\iuiinterface.h"
#include "..\Base\Application\uiapplication.h"
#include "..\Helper\topwindow\topwindowmanager.h"
#include "..\SkinParse\datasource\skindatasource.h"

SkinManager::SkinManager()
{
    m_pISkinManager = nullptr;
	m_bDirty = false;

	m_strLanguage = TEXT("zh_cn");
    // m_strLanguage = TEXT("en_us");
	m_pUIApplication = nullptr;
}
SkinManager::~SkinManager()
{
    SAFE_DELETE(m_pISkinManager);
}

ISkinManager&  SkinManager::GetISkinManager()
{ 
    if (!m_pISkinManager)
        m_pISkinManager = new ISkinManager(this);
    
    return *m_pISkinManager; 
}

// ������UIApplication��������������ǰ�ͷ�
void  SkinManager::Destroy()
{
	//////////////////////////////////////////////////////////////////////////
	// �ͷŸ�Ƥ�������ڴ�

	vector<SkinRes*>::iterator  iter = m_vSkinRes.begin();
	vector<SkinRes*>::iterator  iterEnd = m_vSkinRes.end();

	for ( ; iter != iterEnd; iter++ )
	{
		SkinRes* p = (*iter);
		delete p; 
	}
	m_vSkinRes.clear();
}

// ������Ŀ¼�µ�Ƥ���б�
/* ����
void  SkinManager::SetSkinDirection(LPCTSTR szDir)
{
    if (nullptr == szDir || 0 == _tcslen(szDir))
        return;

    m_strSkinDir = szDir;

    WIN32_FIND_DATA  finddata;
    if (m_strSkinDir[m_strSkinDir.length()-1] != _T('\\') &&
        m_strSkinDir[m_strSkinDir.length()-1] != _T('/'))
    {
        m_strSkinDir.append(_T("\\"));
    }

    String strFind(m_strSkinDir);
    strFind.append(_T("*.*"));

    HANDLE hFind=::FindFirstFile(strFind.c_str(),&finddata);
    if(INVALID_HANDLE_VALUE == hFind)
        return;

    while (1)
    {
        if (finddata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
        {
            if (finddata.cFileName[0] != '.')
            {
                // �ж�Ŀ¼�����Ƿ����skin.xml�ļ�
                String strPath = m_strSkinDir;
                strPath.append(finddata.cFileName);

                String strFile = strPath;
                strFile.append(_T("\\") XML_SKIN_XML);

                if (PathFileExists(strFile.c_str()))
                {
                    OnFindSkinInSkinDir(SKIN_PACKET_TYPE_DIR, finddata.cFileName, strPath.c_str());
                }
            } 
        }
        else
        {
            // �ж��ļ���׺�Ƿ���*.skn��ʽ
            TCHAR szExt[MAX_PATH] = _T("");
            Util::GetPathFileExt(finddata.cFileName, szExt);
            if (0 == _tcscmp(szExt, XML_SKIN_PACKET_EXT))
            {
				int nLength = _tcslen(finddata.cFileName);
				finddata.cFileName[nLength-4] = L'';
                OnFindSkinInSkinDir(SKIN_PACKET_TYPE_ZIP, finddata.cFileName, m_strSkinDir.c_str());
            }
        }
        if(!FindNextFile(hFind,&finddata))
            break;
    }
    FindClose(hFind);
}

void  SkinManager::GetSkinDirection(TCHAR*  szOut)
{
    if (szOut)
    {
        _tcscpy(szOut, m_strSkinDir.c_str());
    }
}

// ��Ƥ��Ŀ¼�����һ����Ƥ�� 
ISkinRes*  SkinManager::AddSkin(LPCTSTR  szPath)
{
    if (nullptr == szPath)
        return nullptr;

    String  strFilePath = m_strSkinDir;
    strFilePath.append(_T("\\"));
    strFilePath.append(szPath);

    if (!PathFileExists(strFilePath.c_str()))
        return nullptr;

    SkinRes* p = OnFindSkinInSkinDir(SKIN_PACKET_TYPE_DIR, szPath, strFilePath.c_str());
    if (p)
        return p->GetISkinRes();

    return nullptr;
}


// �ڵ���SetSkinDirection���������һ��Ƥ���ļ�������ø���Ӧ����
SkinRes*  SkinManager::OnFindSkinInSkinDir(SKIN_PACKET_TYPE eType, LPCTSTR szPath, LPCTSTR szPath)
{
    SkinRes*  pSkin = new SkinRes(*this);
    pSkin->SetParam(eType, szPath, szPath);

    m_vSkinRes.push_back(pSkin);
    return pSkin;
}

// ����
// bSync���ڽ�������һ����ť�������������ť�ڻ����б�delete�ˣ����»����������ջ����
HRESULT SkinManager::ChangeSkin(ISkinRes* pISkinRes, bool bSync)
{
	if (nullptr == pISkinRes)
		return E_INVALIDARG;

    SkinRes* pSkinRes = pISkinRes->GetImpl();
    if (pSkinRes == m_pCurActiveSkinRes)
        return S_FALSE;

    if (!bSync)
    {
        TimerItem  item;
        item.nRepeatCount = 1;
        item.pProc = ChangeSkinTimerProc;
        item.wParam = (WPARAM)this;
        item.lParam = (LPARAM)pISkinRes;
        TimerHelper::GetInstance()->SetTimer(1, &item);
        return E_PENDING;
    }

	SkinRes* pOldSkinRes = m_pCurActiveSkinRes;
	m_pCurActiveSkinRes = nullptr;

    pSkinRes->Load();
    m_pCurActiveSkinRes = pSkinRes;
	
    ITopWindowManager* pTopWndMgr = m_pUIApplication->GetTopWindowMgr();
    if (pTopWndMgr)
        pTopWndMgr->GetImpl()->ChangeSkin(pSkinRes);

    pOldSkinRes->Unload();

	m_bDirty = true;
	return true;
}

SkinRes*  SkinManager::GetSkinResByIndex(long lIndex)
{
	int nSize = (int)m_vSkinRes.size();
	if (lIndex < 0 || lIndex >= nSize )
		return nullptr;
	
	return m_vSkinRes[lIndex];
}

//
//	���õ�ǰ�ĻƤ��������Ƥ���༭�������ڱ༭��Ƥ����
//
//	һЩGET��������Ĭ������ڵ�ǰƤ�����Ե�
//
HRESULT SkinManager::SetActiveSkin(ISkinRes* pSkinRes)
{
	if (nullptr == pSkinRes)
	{
		UI_LOG_WARN(_T("SkinManager::SetActiveSkin failed"));
		return E_INVALIDARG;
	}

	SkinRes* pSkinRes2 = pSkinRes->GetImpl(); // �ڲ���Ȼ����ΪSkinRes�����ڵ���
	m_pCurActiveSkinRes = pSkinRes2;
	return S_OK;
}


//
//	��ȡһ��HSKIN��Ӧ����m_vSkinRes�е�����
//
//	ʧ�ܷ���-1
//
int SkinManager::GetSkinResIndex(SkinRes* pSkinRes)
{
	if (nullptr == pSkinRes)
		return -1;

	int nSize = (int)m_vSkinRes.size();
	if (0 == nSize )
		return -1;

	for (int i = 0; i < nSize; i++)
	{
		if (m_vSkinRes[i] == pSkinRes)
			return i;
	}

	return -1;
}

SkinRes* SkinManager::GetActiveSkin()
{
	if (nullptr == m_pCurActiveSkinRes)
		return nullptr;

	return m_pCurActiveSkinRes;
}

*/

UIApplication*  SkinManager::GetUIApplication()
{
	return m_pUIApplication;
}

void SkinManager::SetUIApplication(UIApplication* pUIApp)
{ 
	m_pUIApplication = pUIApp; 
	// m_SkinBuilderRes.SetUIApplication(pUIApp);
}

// void  ChangeSkinTimerProc(UINT_PTR, TimerItem* pItem)
// {
//     SkinManager* pThis = (SkinManager*)pItem->wParam;
//     pThis->ChangeSkin((ISkinRes*)pItem->lParam, true);
// }

void SkinManager::ChangeSkinHLS( short h, short l, short s, int nFlag )
{
	vector<SkinRes*>::iterator iter = m_vSkinRes.begin();
	for (; iter != m_vSkinRes.end(); ++iter)
	{
		(*iter)->ChangeSkinHLS(h,l,s,nFlag);
	}

	// ֪ͨ����ˢ��
    TopWindowManager* pTopWndMgr = m_pUIApplication->GetTopWindowMgr();
    if (pTopWndMgr)
    {
        UIMSG  msg;
        msg.message = UI_MSG_SKINCHANGED;
        pTopWndMgr->SendMessage2AllWnd(&msg);
        pTopWndMgr->UpdateAllWindow();
    }

	m_bDirty = true;
}

//
//	����Ƥ������
//
SkinRes*  SkinManager::LoadSkinRes(LPCTSTR szPath)
{ 
	if (!szPath)
		return nullptr;

 	UI_LOG_INFO( _T("\n\n------------  LoadSkinRes: %s ----------------\n"), szPath);

	TCHAR szSkinName[MAX_PATH] = {0};
	SKIN_PACKET_TYPE eSkinPackageType = SKIN_PACKET_TYPE_DIR;

	String strPath(szPath);
	if (PathIsDirectory(szPath))
	{
		// ��·���л�ȡƤ������
		TCHAR szDir[MAX_PATH] = {0};
		_tcscpy(szDir, szPath);
		int nLength = (int)_tcslen(szDir);
		if (nLength < 1)
			return nullptr;

		// ������һ���ַ��� \��ɾ����
		if (szDir[nLength-1] == TEXT('\\'))
			szDir[nLength-1] = 0;
		Util::GetPathFileName(szDir, szSkinName);

        // ����ͬ�������ڲ�ͬ��·������
// 		SkinRes* pTest = GetSkinResByName(szSkinName);
// 		if (pTest)
// 		{
// 			UI_LOG_WARN(TEXT("Skin Exist: name=%s"), szSkinName);
// 			return pTest;
// 		}
		eSkinPackageType = SKIN_PACKET_TYPE_DIR;
	}
	else
	{
		TCHAR szExt[MAX_PATH] = _T("");
		Util::GetPathFileExt(szPath, szExt);

		int nExtLength = 0; 

		// ���û�д���׺��������һ�²�����׺
		if (!szExt[0])
		{
			strPath.append(TEXT(".") XML_SKIN_PACKET_EXT);
			if (!PathFileExists(strPath.c_str()))
			{
				UI_LOG_ERROR(TEXT("Skin File not exist: %s"), strPath.c_str());
				return nullptr;
			}
		}
		else if (0 != _tcscmp(szExt, XML_SKIN_PACKET_EXT))
		{
			UI_LOG_ERROR(TEXT("Skin File Format Error: %s"), szExt);
			return nullptr;
		}
		else
		{
			nExtLength = (int)_tcslen(XML_SKIN_PACKET_EXT) + 1;
		}

		Util::GetPathFileName(szPath, szSkinName);
		szSkinName[_tcslen(szSkinName)-nExtLength] = 0;
		SkinRes* pTest = GetSkinResByName(szSkinName);
		if (pTest)
		{
			UI_LOG_WARN(TEXT("Skin Exist: name=%s"), szSkinName);
			return pTest;
		}

		eSkinPackageType = SKIN_PACKET_TYPE_ZIP;
	}

	SkinRes* pSkin = new SkinRes(*this);
	pSkin->CreateDataSource(eSkinPackageType);
	pSkin->SetName(szSkinName);
	pSkin->SetPath(strPath.c_str());

	if (!pSkin->Load())
	{
		UI_LOG_ERROR(TEXT("Skin load failed: %s"), strPath.c_str());
		SAFE_DELETE(pSkin);
		return nullptr;
	}

	m_vSkinRes.push_back(pSkin);
	return pSkin;
}

SkinRes*  SkinManager::LoadSkinRes(HINSTANCE hInstance, int resId)
{
	if (!hInstance)
		return nullptr;

	SkinRes* pSkin = nullptr;
	bool bSuccess = false;
	HGLOBAL hData =  nullptr;
	do 
	{
		if (resId < 0)
			resId = 3344;

		// ������Դ
		HRSRC hRsrc = ::FindResource( 
			hInstance, MAKEINTRESOURCE(resId), L"SKIN");
		if (!hRsrc) 
			 break;

		// ������Դ
		hData = ::LoadResource(hInstance, hRsrc);
		if (!hData)
			break;

		// �����ڴ��е�ָ����Դ
		byte* text = (byte*)::LockResource(hData);
		DWORD dwSize = ::SizeofResource(hInstance, hRsrc);
		if (!text || !dwSize)
			break;

		pSkin = new SkinRes(*this);
		SkinDataSource* pDataSource = pSkin->
			CreateDataSource(SKIN_PACKET_TYPE_RESZIP);
		pDataSource->SetData(text, dwSize);

		if (!pSkin->Load())
			break;

		m_vSkinRes.push_back(pSkin);
		bSuccess = true;	
	} while (0);
	

	// �ͷ���Դ
	if (hData)
	{
		::FreeResource(hData);
	}
	if (bSuccess)
	{
		return pSkin;
	}

	SAFE_DELETE(pSkin);
	return nullptr;
}


SkinRes*  SkinManager::GetDefaultSkinRes()
{
	if (0 == m_vSkinRes.size())
		return nullptr;
	
	return m_vSkinRes[0];
}

//
//	���浽�ļ���
//
//		nullptr - nullptr ȫ������
//		nullptr -  xx  ��ǰƤ����ָ����Դ
//		 xx  - nullptr ָ��Ƥ����������Դ
//
bool SkinManager::Save(SkinRes* pSkinRes)
{
	bool bRet = true;

	//////////////////////////////////////////////////////////////////////////
	// �����Ƥ����Ϣ

	int nSkinCount = (int)m_vSkinRes.size();
	for (int i = 0; i < nSkinCount; i++)
	{
		if (pSkinRes)
		{
			if (pSkinRes != m_vSkinRes[i])
				continue;

			pSkinRes->Save();
		}
		else
        {
			m_vSkinRes[i]->Save();
        }
	}

	return bRet;
}

SkinRes*  SkinManager::GetSkinResByName(LPCTSTR szName)
{
    if (nullptr == szName)
        return nullptr;

    vector<SkinRes*>::iterator iter = m_vSkinRes.begin();
    for (; iter != m_vSkinRes.end(); iter++)
    {
        SkinRes* p = *iter;
        if (0 == _tcscmp(szName, p->GetName()))
            return p;
    }

    return nullptr;
}

uint  SkinManager::GetSkinResCount()
{
	return (uint)m_vSkinRes.size();
}
SkinRes*  SkinManager::GetSkinResByIndex(uint i)
{
	if (i >= m_vSkinRes.size())
		return nullptr;

	return m_vSkinRes[i];
}

LPCTSTR  SkinManager::GetCurrentLanguage()
{
	return m_strLanguage.c_str();
}
void  SkinManager::SetCurrentLanguage(LPCTSTR szText)
{
    if (!szText)
        return;

    if (m_strLanguage == szText)
        return;

    m_strLanguage = szText;

	vector<SkinRes*>::iterator iter = m_vSkinRes.begin();
	for (; iter != m_vSkinRes.end(); ++iter)
	{
		(*iter)->GetI18nManager().Reload();
	}
}