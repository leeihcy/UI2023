#include "textrender_factory.h"
#include "include/inc.h"
#include "src/application/uiapplication.h"
#include "src/private_inc.h"
#include "src/render/render_meta.h"
#include "src/render/textrender/textrender.h"

namespace ui {
TextRenderFactory::TextRenderFactory(Application &app) : m_app(app) {}
TextRenderFactory::~TextRenderFactory() { Clear(); }

void TextRenderFactory::Init() {

  // #define REGISTER_UI_TEXTRENDERBASE2(classname) \
//         this->RegisterUITextRenderCreateData( \
//                 classname::GetXmlName(),          \
//                 classname::GetType(),             \
// 				(ui::pfnUICreateTextRenderPtr)ObjectCreator<I##classname>::CreateInstance2);

  this->RegisterUITextRender(SimpleTextRenderMeta::Get());
  this->RegisterUITextRender(ColorListTextRenderMeta::Get());
#if 0 // TODO
	REGISTER_UI_TEXTRENDERBASE2(FontColorListTextRender)
	REGISTER_UI_TEXTRENDERBASE2(ContrastColorTextRender)
	REGISTER_UI_TEXTRENDERBASE2(ContrastColorListTextRender)
#endif
}

void TextRenderFactory::Clear() { m_vecTextRenderCreator.clear(); }

// bool  TextRenderFactory::RegisterUITextRenderCreateData(
//         const char* szName,
//         int nType,
//         pfnUICreateTextRenderPtr pfunc)
// {
// 	if (nullptr == szName || nullptr == pfunc)
// 		return false;

// 	std::string strName(szName);

// 	UITEXTRENDERBASE_CREATE_INFO* pInfo = new UITEXTRENDERBASE_CREATE_INFO;
// 	pInfo->m_func = pfunc;
// 	pInfo->m_nRenderType = nType;
// 	pInfo->m_strName = strName;
// 	m_vecTextRenderCreator.push_back(pInfo);

// 	//UI_LOG_DEBUG("%s   @ 0x%08X", szName,  pfunc);
// 	return true;
// }

bool TextRenderFactory::RegisterUITextRender(IMeta &meta) {
  m_vecTextRenderCreator.push_back(&meta);
  return true;
}

bool TextRenderFactory::CreateTextRenderBaseByName(IResource *resource,
                                                   const char *name,
                                                   IObject *obj,
                                                   ITextRenderBase **ppOut) {
  if (!resource || !name || !obj || !ppOut)
    return false;

  auto iter = m_vecTextRenderCreator.begin();
  for (; iter != m_vecTextRenderCreator.end(); ++iter) {
    IMeta *meta = *iter;
    if (!meta || !meta->Name())
      continue;

    if (strcmp(name, meta->Name()) != 0)
      continue;

    meta->Create(resource, (void **)ppOut);
    if (*ppOut) {
      (*ppOut)->SetObject(obj);
      (*ppOut)->Init();
      (*ppOut)->SetType((TEXTRENDER_TYPE)meta->Detail().minor_type);
      return true;
    }

    return false;
  }

  UI_LOG_WARN("Create Failed. Name=%s", name);
  return false;
}
bool TextRenderFactory::CreateTextRender(IResource *resource, int type,
                                         IObject *object,
                                         ITextRenderBase **ppOut) {
  if (nullptr == object || nullptr == ppOut)
    return false;

  auto iter = m_vecTextRenderCreator.begin();
  for (; iter != m_vecTextRenderCreator.end(); iter++) {
    IMeta *meta = *iter;
    if (!meta)
      continue;

    if (meta->Detail().minor_type != type)
      continue;

    meta->Create(resource, (void **)ppOut);
    if (*ppOut) {
      (*ppOut)->SetObject(object);
      (*ppOut)->Init();
      (*ppOut)->SetType((TEXTRENDER_TYPE)type);
      return true;
    }

    return false;
  }

  UI_LOG_WARN("Create Failed. Type=%d", type);
  return false;
}

const char *TextRenderFactory::GetTextRenderBaseName(int nType) {
  auto iter = m_vecTextRenderCreator.begin();
  for (; iter != m_vecTextRenderCreator.end(); ++iter) {
    IMeta *meta = *iter;
    if (!meta || !meta->Name())
      continue;

    if (meta->Detail().minor_type != nType)
      continue;

    return meta->Name();
  }

  return nullptr;
}

void TextRenderFactory::EnumTextRenderBaseName(
    pfnEnumTextRenderNameCallback callback, long wParam, long lParam) {
  auto iter = m_vecTextRenderCreator.begin();
  for (; iter != m_vecTextRenderCreator.end(); ++iter) {
    IMeta *meta = *iter;
    if (!meta || !meta->Name())
      continue;

    callback(meta->Name(), wParam, lParam);
  }
}

} // namespace ui