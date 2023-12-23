#include "control.h"
#include "control_meta.h"
#include "include/interface/ilayout.h"
#include "include/interface/itextrenderbase.h"
#include "include/interface/iuiapplication.h"
#include "src/application/uiapplication.h"
#include "src/attribute/attribute.h"
#include "src/attribute/bool_attribute.h"
#include "src/attribute/stringselect_attribute.h"
#include "src/helper/scale/scale_factor.h"
#include "src/resource/res_bundle.h"


namespace ui {

Control::Control(IControl *p) : Object(p) {
  m_pIControl = p;
  memset(&m_controlStyle, 0, sizeof(m_controlStyle));
  m_objStyle.tabstop = 1;
  m_objStyle.default_tabstop = 1;

  m_meta = &ControlMeta::Get();
}

void Control::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_ERASEBKGND) {
    onEraseBkgnd(static_cast<EraseBkgndMessage*>(msg)->rt);
    return;
  }
  else if (msg->message == UI_MSG_GETDESIREDSIZE) {
    onGetDesiredSize(&static_cast<GetDesiredSizeMessage*>(msg)->size);
    return;
  }
  else if (msg->message == UI_MSG_SERIALIZE) {
    auto* m = static_cast<SerializeMessage*>(msg);
    onSerialize(m->param);
    return;
  }
  else if (msg->message == UI_MSG_QUERYINTERFACE) {
    auto* m = static_cast<QueryInterfaceMessage*>(msg);
    if (m->uuid == ControlMeta::Get().UUID()) {
      *(m->pp) = m_pIControl;
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

    s.AddBool(XML_CONTROL_STYLE_TABABLE,
              Slot(&Object::SetTabstop, this),
              Slot(&Object::IsTabstop, this))
        ->SetDefault(m_objStyle.default_tabstop);

    s.AddBool(XML_CONTROL_STYLE_GROUP,
              Slot(&Control::SetGroup, this),
              Slot(&Control::IsGroup, this));

    s.AddString(XML_TOOLTIP,
                Slot(&Control::SetToolTipText, this),
                Slot(&Control::GetToolTipText, this));

    s.AddTextRenderBase(nullptr, this, m_pTextRender);
    // 		s.AddStringEnum(XML_TEXTRENDER_TYPE,
    // 			Slot(&Object::LoadTextRender, this),
    // 			Slot(&Object::SaveTextRender, this))
    // 			->FillTextRenderBaseTypeData()
    // 			->ReloadOnChanged();
    //
    // 		// ����
    // 		if (m_pTextRender)
    // 		{
    // 			SerializeParam data(*pData);
    // 			data.szParentKey = XML_TEXTRENDER_TYPE;
    // 			if (data.pUIApplication->IsDesignMode())
    // 				data.SetErase(false);
    // 			m_pTextRender->Serialize(&data);
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

// ��ȡ���壬���û�У���ʹ��Ĭ�ϵ�
ITextRenderBase *Control::GetTextRenderDefault() {
  if (!m_pTextRender)
    CreateDefaultTextRender();

  return m_pTextRender;
}

// ���û����Ƥ�����������壬���ⲿ�ɵ��øú�����Paintʱ����һ��Ĭ�ϵ�����
ITextRenderBase *Control::CreateDefaultTextRender() {
  // if (!m_pTextRender) {
  //   GetUIApplication()->GetTextRenderFactroy().CreateTextRender(
  //       m_pSkinRes->GetIResource(), TEXTRENDER_TYPE_SIMPLE, m_pIObject,
  //       &m_pTextRender);

  //   if (m_pTextRender) {
  //     IMapAttribute *pMapAttr = m_pIMapAttributeRemain;
  //     if (!pMapAttr)
  //       UICreateIMapAttribute(&pMapAttr);

  //     SerializeParam data = {0};
  //     data.pUIApplication = GetUIApplication()->GetIUIApplication();
  //     data.pSkinRes = m_pSkinRes->GetIResource();
  //     data.pMapAttrib = pMapAttr;
  //     data.szPrefix = nullptr;
  //     data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
  //     m_pTextRender->Serialize(&data);

  //     if (!m_pIMapAttributeRemain)
  //       pMapAttr->Release();
  //   }
  // }

  // return m_pTextRender;
  return nullptr;
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

//   const wchar_t *szText = m_pSkinRes->GetI18nRes().MapConfigValue(szTooltip);
//   pToolTip->SetText(szText);
//   return 1;
// }

// ���ؼ����ݸı�ʱ�����һ���Ƿ���Ҫ���²��֣���label.text, button.text
void Control::TryUpdateLayoutOnContentChanged() {
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
}

void Control::onEraseBkgnd(IRenderTarget *pRenderTarget) {
  Rect rc = {0, 0, this->GetWidth(), this->GetHeight()};
  if (m_pBkgndRender) {
    m_pBkgndRender->DrawState(pRenderTarget, &rc, 0);
  }
}

void Control::onGetDesiredSize(Size *pSize) {
  pSize->width = pSize->height = 0;
  if (m_pBkgndRender) {
    *pSize = ScaleFactorHelper::Scale(m_pBkgndRender->GetDesiredSize(), this);
  }
}

} // namespace ui