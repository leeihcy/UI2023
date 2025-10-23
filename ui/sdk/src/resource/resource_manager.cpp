#include "resource_manager.h"
#include "include/interface/iresbundle.h"
#include "src/util/util.h"
#include "res_bundle.h"

namespace ui {


ResourceManager::ResourceManager(Application &a)
    : m_root_bundle(*this), m_pUIApplication(&a) {
  m_strLanguage = "zh_cn";
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

  std::vector<ResourceBundle *>::iterator iter = m_bundles.begin();
  std::vector<ResourceBundle *>::iterator iterEnd = m_bundles.end();

  for (; iter != iterEnd; iter++) {
    ResourceBundle *p = (*iter);
    delete p;
  }
  m_bundles.clear();
}

// 遍历该目录下的皮肤列表
/* 过期
void  ResourceManager::SetSkinDirection(const char* szDir)
{
    if (nullptr == szDir || 0 == strlen(szDir))
        return;

    m_strSkinDir = szDir;

    WIN32_FIND_DATA  finddata;
    if (m_strSkinDir[m_strSkinDir.length()-1] != _T('\\') &&
        m_strSkinDir[m_strSkinDir.length()-1] != _T('/'))
    {
        m_strSkinDir.append(_T("\\"));
    }

    std::string strFind(m_strSkinDir);
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
                std::string strPath = m_strSkinDir;
                strPath.append(finddata.cFileName);

                std::string strFile = strPath;
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
            char szExt[MAX_PATH] = _T("");
            util::GetPathFileExt(finddata.cFileName, szExt);
            if (0 == strcmp(szExt, XML_SKIN_PACKET_EXT))
            {
                                int nLength = strlen(finddata.cFileName);
                                finddata.cFileName[nLength-4] = L'';
                OnFindSkinInSkinDir(eBundleFormat::Zip, finddata.cFileName,
m_strSkinDir.c_str());
            }
        }
        if(!FindNextFile(hFind,&finddata))
            break;
    }
    FindClose(hFind);
}

void  ResourceManager::GetSkinDirection(char*  szOut)
{
    if (szOut)
    {
        wcscpy(szOut, m_strSkinDir.c_str());
    }
}

// 在皮肤目录中添加一个新皮肤
IResourceBundle*  ResourceManager::AddSkin(const char*  szPath)
{
    if (nullptr == szPath)
        return nullptr;

    std::string  strFilePath = m_strSkinDir;
    strFilePath.append(_T("\\"));
    strFilePath.append(szPath);

    if (!PathFileExists(strFilePath.c_str()))
        return nullptr;

    ResourceBundle* p = OnFindSkinInSkinDir(SKIN_PACKET_TYPE_DIR, szPath,
strFilePath.c_str()); if (p) return p->GetIResource();

    return nullptr;
}


// 在调用SetSkinDirection后，如果发现一个皮肤文件，则调用该响应函数
ResourceBundle*  ResourceManager::OnFindSkinInSkinDir(eBundleFormat eType, const
char* szPath, const char* szPath)
{
    ResourceBundle*  pResourceBundle = new ResourceBundle(*this);
    pResourceBundle->SetParam(eType, szPath, szPath);

    m_bundles.push_back(pResourceBundle);
    return pResourceBundle;
}

// 换肤
//
bSync用于解决：点击一个按钮换肤，但这个按钮在换肤中被delete了，导致换肤结束后堆栈崩溃
long ResourceManager::ChangeSkin(IResourceBundle* pISkinRes, bool bSync)
{
        if (nullptr == pISkinRes)
                return E_INVALIDARG;

    ResourceBundle* resource_bundle = pISkinRes->GetImpl();
    if (resource_bundle == m_pCurActiveSkinRes)
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

        ResourceBundle* pOldSkinRes = m_pCurActiveSkinRes;
        m_pCurActiveSkinRes = nullptr;

    resource_bundle->Load();
    m_pCurActiveSkinRes = resource_bundle;

    ITopWindowManager* pTopWndMgr = m_pUIApplication->GetTopWindowMgr();
    if (pTopWndMgr)
        pTopWndMgr->GetImpl()->ChangeSkin(resource_bundle);

    pOldSkinRes->Unload();

        m_bDirty = true;
        return true;
}

ResourceBundle*  ResourceManager::GetResourceByIndex(long lIndex)
{
        int nSize = (int)m_bundles.size();
        if (lIndex < 0 || lIndex >= nSize )
                return nullptr;

        return m_bundles[lIndex];
}

//
//	设置当前的活动皮肤（例如皮肤编辑器中正在编辑的皮肤）
//
//	一些GET操作都是默认针对于当前皮肤而言的
//
long ResourceManager::SetActiveSkin(IResourceBundle* resource_bundle)
{
        if (nullptr == resource_bundle)
        {
                UI_LOG_WARN("ResourceManager::SetActiveSkin failed");
                return E_INVALIDARG;
        }

        ResourceBundle* pResourceBundleRes2 = resource_bundle->GetImpl(); //
内部仍然保存为SkinRes，便于调用 m_pCurActiveSkinRes = pResourceBundleRes2; return 0;
}


//
//	获取一个HSKIN对应的在m_skins中的索引
//
//	失败返回-1
//
int ResourceManager::GetResourceIndex(ResourceBundle* resource_bundle)
{
        if (nullptr == resource_bundle)
                return -1;

        int nSize = (int)m_bundles.size();
        if (0 == nSize )
                return -1;

        for (int i = 0; i < nSize; i++)
        {
                if (m_bundles[i] == resource_bundle)
                        return i;
        }

        return -1;
}

ResourceBundle* ResourceManager::GetActiveSkin()
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
//     pThis->ChangeSkin((IResourceBundle*)pItem->lParam, true);
// }

void ResourceManager::ChangeSkinHLS(short h, short l, short s, int nFlag) {
#if 0
	std::vector<ResourceBundle*>::iterator iter = m_bundles.begin();
	for (; iter != m_bundles.end(); ++iter)
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
ResourceBundle *ResourceManager::LoadResource(const char *szPath) {
  if (!szPath)
    return nullptr;
  
  const int MAX_PATH = 256;

  UI_LOG_INFO("\n\n------------  LoadResource: %s ----------------\n",
              szPath);

  char szSkinName[MAX_PATH] = {0};
  eBundleFormat eSkinPackageType = eBundleFormat::Directory;

  std::string strPath(szPath);
  if (util::PathIsDirectory(szPath)) { 
    // 从路径中获取皮肤名。
    char szDir[MAX_PATH] = {0};
    strcpy(szDir, szPath);
    int nLength = (int)strlen(szDir);
    if (nLength < 1)
      return nullptr;

    // 如果最后一个字符是 \，删除。
    if (szDir[nLength - 1] == '\\')
      szDir[nLength - 1] = 0;
    ui::util::GetPathFileName(szDir, szSkinName);

    eSkinPackageType = eBundleFormat::Directory;
  } else {
     char szExt[MAX_PATH] = "";
    util::GetPathFileExt(szPath, szExt);

    int nExtLength = 0;

    // 如果没有带后缀名，尝试一下补个后缀
    if (!szExt[0]) {
      strPath.append("." XML_SKIN_PACKET_EXT);
      if (!util::PathExists(strPath.c_str())) {
        UI_LOG_ERROR("Skin File not exist: %s", strPath.c_str());
        return nullptr;
      }
    } else if (0 != strcmp(szExt, XML_SKIN_PACKET_EXT)) {
      UI_LOG_ERROR("Skin File Format Error: %s", szExt);
      return nullptr;
    } else {
      nExtLength = (int)strlen(XML_SKIN_PACKET_EXT) + 1;
    }

    util::GetPathFileName(szPath, szSkinName);
    szSkinName[strlen(szSkinName) - nExtLength] = 0;
    ResourceBundle *pTest = GetResourceByName(szSkinName);
    if (pTest) {
      UI_LOG_WARN("Skin Exist: name=%s", szSkinName);
      return pTest;
    }

    eSkinPackageType = eBundleFormat::Zip;
  }
  
  ResourceBundle *pResourceBundle = new ResourceBundle(*this);
  pResourceBundle->CreateBundleSource(eSkinPackageType);
  pResourceBundle->SetName(szSkinName);
  pResourceBundle->SetPath(strPath.c_str());

  if (!pResourceBundle->Load()) {
    UI_LOG_ERROR("Skin load failed: %s", strPath.c_str());
    SAFE_DELETE(pResourceBundle);
    return nullptr;
  }

  m_bundles.push_back(pResourceBundle);
  return pResourceBundle;
}

ResourceBundle *ResourceManager::LoadResource(llong hInstance, int resId) {
#if 0 // defined(OS_WIN)
  if (!hInstance)
    return nullptr;

  ResourceBundle *pResourceBundle = nullptr;
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

    pResourceBundle = new ResourceBundle(*this);
    BundleSource *pDataSource =
        pResourceBundle->CreateBundleSource(SKIN_PACKET_TYPE_RESZIP);
    pDataSource->SetData(text, dwSize);

    if (!pResourceBundle->Load())
      break;

    m_bundles.push_back(pResourceBundle);
    bSuccess = true;
  } while (0);

  // 释放资源
  if (hData) {
    ::FreeResource(hData);
  }
  if (bSuccess) {
    return pResourceBundle;
  }

  SAFE_DELETE(pResourceBundle);
  return nullptr;
#else
  UIASSERT(false);
  return nullptr;
#endif
}

ResourceBundle *ResourceManager::GetDefaultSkinRes() {
  if (0 == m_bundles.size())
    return nullptr;

  return m_bundles[0];
}

//
//	保存到文件中
//
//		nullptr - nullptr 全部保存
//		nullptr -  xx  当前皮肤的指定资源
//		 xx  - nullptr 指定皮肤的所有资源
//
bool ResourceManager::Save(ResourceBundle *resource_bundle) {
  bool bRet = true;

  //////////////////////////////////////////////////////////////////////////
  // 保存各皮肤信息

  int nSkinCount = (int)m_bundles.size();
  for (int i = 0; i < nSkinCount; i++) {
    if (resource_bundle) {
      if (resource_bundle != m_bundles[i])
        continue;

      resource_bundle->Save();
    } else {
      m_bundles[i]->Save();
    }
  }

  return bRet;
}

ResourceBundle *ResourceManager::GetResourceByName(const char *szName) {
  if (nullptr == szName)
    return nullptr;

  std::vector<ResourceBundle *>::iterator iter = m_bundles.begin();
  for (; iter != m_bundles.end(); iter++) {
    ResourceBundle *p = *iter;
    if (0 == strcmp(szName, p->GetName()))
      return p;
  }

  return nullptr;
}

unsigned int ResourceManager::GetResourceCount() {
  return (unsigned int)m_bundles.size();
}
ResourceBundle *ResourceManager::GetResourceByIndex(unsigned int i) {
  if (i >= m_bundles.size())
    return nullptr;

  return m_bundles[i];
}

const char *ResourceManager::GetCurrentLanguage() {
  return m_strLanguage.c_str();
}
void ResourceManager::SetCurrentLanguage(const char *szText) {
  if (!szText)
    return;

  if (m_strLanguage == szText)
    return;

  m_strLanguage = szText;

  std::vector<ResourceBundle *>::iterator iter = m_bundles.begin();
  for (; iter != m_bundles.end(); ++iter) {
    (*iter)->GetI18nManager().Reload();
  }
}

} // namespace ui