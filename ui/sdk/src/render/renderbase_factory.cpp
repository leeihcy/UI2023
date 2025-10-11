#include "renderbase_factory.h"
#include "include/inc.h"
#include "include/macro/msg.h"
#include "src/application/uiapplication.h"
#include "src/private_inc.h"
#include "src/render/colorrender/colorrender.h"
#include "src/render/imagerender/imagerender.h"
#include "render_meta.h"

namespace ui {
RenderBaseFactory::RenderBaseFactory(Application &app) : m_app(app) {}

RenderBaseFactory::~RenderBaseFactory() { Clear(); }

void RenderBaseFactory::Init() {
  // REGISTER_UI_RENDERBASE(this, NullRender) -- 2015.4.1过期，不再使用

  this->RegisterUIRenderBase(ColorRenderMeta::Get());
  // this->RegisterUIRenderBase(GradientRenderMeta::Get());
  this->RegisterUIRenderBase(ColorListRenderMeta::Get());
  this->RegisterUIRenderBase(ImageRenderMeta::Get());
  this->RegisterUIRenderBase(ImageListRenderMeta::Get());
  this->RegisterUIRenderBase(ImageListItemRenderMeta::Get());
}

void RenderBaseFactory::Clear() {
  m_vecUIRenderBase.clear();
}

bool RenderBaseFactory::RegisterUIRenderBase(IMeta& meta) {
  m_vecUIRenderBase.push_back(&meta);
  // UI_LOG_DEBUG("%s, type=%d, ctrl=%d @ 0x%08X", bstrName, nType, pfunc);
  return true;
}

bool RenderBaseFactory::CreateRenderBaseByName(IResource *pSkinRes,
                                               const char *strName,
                                               IObject *pObject,
                                               IRenderBase **ppOut) {
  if (!strName || !strName[0] || !pObject || !ppOut)
    return false;

  auto iter = m_vecUIRenderBase.begin();
  for (; iter != m_vecUIRenderBase.end(); ++iter) {
    IMeta* meta = *iter;
    if (!meta || !meta->Name())
      continue;

    if (strcmp(strName, meta->Name()) != 0)
      continue;

    // 废弃
    //      if (-1 != pData->m_nControlType)
    //      {
    //          if (pObject->GetObjectExtentType() != pData->m_nControlType)
    //              continue;
    //      }
    //
    //      if (-1 != pData->m_nControlSubType)
    //      {
    //          int  nStylyEx = pObject->GetStyleEx();
    //          if (GETCONTROLSUBTYPE(nStylyEx) != pData->m_nControlSubType)
    //              continue;
    //      }

    meta->Create(pSkinRes, (void **)ppOut);
    if (*ppOut) {
      (*ppOut)->SetObject(m_app.GetIUIApplication(), pObject);
      (*ppOut)->Init();
      (*ppOut)->SetType((RENDER_TYPE)meta->Detail().minor_type);
      return true;
    }

    return false;
  }

  UI_LOG_WARN("Create Failed. Name=%s", strName);
  return false;
}
bool RenderBaseFactory::CreateRenderBase(IResource *pSkinRes, int nType,
                                         IObject *pObject,
                                         IRenderBase **ppOut) {
  if (nullptr == ppOut)
    return false;

  auto iter = m_vecUIRenderBase.begin();
  for (; iter != m_vecUIRenderBase.end(); iter++) {
    IMeta* meta = *iter;
    if (!meta)
      continue;

    if (meta->Detail().minor_type != nType)
      continue;

    meta->Create(pSkinRes, (void **)ppOut);
    if (*ppOut) {
      (*ppOut)->SetObject(m_app.GetIUIApplication(), pObject);
      (*ppOut)->Init();
      (*ppOut)->SetType((RENDER_TYPE)nType);
      return true;
    }

    return false;
  }

  UI_LOG_WARN("Create Failed. Type=%d", nType);
  return false;
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