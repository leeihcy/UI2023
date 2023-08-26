#include "renderbase_factory.h"
#include "include/inc.h"
#include "src/application/uiapplication.h"
#include "src/private_inc.h"
#include "src/render/colorrender/colorrender.h"
#include "src/render/imagerender/imagerender.h"

namespace ui {
RenderBaseFactory::RenderBaseFactory(Application &app) : m_app(app) {}

RenderBaseFactory::~RenderBaseFactory() { Clear(); }

void RenderBaseFactory::Init() {
#define REGISTER_UI_RENDERBASE2(classname)                                     \
  this->RegisterUIRenderBaseCreateData(                                        \
      classname::GetXmlName(), classname::GetType(),                           \
      (ui::pfnUICreateRenderBasePtr)                                           \
          ObjectCreator<I##classname>::CreateInstance2);

  // REGISTER_UI_RENDERBASE(this, NullRender) -- 2015.4.1过期，不再使用
#if 0 // TODO:
  REGISTER_UI_RENDERBASE2(ColorRender);
  REGISTER_UI_RENDERBASE2(SysColorRender);
  REGISTER_UI_RENDERBASE2(GradientRender);
  REGISTER_UI_RENDERBASE2(ColorListRender);
  REGISTER_UI_RENDERBASE2(ImageRender);
  REGISTER_UI_RENDERBASE2(ImageListItemRender);
  REGISTER_UI_RENDERBASE2(ImageListRender);
#endif
}

void RenderBaseFactory::Clear() {
  UIRENDERBASE_CREATE_DATA::iterator iter = m_vecUIRenderBaseCreateData.begin();
  for (; iter != m_vecUIRenderBaseCreateData.end(); iter++) {
    SAFE_DELETE(*iter);
  }
  m_vecUIRenderBaseCreateData.clear();
}

bool RenderBaseFactory::RegisterUIRenderBaseCreateData(
    const char *bstrName, int nType, pfnUICreateRenderBasePtr pfunc) {
  if (nullptr == bstrName || nullptr == pfunc)
    return false;

  std::string strName(bstrName);

  UIRENDERBASE_CREATE_INFO *pInfo = new UIRENDERBASE_CREATE_INFO;
  pInfo->m_func = pfunc;
  pInfo->m_nRenderType = nType;
  pInfo->m_strName = strName;
  m_vecUIRenderBaseCreateData.push_back(pInfo);

  // UI_LOG_DEBUG(_T("%s, type=%d, ctrl=%d @ 0x%08X"), bstrName, nType, pfunc);
  return true;
}

bool RenderBaseFactory::CreateRenderBaseByName(IResource *pSkinRes,
                                               const char *strName,
                                               IObject *pObject,
                                               IRenderBase **ppOut) {
  if (!strName || !strName[0] || !pObject || !ppOut)
    return false;

  UIRENDERBASE_CREATE_DATA::iterator iter = m_vecUIRenderBaseCreateData.begin();
  for (; iter != m_vecUIRenderBaseCreateData.end(); ++iter) {
    UIRENDERBASE_CREATE_INFO *pData = *iter;
    if (!pData)
      continue;

    if (pData->m_strName != strName)
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

    pData->m_func(pSkinRes, (void **)ppOut);
    if (*ppOut) {
      (*ppOut)->SetObject(m_app.GetIUIApplication(), pObject);
      (*ppOut)->Init();
      (*ppOut)->SetType((RENDER_TYPE)pData->m_nRenderType);
      return true;
    }

    return false;
  }

  UI_LOG_WARN(_T("Create Failed. Name=%s"), strName);
  return false;
}
bool RenderBaseFactory::CreateRenderBase(IResource *pSkinRes, int nType,
                                         IObject *pObject,
                                         IRenderBase **ppOut) {
  if (nullptr == ppOut)
    return false;

  UIRENDERBASE_CREATE_DATA::iterator iter = m_vecUIRenderBaseCreateData.begin();
  for (; iter != m_vecUIRenderBaseCreateData.end(); iter++) {
    UIRENDERBASE_CREATE_INFO *pData = *iter;
    if (nullptr == pData)
      continue;

    if (pData->m_nRenderType != nType)
      continue;

    pData->m_func(pSkinRes, (void **)ppOut);
    if (*ppOut) {
      (*ppOut)->SetObject(m_app.GetIUIApplication(), pObject);
      (*ppOut)->Init();
      (*ppOut)->SetType((RENDER_TYPE)nType);
      return true;
    }

    return false;
  }

  UI_LOG_WARN(_T("Create Failed. Type=%d"), nType);
  return false;
}

// 根据类型获取对应的xml name
const char *RenderBaseFactory::GetRenderBaseName(int nType) {
  UIRENDERBASE_CREATE_DATA::iterator iter = m_vecUIRenderBaseCreateData.begin();
  for (; iter != m_vecUIRenderBaseCreateData.end(); ++iter) {
    UIRENDERBASE_CREATE_INFO *pData = *iter;
    if (nullptr == pData)
      continue;

    if (pData->m_nRenderType != nType)
      continue;

    return pData->m_strName.c_str();
  }

  return nullptr;
}

void RenderBaseFactory::EnumRenderBaseName(
    pfnEnumRenderBaseNameCallback callback, long wParam, long lParam) {
  UIRENDERBASE_CREATE_DATA::iterator iter = m_vecUIRenderBaseCreateData.begin();
  for (; iter != m_vecUIRenderBaseCreateData.end(); ++iter) {
    UIRENDERBASE_CREATE_INFO *pData = *iter;
    if (nullptr == pData)
      continue;

    callback(pData->m_strName.c_str(), wParam, lParam);
  }
}
} // namespace ui