#include "src/control/text/label.h"
#include "include/macro/msg.h"
#include "src/attribute/attribute.h"
#include "src/control/text/text_meta.h"

namespace ui {

Label::Label(ILabel *p) : Control(p), m_pILabel(p) {
}

void Label::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_PAINT) {
    onPaint(static_cast<PaintMessage *>(msg)->rt);
    return;
  } else if (msg->message == UI_MSG_GETDESIREDSIZE) {
    onGetDesiredSize(&static_cast<GetDesiredSizeMessage *>(msg)->size);
    return;
  } else if (msg->message == UI_MSG_SERIALIZE) {
    auto *m = static_cast<SerializeMessage *>(msg);
    onSerialize(m->param);
    return;
  } else if (msg->message == UI_MSG_QUERYINTERFACE) {
    auto *m = static_cast<QueryInterfaceMessage *>(msg);
    if (m->uuid == LabelMeta::Get().UUID()) {
      *(m->pp) = m_pIControl;
      return;
    }
  } else if (msg->message == UI_MSG_FINALCONSTRUCT) {
    onFinalConstruct(static_cast<FinalConstructMessage *>(msg));
    return;
  }
  Control::onRouteMessage(msg);
}

void Label::onFinalConstruct(FinalConstructMessage *msg) {
  Control::onRouteMessage(msg);

  OBJSTYLE s = {0};
  s.default_transparent = 1;          // 默认透明
  s.default_reject_all_mouse_msg = 1; // 默认不接收鼠标消息

  OBJSTYLE sRemove = {0};
  sRemove.default_tabstop = 1; // 没有焦点
  ModifyObjectStyle(&s, &sRemove);
}

const char *Label::GetText() { return m_text.c_str(); }
void Label::SetText(const char *text) {
  if (text) {
    if (m_text == text) {
      return;
    }
    m_text = text;
  } else {
    if (m_text.empty()) {
      return;
    }
    m_text.clear();
  }
  onTextChanged();
}

void Label::onTextChanged() {
#if 0
ILayoutParam*  param = m_pILabel->GetSafeLayoutParam();
	if (param && param->IsSizedByContent())
	{
		UI::IObject* parent = m_pILabel->GetParentObject();
		if (parent)
		{
			UI::ILayout* pLayout = (UI::ILayout*)
				UISendMessage(parent, UI_MSG_GETLAYOUT);
			if (pLayout)
			{
				pLayout->SetDirty(true);
				parent->Invalidate();
			}
		}
	}
	m_pILabel->Invalidate();
#endif
}

void Label::onPaint(IRenderTarget *rt) {
  if (m_text.empty()) {
    return;
  }
  Rect rect;
  GetClientRectWithZeroOffset(&rect);

  ITextRenderBase *p = GetTextRenderOrDefault();
  if (p) {
    p->DrawState(rt, &rect, 0, m_text.c_str());
  }
}

void Label::onSerialize(SerializeParam *pData) {
  Control::onSerialize(pData);

  AttributeSerializer s(pData, "Label");
  s.AddI18nString(XML_TEXT, m_text)->AsData();
}

void Label::onGetDesiredSize(Size *size) {
  size->width = size->height = 0;

#if 0 // TODO:
  ITextRenderBase *pTextRender = m_pILabel->GetTextRenderDefault();
  if (pTextRender) {
    LPCTSTR szText = m_strText.c_str();
    if (0 == szText[0]) {
      // TBD: 内容为空时，至少保证高度
      szText = TEXT(" ");
      *pSize = pTextRender->GetDesiredSize(szText, m_pILabel->GetMaxWidth());
      pSize->cx = 0;
    } else {
      *pSize = pTextRender->GetDesiredSize(szText, m_pILabel->GetMaxWidth());
    }
  }
#endif
  REGION4 rcNonClient;
  m_pILabel->GetNonClientRegion(&rcNonClient);

  size->width += rcNonClient.left + rcNonClient.right;
  size->height += rcNonClient.top + rcNonClient.bottom;
}

} // namespace ui