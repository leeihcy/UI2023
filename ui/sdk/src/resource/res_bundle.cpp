#include "res_bundle.h"
#include "src/resource/resource_manager.h"
#include "src/parser/datasource/bundle_source.h"
#include "src/parser/skinparseengine.h"
#include "src/parser/xml/xmlwrap.h"
#include "include/interface/iuires.h"
#include "uiresource.h"

namespace ui {

ResourceBundle::ResourceBundle(ResourceManager &o)
    : m_mgrSkinRef(o), m_mgrLayout(this), m_mgrStyle(*this), /*m_mgrFont(this),*/
      m_mgrColor(this), m_mgrImage(this), m_mgrI18n(this), m_fontres(*this)

{
  m_pISkinRes = nullptr;
  m_eLoadState = SKIN_RES_LOAD_STATE_NONE;
  m_pHLSInfo = nullptr;
  m_pParentSkinRes = nullptr;
}
ResourceBundle::~ResourceBundle() {
  Unload();
  SAFE_DELETE(m_pISkinRes);
  SAFE_DELETE(m_pHLSInfo);
}

IResourceBundle *ResourceBundle::GetIResource() {
  if (nullptr == m_pISkinRes) {
    m_pISkinRes = new IResourceBundle(this);
  }
  return m_pISkinRes;
}

void ResourceBundle::SetName(const char *szName) {
  if (szName)
    m_strSkinResName = szName;
  else
    m_strSkinResName.clear();
}

void ResourceBundle::SetPath(const char *szPath) {
  UIASSERT(m_source);
  if (!m_source)
    return;
  m_source->SetPath(szPath);
}

void ResourceBundle::CreateBundleSource(eBundleFormat eType) {
  m_source = ui::CreateBundleSource(eType);
}

const char *ResourceBundle::GetName() { return m_strSkinResName.c_str(); }

const char *ResourceBundle::GetPath() {
  if (nullptr == m_source)
    return nullptr;

  return m_source->GetPath();
}

bool ResourceBundle::Load() {
  if (m_eLoadState == SKIN_RES_LOAD_STATE_LOADED) {
    return true;
  }

  SkinParseEngine parse(this);
  if (parse.Parse(m_source.get(), XML_RESOURCE_XML)) {
    m_eLoadState = SKIN_RES_LOAD_STATE_LOADED;
    return true;
  } else {
    Unload();
    return false;
  }
}

bool ResourceBundle::Unload() {
  _DocList::iterator iter = m_listDoc.begin();
  for (; iter != m_listDoc.end(); iter++) {
    UIDocument *po = *iter;
    SAFE_RELEASE(po);
  }
  m_listDoc.clear();

  m_eLoadState = SKIN_RES_LOAD_STATE_NONE;
  return true;
}

void ResourceBundle::SetParentSkinRes(ResourceBundle *p) {
  UIASSERT(p != this);
  m_pParentSkinRes = p;
}

ResourceBundle *ResourceBundle::GetParentSkinRes() { return m_pParentSkinRes; }

// parse.ProcessFile在解析配置文件过程中，如果发现一个新的文档，
// 则会回调该函数进行通知
void ResourceBundle::OnNewUIDocument(UIDocument *pDoc) {
  if (!pDoc)
    return;

  pDoc->AddRef();
  m_listDoc.push_back(pDoc);
}

bool ResourceBundle::Save() {
  _DocList::iterator iter = m_listDoc.begin();
  for (; iter != m_listDoc.end(); iter++) {
    UIDocument *po = *iter;

    if (!po->IsDirty())
      continue;

    if (!po->Save()) {
      UI_LOG_ERROR("%s Save Xml Doc Failed. path=%s", po->GetSkinPath());
    }
  }

  return true;
}

bool ResourceBundle::ChangeSkinHLS(short h, short l, short s, int nFlag) {
  if (nullptr == m_pHLSInfo) {
    m_pHLSInfo = new SKIN_HLS_INFO;
  }
  m_pHLSInfo->h = h;
  m_pHLSInfo->l = l;
  m_pHLSInfo->s = s;
  m_pHLSInfo->nFlag = nFlag;

  if (false == m_mgrImage.ChangeSkinHLS(h, l, s, nFlag))
    return false;
  if (false == m_mgrColor.ChangeSkinHLS(h, l, s, nFlag))
    return false;

#if 0 // -- 架构改造	
	// 保存该 HLS 数据
	SkinConfigRes* pSkinConfigRes = m_pSkinConfigMgr->GetSkinConfigRes();
	if (nullptr == pSkinConfigRes)
		return false;

	pSkinConfigRes->SetHLS((char)h,(char)l,(char)s,nFlag);
#endif
  return true;
}

UIDocument *ResourceBundle::GetXmlDoc(unsigned int nIndex) {
  if (nIndex >= GetXmlDocCount())
    return nullptr;

  return m_listDoc[nIndex];
}

UIDocument *ResourceBundle::GetXmlDocByName(const char *szName) {
  UIASSERT(0);
  // if (!szName)
  // 	return nullptr;

  // _DocList::iterator iter = m_listDoc.begin();
  // for (; iter != m_listDoc.end(); iter++)
  // {
  // 	UIDocument* po = *iter;
  // 	if (0 == strcmp(szName, po->GetSkinPath()))
  // 	{
  // 		return po;
  // 	}
  // }
  return nullptr;
}

Application *ResourceBundle::GetUIApplication() {
  return m_mgrSkinRef.GetUIApplication();
}
IResourceManager &ResourceBundle::GetIResourceManager() {
  return m_mgrSkinRef.GetIResourceManager();
}

ImageRes &ResourceBundle::GetImageRes() { return m_mgrImage.GetImageRes(); }
ColorRes &ResourceBundle::GetColorRes() { return m_mgrColor.GetColorRes(); }
// FontRes &ResourceBundle::GetFontRes() { return m_mgrFont.GetFontRes(); }

StyleRes &ResourceBundle::GetStyleRes() { return m_mgrStyle.GetStyleRes(); }

I18nRes &ResourceBundle::GetI18nRes() { return m_mgrI18n.GetI18nRes(); }
} // namespace ui