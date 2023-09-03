#include "renderbase.h"
#include "include/inc.h"

#include "include/interface/iuires.h"
#include "include/interface/renderlibrary.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/object/object.h"
#include "src/resource/res_bundle.h"
// #include "src\UIObject\Window\windowbase.h"
// #include "src/Layer\windowrender.h"

#if 0 // defined(OS_WIN)
#pragma comment(lib, "uxtheme.lib")
#endif
using namespace ui;

RenderBase::RenderBase(IRenderBase *p) : Message(p) {
  m_pIRenderBase = p;
  m_pObject = nullptr;
  m_nRenderType = RENDER_TYPE_NULL;
  m_pUIApplication = nullptr;
}

Resource *RenderBase::GetResource() {
  if (!m_pObject)
    return nullptr;

  return m_pObject->GetResource();
}
ColorRes *RenderBase::GetSkinColorRes() {
  Resource *pSkinRes = GetResource();
  if (!pSkinRes)
    return nullptr;

  return &pSkinRes->GetColorRes();
}

ImageRes *RenderBase::GetSkinImageRes() {
  Resource *pSkinRes = GetResource();
  if (!pSkinRes)
    return nullptr;

  return &pSkinRes->GetImageRes();
}

void RenderBase::CheckThemeChanged() {
  if (this->IsThemeRender()) {
    static_cast<IMessage *>(m_pIRenderBase)->SendMessage(WM_THEMECHANGED);
  }
}

void RenderBase::_LoadColor(const char *szColorId, Color *&pColorRef) {
  // SAFE_RELEASE(pColorRef);
  // if (!szColorId)
  //   return;

  // ColorRes *pColorRes = GetSkinColorRes();
  // if (!pColorRes)
  //   return;

  // pColorRes->GetColor(szColorId, &pColorRef);
}
const char *RenderBase::_GetColorId(Color *&pColorRef) {
  // if (!pColorRef)
  //   return nullptr;

  // ColorRes *pColorRes = GetSkinColorRes();
  // if (pColorRes) {
  //   const char *szId = pColorRes->GetColorId(pColorRef);
  //   if (szId)
  //     return szId;
  // }

  // char *szBuffer = GetTempBuffer();
  // pColorRef->ToWebString(szBuffer);
  // return szBuffer;
  return nullptr;
}

void RenderBase::_LoadBitmap(const char *szBitmapId,
                             IRenderBitmap *&pBitmapRef) {
#if 0 // defined(OS_WIN)
  SAFE_RELEASE(pBitmapRef);
  if (!szBitmapId)
    return;

  if (m_pObject) {
    Resource *pSkinRes = m_pObject->GetResource();
    if (!pSkinRes)
      return;

    GRAPHICS_RENDER_LIBRARY_TYPE eType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
    WindowBase *pWnd = m_pObject->GetWindowObject();
    if (pWnd) {
      WindowRender *pRender = pWnd->GetWindowRender();
      if (pRender)
        eType = pRender->GetGraphicsRenderType();
    }

    pSkinRes->GetImageRes().GetBitmap(szBitmapId, eType, &pBitmapRef);
  } else {
    ImageRes *pImageRes = GetSkinImageRes();
    if (!pImageRes)
      return;

    pImageRes->GetBitmap(szBitmapId, UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI,
                         &pBitmapRef);
  }
#else
  UIASSERT(false);
#endif
}

const char *RenderBase::_GetBitmapId(IRenderBitmap *&pBitmapRef) {
  if (!pBitmapRef)
    return nullptr;

  ImageRes *pImageRes = GetSkinImageRes();
  if (!pImageRes)
    return nullptr;

  return pImageRes->GetRenderBitmapId(pBitmapRef);
}
