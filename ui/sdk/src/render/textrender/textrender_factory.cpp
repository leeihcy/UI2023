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
  this->RegisterUITextRender(SimpleTextRenderMeta::Get());
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

bool TextRenderFactory::RegisterUITextRender(ITextRenderBaseMeta &meta) {
  m_vecTextRenderCreator.push_back(&meta);
  return true;
}

std::shared_ptr<ITextRenderBase> TextRenderFactory::CreateTextRenderBaseByName(IResourceBundle *resource,
                                                   const char *name) {
  if (!resource || !name)
    return nullptr;

  auto iter = m_vecTextRenderCreator.begin();
  for (; iter != m_vecTextRenderCreator.end(); ++iter) {
    ITextRenderBaseMeta *meta = *iter;
    if (!meta || !meta->Name())
      continue;

    if (strcmp(name, meta->Name()) != 0)
      continue;

    std::shared_ptr<ITextRenderBase> out = meta->CreateShared(resource);
    if (out) {
      // out->SetObject(obj);
      out->Init();
      out->SetType((TEXTRENDER_TYPE)meta->Detail().minor_type);
    }
    return out;
  }

  UI_LOG_WARN("Create Failed. Name=%s", name);
  return nullptr;
}
std::shared_ptr<ITextRenderBase> TextRenderFactory::CreateTextRender(IResourceBundle *resource, int type) {
  auto iter = m_vecTextRenderCreator.begin();
  for (; iter != m_vecTextRenderCreator.end(); iter++) {
    ITextRenderBaseMeta *meta = *iter;
    if (!meta)
      continue;

    if (meta->Detail().minor_type != type)
      continue;

    std::shared_ptr<ITextRenderBase> out = meta->CreateShared(resource);
    if (out) {
      // out->SetObject(object);
      out->Init();
      out->SetType((TEXTRENDER_TYPE)type);
    }
    return out;
  }

  UI_LOG_WARN("Create Failed. Type=%d", type);
  return nullptr;
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