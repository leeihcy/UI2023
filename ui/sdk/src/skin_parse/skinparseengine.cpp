#include "skinparseengine.h"
#include "datasource/skindatasource.h"
#include "include/inc.h"
#include "include/interface/iskindatasource.h"
#include "include/interface/iuires.h"
#include "include/interface/ixmlwrap.h"
#include "src/application/uiapplication.h"
#include "src/resource/res_bundle.h"
#include "xml/pugixmlwrap/pugixmlwrap.h"

namespace ui {

SkinParseEngine::SkinParseEngine(Resource *pSkinRes) {
  UIASSERT(pSkinRes);
  m_pSkinRes = pSkinRes;
  m_pUIApplication = m_pSkinRes->GetUIApplication();
}

SkinParseEngine::~SkinParseEngine() {}

bool SkinParseEngine::Parse(SkinDataSource *pDataSource,
                            const wchar_t *szXmlFile) {
#if 0
	1. 查找 <skin> root element，如果没有找到则直接返回，表示这不是一个合法文件
		2. 遍历 <skin> 的child element

		2.1 获取tagName，例如image
		2.2 获取image标签对应的解析器 IImageParse
		2.3 调用IImageParse的NewElement
		2.4 IImageParse自己去负责遍历子结点
#endif
  if (!pDataSource || !szXmlFile)
    return false;

  UIDocument *pUIDocument = nullptr;
  CreateXmlDocument(XML_ENGINE_DEFAULT, &pUIDocument);
  if (!pDataSource->Load_UIDocument(pUIDocument, szXmlFile)) {
    SAFE_RELEASE(pUIDocument);
    return false;
  }
  pUIDocument->SetSkinPath(szXmlFile);

  do {
    UIElementProxy rootElem = pUIDocument->FindElem(XML_SKIN);
    if (!rootElem) {
      UI_LOG_ERROR(_T("Cannot find root element: %s"), XML_SKIN);
      break;
    }

    m_pSkinRes->OnNewUIDocument(pUIDocument);

    UIElementProxy childElem = rootElem->FirstChild();
    while (childElem) {
      this->NewChild(childElem.get());
      childElem = childElem->NextElement();
    }
  } while (0);

  SAFE_RELEASE(pUIDocument);
  return true;
}

void SkinParseEngine::NewChild(UIElement *pElement) {
  if (!pElement)
    return;

  pfnParseSkinTag func;
  if (!m_pUIApplication->GetSkinTagParseFunc(
          (const wchar_t *)pElement->GetTagName(), &func)) {
    return;
  }

  func(pElement->GetIUIElement(), m_pSkinRes->GetIResource());
}

int SkinParseEngine::UIParseIncludeTagCallback(IUIElement *pElement,
                                               IResource *pSkinRes) {
  if (nullptr == pElement || nullptr == pSkinRes)
    return -1; // E_FAIL;

  const wchar_t *szData = pElement->GetData();
  if (!szData)
    return -1; // E_FAIL;

  SkinParseEngine parse(pSkinRes->GetImpl());
  SkinDataSource *pDataSource = pSkinRes->GetImpl()->GetDataSource();

  if (!parse.Parse(pDataSource, szData))
    return -1; // E_FAIL;

  return 0;
}

bool CreateXmlDocument(XML_ENGINE e, UIDocument **pp) {
  if (!pp)
    return false;

  *pp = nullptr;

  switch (e) {
    //     case MSXML:
    //         {
    //             return MsXmlDocument::CreateInstance(pp);
    //         }
    //         break;

  case PUGIXML:
  case XML_ENGINE_DEFAULT: {
    UIDocument *p = new PugiXmlDocument;
    p->AddRef();
    *pp = p;
    return true;
  } break;
  }

  return false;
}

} // namespace ui