#include "resource_manager.h"
#include "common.h"
#include "interface/iresbundle.h"

// #include "interface\iuires.h"
#include "res_bundle.h"
// #include "include/interface/iuiinterface.h"
// #include "..\Base\Application\uiapplication.h"
// #include "..\Helper\topwindow\topwindowmanager.h"
// #include "..\SkinParse\datasource\skindatasource.h"

namespace ui {

ResourceManager::ResourceManager(Application &a)
    : m_root_bundle(*this), m_pUIApplication(&a) {
  m_strLanguage = L"zh_cn";
  // m_strLanguage = TEXT("en_us");
}

ResourceManager::~ResourceManager() { SAFE_DELETE(m_pIResourceManager); }

IResourceManager &ResourceManager::GetIResourceManager() {
  if (!m_pIResourceManager)
    m_pIResourceManager = new IResourceManager(this);

  return *m_pIResourceManager;
}

// 用于在UIApplication的析构函数中提前释放
void ResourceManager::Destroy() {
  //////////////////////////////////////////////////////////////////////////
  // 释放各皮肤数据内存

  std::vector<ResBundle *>::iterator iter = m_resoures.begin();
  std::vector<ResBundle *>::iterator iterEnd = m_resoures.end();

  for (; iter != iterEnd; iter++) {
    ResBundle *p = (*iter);
    delete p;
  }
  m_resoures.clear();
}

// 遍历该目录下的皮肤列表
/* 过期
void  ResourceManager::SetSkinDirection(const wchar_t* szDir)
{
    if (nullptr == szDir || 0 == wcslen(szDir))
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
                // 判断目录下面是否存在skin.xml文件
                String strPath = m_strSkinDir;
                strPath.append(finddata.cFileName);

                String strFile = strPath;
                strFile.append(_T("\\") XML_SKIN_XML);

                if (PathFileExists(strFile.c_str()))
                {
                    OnFindSkinInSkinDir(SKIN_PACKET_TYPE_DIR,
finddata.cFileName, strPath.c_str());
                }
            }
        }
        else
        {
            // 判断文件后缀是否是*.skn格式
            wchar szExt[MAX_PATH] = _T("");
            util::GetPathFileExt(finddata.cFileName, szExt);
            if (0 == wcscmp(szExt, XML_SKIN_PACKET_EXT))
            {
                                int nLength = wcslen(finddata.cFileName);
                                finddata.cFileName[nLength-4] = L'';
                OnFindSkinInSkinDir(SKIN_PACKET_TYPE_ZIP, finddata.cFileName,
m_strSkinDir.c_str());
            }
        }
        if(!FindNextFile(hFind,&finddata))
            break;
    }
    FindClose(hFind);
}

void  ResourceManager::GetSkinDirection(wchar*  szOut)
{
    if (szOut)
    {
        wcscpy(szOut, m_strSkinDir.c_str());
    }
}

// 在皮肤目录中添加一个新皮肤
IResBundle*  ResourceManager::AddSkin(const wchar_t*  szPath)
{
    if (nullptr == szPath)
        return nullptr;

    String  strFilePath = m_strSkinDir;
    strFilePath.append(_T("\\"));
    strFilePath.append(szPath);

    if (!PathFileExists(strFilePath.c_str()))
        return nullptr;

    ResBundle* p = OnFindSkinInSkinDir(SKIN_PACKET_TYPE_DIR, szPath,
strFilePath.c_str()); if (p) return p->GetIResBundle();

    return nullptr;
}


// 在调用SetSkinDirection后，如果发现一个皮肤文件，则调用该响应函数
ResBundle*  ResourceManager::OnFindSkinInSkinDir(SKIN_PACKET_TYPE eType, const
wchar_t* szPath, const wchar_t* szPath)
{
    ResBundle*  pSkin = new ResBundle(*this);
    pSkin->SetParam(eType, szPath, szPath);

    m_resoures.push_back(pSkin);
    return pSkin;
}

// 换肤
//
bSync用于解决：点击一个按钮换肤，但这个按钮在换肤中被delete了，导致换肤结束后堆栈崩溃
long ResourceManager::ChangeSkin(IResBundle* pISkinRes, bool bSync)
{
        if (nullptr == pISkinRes)
                return E_INVALIDARG;

    ResBundle* pSkinRes = pISkinRes->GetImpl();
    if (pSkinRes == m_pCurActiveSkinRes)
        return S_FALSE;

    if (!bSync)
    {
        TimerItem  item;
        item.nRepeatCount = 1;
        item.pProc = ChangeSkinTimerProc;
        item.wParam = (long)this;
        item.lParam = (long)pISkinRes;
        TimerHelper::GetInstance()->SetTimer(1, &item);
        return E_PENDING;
    }

        ResBundle* pOldSkinRes = m_pCurActiveSkinRes;
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

ResBundle*  ResourceManager::GetSkinResByIndex(long lIndex)
{
        int nSize = (int)m_resoures.size();
        if (lIndex < 0 || lIndex >= nSize )
                return nullptr;

        return m_resoures[lIndex];
}

//
//	设置当前的活动皮肤（例如皮肤编辑器中正在编辑的皮肤）
//
//	一些GET操作都是默认针对于当前皮肤而言的
//
long ResourceManager::SetActiveSkin(IResBundle* pSkinRes)
{
        if (nullptr == pSkinRes)
        {
                UI_LOG_WARN(_T("ResourceManager::SetActiveSkin failed"));
                return E_INVALIDARG;
        }

        ResBundle* pSkinRes2 = pSkinRes->GetImpl(); //
内部仍然保存为SkinRes，便于调用 m_pCurActiveSkinRes = pSkinRes2; return 0;
}


//
//	获取一个HSKIN对应的在m_skins中的索引
//
//	失败返回-1
//
int ResourceManager::GetSkinResIndex(ResBundle* pSkinRes)
{
        if (nullptr == pSkinRes)
                return -1;

        int nSize = (int)m_resoures.size();
        if (0 == nSize )
                return -1;

        for (int i = 0; i < nSize; i++)
        {
                if (m_resoures[i] == pSkinRes)
                        return i;
        }

        return -1;
}

ResBundle* ResourceManager::GetActiveSkin()
{
        if (nullptr == m_pCurActiveSkinRes)
                return nullptr;

        return m_pCurActiveSkinRes;
}

*/

Application *ResourceManager::GetUIApplication() { return m_pUIApplication; }

// void  ChangeSkinTimerProc(unsigned int*, TimerItem* pItem)
// {
//     ResourceManager* pThis = (ResourceManager*)pItem->wParam;
//     pThis->ChangeSkin((IResBundle*)pItem->lParam, true);
// }

void ResourceManager::ChangeSkinHLS(short h, short l, short s, int nFlag) {
#if 0
	std::vector<ResBundle*>::iterator iter = m_resoures.begin();
	for (; iter != m_resoures.end(); ++iter)
	{
		(*iter)->ChangeSkinHLS(h,l,s,nFlag);
	}

	// 通知窗口刷新
    TopWindowManager* pTopWndMgr = m_pUIApplication->GetTopWindowMgr();
    if (pTopWndMgr)
    {
        UIMSG  msg;
        msg.message = UI_MSG_SKINCHANGED;
        pTopWndMgr->SendMessage2AllWnd(&msg);
        pTopWndMgr->UpdateAllWindow();
    }

	m_bDirty = true;
#endif
  UIASSERT(false); // TODO:
}

//
//	加载皮肤数据
//
ResBundle *ResourceManager::LoadResBundle(const wchar_t *szPath) {
  if (!szPath)
    return nullptr;

  UI_LOG_INFO(L"\n\n------------  LoadResBundle: %s ----------------\n",
              szPath);

  wchar szSkinName[MAX_PATH] = {0};
  SKIN_PACKET_TYPE eSkinPackageType = SKIN_PACKET_TYPE_DIR;

  String strPath(szPath);
  if (ui::util::PathIsDirectory(szPath)) {
    // 从路径中获取皮肤名。
    wchar szDir[MAX_PATH] = {0};
    wcscpy(szDir, szPath);
    int nLength = (int)wcslen(szDir);
    if (nLength < 1)
      return nullptr;

    // 如果最后一个字符是 \，删除。
    if (szDir[nLength - 1] == TEXT('\\'))
      szDir[nLength - 1] = 0;
    ui::util::GetPathFileName(szDir, szSkinName);

    // 允许同名，但在不同的路径下面
    // 		ResBundle* pTest = GetSkinResByName(szSkinName);
    // 		if (pTest)
    // 		{
    // 			UI_LOG_WARN(TEXT("Skin Exist: name=%s"), szSkinName);
    // 			return pTest;
    // 		}
    eSkinPackageType = SKIN_PACKET_TYPE_DIR;
  } else {
    wchar szExt[MAX_PATH] = _T("");
    util::GetPathFileExt(szPath, szExt);

    int nExtLength = 0;

    // 如果没有带后缀名，尝试一下补个后缀
    if (!szExt[0]) {
      strPath.append(TEXT(".") XML_SKIN_PACKET_EXT);
      if (!ui::util::PathFileExists(strPath.c_str())) {
        UI_LOG_ERROR(TEXT("Skin File not exist: %s"), strPath.c_str());
        return nullptr;
      }
    } else if (0 != wcscmp(szExt, XML_SKIN_PACKET_EXT)) {
      UI_LOG_ERROR(TEXT("Skin File Format Error: %s"), szExt);
      return nullptr;
    } else {
      nExtLength = (int)wcslen(XML_SKIN_PACKET_EXT) + 1;
    }

    util::GetPathFileName(szPath, szSkinName);
    szSkinName[wcslen(szSkinName) - nExtLength] = 0;
    ResBundle *pTest = GetSkinResByName(szSkinName);
    if (pTest) {
      UI_LOG_WARN(TEXT("Skin Exist: name=%s"), szSkinName);
      return pTest;
    }

    eSkinPackageType = SKIN_PACKET_TYPE_ZIP;
  }

  ResBundle *pSkin = new ResBundle(*this);
  pSkin->CreateDataSource(eSkinPackageType);
  pSkin->SetName(szSkinName);
  pSkin->SetPath(strPath.c_str());

  if (!pSkin->Load()) {
    UI_LOG_ERROR(TEXT("Skin load failed: %s"), strPath.c_str());
    SAFE_DELETE(pSkin);
    return nullptr;
  }

  m_resoures.push_back(pSkin);
  return pSkin;
}

ResBundle *ResourceManager::LoadResBundle(long hInstance, int resId) {
#if defined(OS_WIN)
  if (!hInstance)
    return nullptr;

  ResBundle *pSkin = nullptr;
  bool bSuccess = false;
  HGLOBAL hData = nullptr;
  do {
    if (resId < 0)
      resId = 3344;

    // 查找资源
    HRSRC hRsrc = ::FindResource(hInstance, MAKEINTRESOURCE(resId), L"SKIN");
    if (!hRsrc)
      break;

    // 加载资源
    hData = ::LoadResource(hInstance, hRsrc);
    if (!hData)
      break;

    // 锁定内存中的指定资源
    byte *text = (byte *)::LockResource(hData);
    unsigned int dwSize = ::SizeofResource(hInstance, hRsrc);
    if (!text || !dwSize)
      break;

    pSkin = new ResBundle(*this);
    SkinDataSource *pDataSource =
        pSkin->CreateDataSource(SKIN_PACKET_TYPE_RESZIP);
    pDataSource->SetData(text, dwSize);

    if (!pSkin->Load())
      break;

    m_resoures.push_back(pSkin);
    bSuccess = true;
  } while (0);

  // 释放资源
  if (hData) {
    ::FreeResource(hData);
  }
  if (bSuccess) {
    return pSkin;
  }

  SAFE_DELETE(pSkin);
  return nullptr;
#else
  UIASSERT(false);
  return nullptr;
#endif
}

ResBundle *ResourceManager::GetDefaultSkinRes() {
  if (0 == m_resoures.size())
    return nullptr;

  return m_resoures[0];
}

//
//	保存到文件中
//
//		nullptr - nullptr 全部保存
//		nullptr -  xx  当前皮肤的指定资源
//		 xx  - nullptr 指定皮肤的所有资源
//
bool ResourceManager::Save(ResBundle *pSkinRes) {
  bool bRet = true;

  //////////////////////////////////////////////////////////////////////////
  // 保存各皮肤信息

  int nSkinCount = (int)m_resoures.size();
  for (int i = 0; i < nSkinCount; i++) {
    if (pSkinRes) {
      if (pSkinRes != m_resoures[i])
        continue;

      pSkinRes->Save();
    } else {
      m_resoures[i]->Save();
    }
  }

  return bRet;
}

ResBundle *ResourceManager::GetSkinResByName(const wchar_t *szName) {
  if (nullptr == szName)
    return nullptr;

  std::vector<ResBundle *>::iterator iter = m_resoures.begin();
  for (; iter != m_resoures.end(); iter++) {
    ResBundle *p = *iter;
    if (0 == wcscmp(szName, p->GetName()))
      return p;
  }

  return nullptr;
}

unsigned int ResourceManager::GetSkinResCount() {
  return (unsigned int)m_resoures.size();
}
ResBundle *ResourceManager::GetSkinResByIndex(unsigned int i) {
  if (i >= m_resoures.size())
    return nullptr;

  return m_resoures[i];
}

const wchar_t *ResourceManager::GetCurrentLanguage() {
  return m_strLanguage.c_str();
}
void ResourceManager::SetCurrentLanguage(const wchar_t *szText) {
  if (!szText)
    return;

  if (m_strLanguage == szText)
    return;

  m_strLanguage = szText;

  std::vector<ResBundle *>::iterator iter = m_resoures.begin();
  for (; iter != m_resoures.end(); ++iter) {
    (*iter)->GetI18nManager().Reload();
  }
}

} // namespace ui