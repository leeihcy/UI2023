#include "control.h"
#include "control_meta.h"
#include "include/interface/ilayout.h"
#include "include/interface/itextrenderbase.h"
#include "include/interface/iuiapplication.h"
#include "include/macro/msg.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/attribute/bool_attribute.h"
#include "src/helper/scale/scale_factor.h"
#include "src/resource/res_bundle.h"
#include <memory>

namespace ui {

Control::Control(IControl *p) : Object(p) {
  memset(&m_controlStyle, 0, sizeof(m_controlStyle));
  m_objStyle.tabstop = 1;
  m_objStyle.default_tabstop = 1;
}

void Control::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_PAINTBKGND) {
    onPaintBkgnd(static_cast<PaintBkgndMessage *>(msg)->rt);
    return;
  } else if (msg->message == UI_MSG_PAINT) {
    onPaint(static_cast<PaintMessage *>(msg)->rt);
    return;
  } 
  else if (msg->message == UI_MSG_GETDESIREDSIZE) {
    onGetDesiredSize(&static_cast<GetDesiredSizeMessage *>(msg)->size);
    return;
  } else if (msg->message == UI_MSG_SERIALIZE) {
    auto *m = static_cast<SerializeMessage *>(msg);
    onSerialize(m->param);
    return;
  } else if (msg->message == UI_MSG_QUERYINTERFACE) {
    auto *m = static_cast<QueryInterfaceMessage *>(msg);
    if (m->uuid == ControlMeta::Get().UUID()) {
      *(m->pp) = GetIControl();
      return;
    }
  }
  // UIMSG_GET_TOOLTIPINFO(OnGetToolTipInfo)
  Object::onRouteMessage(msg);
}

void Control::onSerialize(SerializeParam *pData) {
  Object::onSerialize(pData);

  {
    AttributeSerializer s(pData, "Control");

    s.AddBool(XML_CONTROL_STYLE_TABABLE, Slot(&Object::SetTabstop, this),
              Slot(&Object::IsTabstop, this))
        ->SetDefault(m_objStyle.default_tabstop);

    s.AddBool(XML_CONTROL_STYLE_GROUP, Slot(&Control::SetGroup, this),
              Slot(&Control::IsGroup, this));

    s.AddString(XML_TOOLTIP, Slot(&Control::SetToolTipText, this),
                Slot(&Control::GetToolTipText, this));

    s.AddTextRenderBase(nullptr, m_text_render);
    // 		s.AddStringEnum(XML_TEXTRENDER_TYPE,
    // 			Slot(&Object::LoadTextRender, this),
    // 			Slot(&Object::SaveTextRender, this))
    // 			->FillTextRenderBaseTypeData()
    // 			->ReloadOnChanged();
    //
    // 		// 字体
    // 		if (m_text_render)
    // 		{
    // 			SerializeParam data(*pData);
    // 			data.szParentKey = XML_TEXTRENDER_TYPE;
    // 			if (data.pUIApplication->IsDesignMode())
    // 				data.SetErase(false);
    // 			m_text_render->Serialize(&data);
    // 		}
  }
}

void Control::ModifyControlStyle(ControlStyle *add, ControlStyle *remove) {
  if (add) {
    if (add->group)
      m_controlStyle.group = 1;
    if (add->ownerdraw)
      m_controlStyle.ownerdraw = 1;
  }

  if (remove) {
    if (remove->group)
      m_controlStyle.group = 0;
    if (remove->ownerdraw)
      m_controlStyle.ownerdraw = 0;
  }
}
bool Control::TestControlStyle(ControlStyle *test) {
  if (!test)
    return false;

  if (test->group && m_controlStyle.group)
    return true;
  if (test->ownerdraw && m_controlStyle.ownerdraw)
    return true;

  return false;
}

bool Control::IsGroup() { return m_controlStyle.group; }
void Control::SetGroup(bool b) { m_controlStyle.group = b; }

// 获取字体，如果没有，则使用默认的
std::shared_ptr<ITextRenderBase> Control::GetTextRenderOrDefault() {
  if (!m_text_render)
    CreateDefaultTextRender();

  return m_text_render;
}

// 如果没有在皮肤中配置字体，则外部可调用该函数在Paint时创建一个默认的字体
std::shared_ptr<ITextRenderBase> Control::CreateDefaultTextRender() {
  if (m_text_render) {
    return m_text_render;
  }

  m_text_render = GetUIApplication()->GetTextRenderFactroy().CreateTextRender(
      m_resource->GetIResource(), TEXTRENDER_TYPE_SIMPLE);

  if (m_text_render) {
    std::shared_ptr<IAttributeMap> pMapAttr = m_attribute_map_remaining;
    if (!pMapAttr)
      pMapAttr = UICreateIMapAttribute();

    SerializeParam data = {0};
    data.resource = m_resource->GetIResource();
    data.attribute_map = pMapAttr.get();
    data.szPrefix = nullptr;
    data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
    m_text_render->Serialize(&data);
  }

  return m_text_render;
}

const char *Control::GetToolTipText() {
  return GetAttribute(XML_TOOLTIP, false);
}
void Control::SetToolTipText(const char *szText) {
  if (!szText || 0 == szText[0]) {
    GetAttribute(XML_TOOLTIP, true);
  } else {
    AddAttribute(XML_TOOLTIP, szText);
  }
}

// BOOL Control::OnGetToolTipInfo(TOOLTIPITEM *pToolTipItem,
//                                IToolTipUI *pToolTip) {
//   if (nullptr == pToolTipItem || nullptr == pToolTip)
//     return 0;

//   const wchar_t *szTooltip = GetToolTipText();
//   if (!szTooltip || !szTooltip[0])
//     return 0;

//   const wchar_t *szText = m_resource_bundle->GetI18nRes().MapConfigValue(szTooltip);
//   pToolTip->SetText(szText);
//   return 1;
// }

// 当控件内容改变时，检查一下是否需要更新布局，如label.text, button.text
void Control::TryUpdateLayoutOnContentChanged() {
  assert(false);
#if 0
  Object *pObj = this;
  while (pObj) {
    ILayoutParam *param = pObj->GetSafeLayoutParam();
    if (!param /*|| !param->IsSizedByContent()*/)
      break;

    Object *parent = pObj->GetParentObject();
    if (!parent)
      break;

    GetLayoutMessage msg;
    parent->RouteMessage(&msg);
    ui::ILayout *pLayout = msg.layout;
    if (!pLayout)
      break;

    pLayout->SetDirty(true);
    parent->Invalidate();
    pObj = parent;
  }
#endif
}

void Control::onPaintBkgnd(IRenderTarget *r) {
  if (!m_back_render) {
    return;
  }
  Rect rc = {0, 0, this->GetWidth(), this->GetHeight()};
  m_back_render->DrawState(r, &rc, 0);
}

void Control::onPaint(IRenderTarget *r) {
  if (!m_fore_render) {
    return;
  }
  Rect rc;
  GetClientRectWithZeroOffset(&rc);
  m_fore_render->DrawState(r, &rc, 0);
}

void Control::onGetDesiredSize(Size *pSize) {
  pSize->width = pSize->height = 0;
  if (m_back_render) {
    // *pSize = ScaleFactorHelper::Scale(m_back_render->GetDesiredSize(),
    // this);
    *pSize = m_back_render->GetDesiredSize();
  }
}

} // namespace ui