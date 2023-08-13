#include "panel.h"
#include "include/interface/ilayout.h"
#include "include/interface/imapattr.h"
#include "include/interface/irenderbase.h"
#include "include/interface/iwindow.h"
#include "src/window/window.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/attribute/stringselect_attribute.h"
#include "src/layout/layout.h"

namespace ui {

Panel::Panel(IPanel *p) : Object(p) {
  m_pIPanel = p;
  m_pLayout = nullptr;

  m_pTextureRender = nullptr;
  m_rcTextureRenderRegion.SetEmpty();

  m_pMaskRender = nullptr;
  m_rcMaskRenderRegion.SetEmpty();

  m_rcBkgndRenderRegion.SetEmpty();
  m_rcForegndRenderRegion.SetEmpty();

  OBJSTYLE s = {0};
  s.default_transparent = 1;
  s.default_reject_self_mouse_msg = 1;
  ModifyObjectStyle(&s, 0);

  this->SetDescription(PanelDescription::Get());
}

Panel::~Panel() {
  SAFE_RELEASE(m_pLayout);
  SAFE_RELEASE(m_pTextureRender);
  SAFE_RELEASE(m_pMaskRender);
}

ILayout *Panel::GetLayout() { return this->m_pLayout; }

long Panel::GetLayoutType2() {
  return (long)GetLayoutType();
}
void Panel::SetLayoutType2(long eLayoutType) {
  SetLayoutType((LAYOUTTYPE)eLayoutType);
}

void Panel::SetLayoutType(LAYOUTTYPE eLayoutType) {
  if (m_pLayout && m_pLayout->GetLayoutType() == eLayoutType)
    return;

  SAFE_RELEASE(m_pLayout);

  GetUIApplication()->GetLayoutFactory().Create(eLayoutType, m_pIPanel,
                                                &m_pLayout);

  // 子结点的布局类型全部跟着变
  Object *pChild = nullptr;
  while ((pChild = EnumChildObject(pChild))) {
    pChild->DestroyLayoutParam();
    pChild->GetSafeLayoutParam();
  }
}

LAYOUTTYPE Panel::GetLayoutType() {
  if (!m_pLayout)
    return LAYOUT_TYPE_CANVAS;

  return m_pLayout->GetLayoutType();
}

long Panel::OnGetLayoutPtr(unsigned int uMsg, long wParam, long lParam) {
  return (long)m_pLayout;
}

void Panel::virtualOnSize(unsigned int nType, unsigned int cx,
                          unsigned int cy) {
  Object::virtualOnSize(nType, cx, cy);
  if (m_pLayout)
    m_pLayout->Arrange(nullptr);
}

void Panel::OnGetDesiredSize(Size *pSize) {
  pSize->width = pSize->height = 0;

  if (this->m_pLayout)
    *pSize = this->m_pLayout->Measure();

  Rect rcNonClient = {0};
  GetNonClientRegion(&rcNonClient);

  pSize->width += rcNonClient.left + rcNonClient.right;
  pSize->height += rcNonClient.top + rcNonClient.bottom;
}

void Panel::OnSerialize(SERIALIZEDATA *pData) {
  Object::OnSerialize(pData);

  if (pData->IsReload()) {
    SAFE_RELEASE(m_pLayout);
    m_objStyle.post_paint = 0;
  }

  {
    AttributeSerializer s(pData, TEXT("Panel"));

    // 纹理层
    s.AddRenderBase(XML_TEXTURE_RENDER_PREFIX, this, m_pTextureRender);
    s.AddRect(XML_TEXTURE_RENDER_PREFIX XML_PANEL_RENDER_REGION,
              m_rcTextureRenderRegion);

    // 顶层遮罩层
    s.AddRenderBase(XML_MASK_RENDER_PREFIX, this, m_pMaskRender);
    s.AddRect(XML_MASK_RENDER_PREFIX XML_PANEL_RENDER_REGION,
              m_rcMaskRenderRegion);

    s.AddRect(XML_BACKGND_RENDER_PREFIX XML_PANEL_RENDER_REGION,
              m_rcBkgndRenderRegion);
    s.AddRect(XML_FOREGND_RENDER_PREFIX XML_PANEL_RENDER_REGION,
              m_rcForegndRenderRegion);

    s.AddEnum(XML_LAYOUT_TYPE,
              Slot(&Panel::SetLayoutType2, this),
              Slot(&Panel::GetLayoutType2, this))
        ->FillLayoutTypeData();
  }
  if (pData->IsLoad()) {
    if (m_pMaskRender) {
      m_objStyle.post_paint = 1;
    }
    if (!m_pLayout) {
      SetLayoutType(LAYOUT_TYPE_CANVAS);
    }
  }

  if (m_pLayout) {
    SERIALIZEDATA data(*pData);
    data.szParentKey = XML_LAYOUT;
    m_pLayout->Serialize(&data);
  }
}

void Panel::OnEraseBkgnd(IRenderTarget *pRenderTarget) {
  if (m_pLayout && m_pLayout->IsDirty()) {
    m_pLayout->Arrange(nullptr);
  }

  Rect rc = {0, 0, this->GetWidth(), this->GetHeight()};

  if (m_pBkgndRender) {
    Rect rcBkgnd = rc;
    rcBkgnd.Deflate(m_rcBkgndRenderRegion);
    m_pBkgndRender->DrawState(pRenderTarget, &rcBkgnd, 0);
  }

  if (m_pTextureRender) {
    Rect rcTextureRegion = rc;
    rcTextureRegion.Deflate(m_rcTextureRenderRegion);
    m_pTextureRender->DrawState(pRenderTarget, &rcTextureRegion, 0);
  }
}

void Panel::OnPaint(IRenderTarget *pRenderTarget) {
  if (m_pForegndRender) {
    Rect rcForegnd = {0, 0, this->GetWidth(), this->GetHeight()};
    rcForegnd.Deflate(m_rcForegndRenderRegion);
    m_pForegndRender->DrawState(pRenderTarget, &rcForegnd, 0);
  }
}
void Panel::OnPostPaint(IRenderTarget *pRenderTarget) {
  if (m_pMaskRender) {
    Rect rcMask = {0, 0, this->GetWidth(), this->GetHeight()};
    rcMask.Deflate(m_rcMaskRenderRegion);
    m_pMaskRender->DrawState(pRenderTarget, &rcMask, 0);
  }
}

void Panel::SetTextureRender(IRenderBase *p) {
  SAFE_RELEASE(m_pTextureRender);
  m_pTextureRender = p;

  if (m_pTextureRender)
    m_pTextureRender->AddRef();
}
IRenderBase *Panel::GetTextureRender() { return m_pTextureRender; }

} // namespace ui
