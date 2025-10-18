#include "renderbase_factory.h"
#include "include/inc.h"
#include "include/interface/irenderbase.h"
#include "include/macro/msg.h"
#include "src/application/uiapplication.h"
#include "src/private_inc.h"
#include "src/render/colorrender/colorrender.h"
#include "src/render/imagerender/imagerender.h"
#include "render_meta.h"
#include <cstddef>
#include <memory>

namespace ui {
RenderBaseFactory::RenderBaseFactory(Application &app) : m_app(app) {}

RenderBaseFactory::~RenderBaseFactory() { Clear(); }

void RenderBaseFactory::Init() {
  this->RegisterUIRenderBase(ColorRenderMeta::Get());
  this->RegisterUIRenderBase(ImageRenderMeta::Get());
  this->RegisterUIRenderBase(ImageListRenderMeta::Get());
  this->RegisterUIRenderBase(ImageListItemRenderMeta::Get());
}

void RenderBaseFactory::Clear() {
  m_vecUIRenderBase.clear();
}

bool RenderBaseFactory::RegisterUIRenderBase(IRenderBaseMeta& meta) {
  m_vecUIRenderBase.push_back(&meta);
  UI_LOG_DEBUG("register render base %s", meta.Name());
  return true;
}

std::shared_ptr<IRenderBase> RenderBaseFactory::CreateRenderBaseByName(IResource *pSkinRes,
                                               const char *strName) {
  if (!strName || !strName[0])
    return nullptr;

  auto iter = m_vecUIRenderBase.begin();
  for (; iter != m_vecUIRenderBase.end(); ++iter) {
    IRenderBaseMeta* meta = *iter;
    if (!meta || !meta->Name())
      continue;

    if (strcmp(strName, meta->Name()) != 0)
      continue;

    std::shared_ptr<IRenderBase> out = meta->CreateShared(pSkinRes);
    if (out) {
      out->Init();
      out->SetType((RENDER_TYPE)meta->Detail().minor_type);
    }
    return out;
  }

  UI_LOG_WARN("Create Failed. Name=%s", strName);
  return nullptr;
}
std::shared_ptr<IRenderBase> RenderBaseFactory::CreateRenderBase(IResource *pSkinRes, int nType) {
  auto iter = m_vecUIRenderBase.begin();
  for (; iter != m_vecUIRenderBase.end(); iter++) {
    IRenderBaseMeta* meta = *iter;
    if (!meta)
      continue;

    if (meta->Detail().minor_type != nType)
      continue;

    std::shared_ptr<IRenderBase> out = meta->CreateShared(pSkinRes);
    if (out) {
      out->Init();
      out->SetType((RENDER_TYPE)nType);
    }

    return out;
  }

  UI_LOG_WARN("Create Failed. Type=%d", nType);
  return nullptr;
}

// 根据类型获取对应的xml name
const char *RenderBaseFactory::GetRenderBaseName(int nType) {
  auto iter = m_vecUIRenderBase.begin();
  for (; iter != m_vecUIRenderBase.end(); ++iter) {
    IMeta* meta = *iter;
    if (!meta || !meta->Name())
      continue;

    if (meta->Detail().minor_type != nType)
      continue;
    
    return meta->Name();
  }

  return nullptr;
}

void RenderBaseFactory::EnumRenderBaseName(
    pfnEnumRenderBaseNameCallback callback, llong wParam, llong lParam) {
  auto iter = m_vecUIRenderBase.begin();
  for (; iter != m_vecUIRenderBase.end(); ++iter) {
    IMeta* meta = *iter;
    if (!meta || !meta->Name())
      continue;

    callback(meta->Name(), wParam, lParam);
  }
}
} // namespace ui