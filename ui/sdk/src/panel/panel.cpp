#include "panel.h"
#include "panel_meta.h"
#include "include/interface/ilayout.h"
#include "include/interface/iattributemap.h"
#include "include/interface/irenderbase.h"
#include "include/interface/iwindow.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/attribute/stringselect_attribute.h"
#include "src/helper/scale/scale_factor.h"
#include "src/layout/layout.h"
#include "src/object/layout/layout_object.h"
#include "src/window/window.h"
#include <memory>


namespace ui {

Panel::Panel(IPanel *p) : Object(p) {
  m_pIPanel = p;

  OBJSTYLE s = {0};
  s.default_transparent = 1;
  s.default_reject_self_mouse_msg = 1;
  ModifyObjectStyle(&s, 0);
}

Panel::~Panel() {
  SAFE_RELEASE(m_pLayout);
}

void Panel::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_PAINTBKGND) {
    onPaintBkgnd(static_cast<PaintBkgndMessage*>(msg)->rt);
    return;
  }
  else if (msg->message == UI_MSG_PAINT) {
    onPaint(static_cast<PaintBkgndMessage*>(msg)->rt);
    return;
  }
  else if (msg->message == UI_MSG_POSTPAINT) {
    onPostPaint(static_cast<PostPaintMessage*>(msg)->rt);
    return;
  }
  else if (msg->message == UI_MSG_GETDESIREDSIZE) {
    onGetDesiredSize(&static_cast<GetDesiredSizeMessage*>(msg)->size);
    return;
  }
  else if (msg->message == UI_MSG_GETLAYOUT) {
    static_cast<GetLayoutMessage*>(msg)->layout = m_pLayout;
    return;
  }
  if (msg->message == UI_MSG_QUERYINTERFACE) {
    auto* m = static_cast<QueryInterfaceMessage*>(msg);
    if (m->uuid == PanelMeta::Get().UUID()) {
      *(m->pp) = m_pIPanel;
      return;
    }
  }
  else if (msg->message == UI_MSG_SERIALIZE) {
    auto* m = static_cast<SerializeMessage*>(msg);
    onSerialize(m->param);
    return;
  }
  Object::onRouteMessage(msg);
}

ILayout *Panel::GetLayout() { return this->m_pLayout; }

const char *Panel::GetLayoutName() { return m_layout_name.c_str(); }
void Panel::SetLayoutName(const char *name) {
  if (!name) {
    m_layout_name.clear();
  } else {
    m_layout_name = name;
  }
}

void Panel::SetLayoutType(const char *layout_name) {
  // if (m_pLayout && m_pLayout->GetLayoutType() == eLayoutType)
  //   return;

  ILayout *layout = nullptr;
  GetUIApplication()->GetLayoutFactory().CreateByName(layout_name, m_pIPanel,
                                                      false, &layout);
  SetLayout(layout);
}
void Panel::SetLayout(ILayout *p) {

  SAFE_RELEASE(m_pLayout);
  m_pLayout = p;

  // 子结点的布局类型全部跟着变
  Object *pChild = nullptr;
  while ((pChild = EnumChildObject(pChild))) {
    LayoutObject* layout_object = pChild->GetLayoutObject();
    if (!layout_object) {
      continue;
    }
    layout_object->DestroyLayoutParam();
    layout_object->GetSafeLayoutParam();
  }
}

void Panel::virtualOnSize(unsigned int nType, unsigned int cx,
                          unsigned int cy, float scale) {
  Object::virtualOnSize(nType, cx, cy, scale);
  if (m_pLayout) {
    ArrangeParam param = {
      .obj_to_arrange = nullptr,
      .reason = ArrangeReason::NoReason,
      .scale = scale,
    };
    m_pLayout->Arrange(param);
  }
}

void Panel::onGetDesiredSize(Size *pSize) {
  pSize->width = pSize->height = 0;

  if (this->m_pLayout && m_pChild)
    *pSize = this->m_pLayout->Measure();
  else if (m_back_render) {
    *pSize = ScaleFactorHelper::Scale(m_back_render->GetDesiredSize(), this);
  }

  Rect rcNonClient = {0};
  GetNonClientRegion(&rcNonClient);

  pSize->width += rcNonClient.left + rcNonClient.right;
  pSize->height += rcNonClient.top + rcNonClient.bottom;
}

void Panel::onSerialize(SerializeParam *pData) {
  Object::onSerialize(pData);

  if (pData->IsReload()) {
    SAFE_RELEASE(m_pLayout);
    m_objStyle.post_paint = 0;
  }

  {
    AttributeSerializer s(pData, "Panel");

    // 纹理层
    s.AddRenderBase(XML_TEXTURE_RENDER_PREFIX, m_pTextureRender);
    // 顶层遮罩层
    s.AddRenderBase(XML_MASK_RENDER_PREFIX, m_pMaskRender);
    s.AddStringEnum(XML_LAYOUT_TYPE, Slot(&Panel::SetLayoutName, this),
                    Slot(&Panel::GetLayoutName, this))
        ->FillLayoutTypeData();
  }
  if (pData->IsLoad()) {
    if (m_pMaskRender) {
      m_objStyle.post_paint = 1;
    }
    if (!m_pLayout) {
      if (!m_layout_name.empty()) {
        SetLayoutType(m_layout_name.c_str());
      } else {
        SetLayoutType(XML_LAYOUT_CANVAS);
      }
    }
  }

  if (m_pLayout) {
    SerializeParam data(*pData);
    data.szParentKey = XML_LAYOUT;
    m_pLayout->Serialize(&data);
  }
}

void Panel::onPaintBkgnd(IRenderTarget *pRenderTarget) {
  if (m_pLayout && m_pLayout->IsDirty()) {
    float scale = 1.0f;
    auto* window = GetWindow();
    if (window) {
      scale = window->m_dpi.GetScaleFactor();
    }
    ArrangeParam param = {
      .obj_to_arrange = nullptr,
      .reason = ArrangeReason::NoReason,
      .scale = scale,
    };

    m_pLayout->Arrange(param);
  }

  Rect rc = {0, 0, this->GetWidth(), this->GetHeight()};

  if (m_back_render) {
    Rect rcBkgnd = rc;
    m_back_render->DrawState(pRenderTarget, &rcBkgnd, 0);
  }

  if (m_pTextureRender) {
    Rect rcTextureRegion = rc;
    m_pTextureRender->DrawState(pRenderTarget, &rcTextureRegion, 0);
  }
}

void Panel::onPaint(IRenderTarget *pRenderTarget) {
  if (m_fore_render) {
    Rect rcForegnd = {0, 0, this->GetWidth(), this->GetHeight()};
    m_fore_render->DrawState(pRenderTarget, &rcForegnd, 0);
  }
}
void Panel::onPostPaint(IRenderTarget *pRenderTarget) {
  if (m_pMaskRender) {
    Rect rcMask = {0, 0, this->GetWidth(), this->GetHeight()};
    m_pMaskRender->DrawState(pRenderTarget, &rcMask, 0);
  }
}

void Panel::SetTextureRender(std::shared_ptr<IRenderBase> p) {
  m_pTextureRender = p;
}
std::shared_ptr<IRenderBase> Panel::GetTextureRender() { return m_pTextureRender; }

} // namespace ui
