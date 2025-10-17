#include "button.h"
#include "include/interface/icontrol.h"
#include "include/interface/itextrenderbase.h"
#include "include/macro/msg.h"
#include "include/macro/xmldefine.h"
#include "include/util/rect.h"
#include "src/attribute/attribute.h"
#include "src/control/button/button_meta.h"

namespace ui {

const uint  BUTTON_BKGND_RENDER_STATE_NORMAL = RENDER_STATE_NORMAL | 0;
const uint  BUTTON_BKGND_RENDER_STATE_HOVER = RENDER_STATE_HOVER | 1;
const uint  BUTTON_BKGND_RENDER_STATE_PRESS = RENDER_STATE_PRESS | 2;
const uint  BUTTON_BKGND_RENDER_STATE_DISABLE = RENDER_STATE_DISABLE | 3;
const uint  BUTTON_BKGND_RENDER_STATE_DEFAULT = RENDER_STATE_DEFAULT | 4;
const uint  BUTTON_BKGND_RENDER_STATE_SELECTED_NORMAL = RENDER_STATE_NORMAL | RENDER_STATE_SELECTED | 4;
const uint  BUTTON_BKGND_RENDER_STATE_SELECTED_HOVER = RENDER_STATE_HOVER | RENDER_STATE_SELECTED | 5;
const uint  BUTTON_BKGND_RENDER_STATE_SELECTED_PRESS = RENDER_STATE_PRESS | RENDER_STATE_SELECTED | 6;
const uint  BUTTON_BKGND_RENDER_STATE_SELECTED_DISABLE = RENDER_STATE_DISABLE | RENDER_STATE_SELECTED | 7;

const uint  BUTTON_ICON_RENDER_STATE_NORMAL = RENDER_STATE_NORMAL | 0;
const uint  BUTTON_ICON_RENDER_STATE_HOVER = RENDER_STATE_HOVER | 1;
const uint  BUTTON_ICON_RENDER_STATE_PRESS = RENDER_STATE_PRESS | 2;
const uint  BUTTON_ICON_RENDER_STATE_DISABLE = RENDER_STATE_DISABLE | 3;
const uint  BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL = RENDER_STATE_NORMAL | RENDER_STATE_SELECTED | 4;
const uint  BUTTON_ICON_RENDER_STATE_SELECTED_HOVER = RENDER_STATE_HOVER | RENDER_STATE_SELECTED | 5;
const uint  BUTTON_ICON_RENDER_STATE_SELECTED_PRESS = RENDER_STATE_PRESS | RENDER_STATE_SELECTED | 6;
const uint  BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE = RENDER_STATE_DISABLE | RENDER_STATE_SELECTED | 7;


Button::Button(IButton *p) : Control(p), m_pIButton(p) {
  	memset(&m_button_style, 0, sizeof(ButtonStyle));
}

void Button::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_PAINT) {
    onPaint(static_cast<PaintMessage *>(msg)->rt);
    return;
  } else if (msg->message == UI_MSG_PAINTBKGND) {
    onPaintBkgnd(static_cast<PaintBkgndMessage *>(msg)->rt);
    return;
  } else if (msg->message == UI_MSG_STATECHANGED) {
    onStateChanged(static_cast<StateChangedMessage*>(msg));
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
    if (m->uuid == ButtonMeta::Get().UUID()) {
      *(m->pp) = m_pIButton;
      return;
    }
  } else if (msg->message == UI_MSG_FINALCONSTRUCT) {
    onFinalConstruct(static_cast<FinalConstructMessage *>(msg));
    return;
  } else if (msg->message == UI_MSG_LBUTTONDOWN) {
    onLButtonDown(static_cast<LButtonDownMessage*>(msg));
  } else if (msg->message == UI_MSG_LBUTTONUP) {
    onLButtonUp(static_cast<LButtonUpMessage*>(msg));
  }
  Control::onRouteMessage(msg);
}

void Button::onFinalConstruct(FinalConstructMessage *msg) {
  Control::onRouteMessage(msg);

  OBJSTYLE s = {0};
  s.default_transparent = 1;          // 默认透明
}


void  Button::ModifyButtonStyle(ButtonStyle* add, ButtonStyle* remove)
{
#define __REMOVE(x) \
    if (remove->x) m_button_style.x = 0;
#define __ADD(x) \
	if (add->x) m_button_style.x = 1;
    
	if (add)
	{
		__ADD(click_on_pushdown);
		__ADD(click_on_pushdown);
		__ADD(notify_hoverleave);
		__ADD(hand_cursor);
		__ADD(enable_radio_toggle);
	}

	if (remove)
	{
		__REMOVE(click_on_pushdown);
		__REMOVE(click_on_pushdown);
		__REMOVE(notify_hoverleave);
		__REMOVE(hand_cursor);
		__REMOVE(enable_radio_toggle); 
	}
}

bool Button::TestButtonStyle(const ButtonStyle& test)
{
#define __TEST(x) \
    if (test.x && !m_button_style.x) return false; 

	__TEST(click_on_pushdown);
	__TEST(click_on_pushdown);
	__TEST(notify_hoverleave);
	__TEST(hand_cursor);
	__TEST(enable_radio_toggle);

	return true;
}


const char *Button::GetText() { return m_text.c_str(); }
void Button::SetText(const char *text) {
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

void Button::onTextChanged() {
}

void Button::onClicked() {
  ButtonClickedEvent event;
  event.button = m_pIButton;
  emit(BUTTON_CLICK_EVENT, &event);
}

void Button::onLButtonDown(LButtonDownMessage *msg) {
  if (m_button_style.click_on_pushdown) {
    onClicked();
  }
}

void Button::onLButtonUp(LButtonUpMessage* msg) {
  if (m_button_style.click_on_pushdown)
    return;

  if (IsHover()) {
    onClicked();
  }
}

void Button::onStateChanged(StateChangedMessage *msg) {
  int mask = msg->state_changed_mask;
  if (mask & OSB_HOVER) {
    if (m_button_style.notify_hoverleave) {
      if (IsHover()) {
        ButtonHoverEvent event;
        event.button = m_pIButton;
        emit(BUTTON_HOVER_EVENT, &event);
      } else {
        ButtonLeaveEvent event;
        event.button = m_pIButton;
        emit(BUTTON_LEAVE_EVENT, &event);
      }
    }
  }
  Invalidate();
}

void Button::onPaint(IRenderTarget *r) {
  ButtonDrawContext c {
    .r = r
  };
  
  drawIcon(c);
  drawText(c);
  drawFocus(c);
}

int Button::getDrawState() {
  bool bDisable = !IsEnable();
  bool bHover = IsHover();
  bool bPress = IsPress();
  bool bForePress = IsForcePress();
  bool bChecked = IsChecked();
  bool bDefault = IsDefault();

  Rect rc = ui::Rect::MakeXYWH(0, 0, GetWidth(), GetHeight());
  if (bDisable) {
    return bChecked ? BUTTON_BKGND_RENDER_STATE_SELECTED_DISABLE
                    : BUTTON_BKGND_RENDER_STATE_DISABLE;
  } else if (bForePress || (bPress && bHover)) {
    return bChecked ? BUTTON_BKGND_RENDER_STATE_SELECTED_PRESS
                    : BUTTON_BKGND_RENDER_STATE_PRESS;
  } else if (bHover || bPress) {
    return bChecked ? BUTTON_BKGND_RENDER_STATE_SELECTED_HOVER
                    : BUTTON_BKGND_RENDER_STATE_HOVER;
  } else if (bDefault) {
    return BUTTON_BKGND_RENDER_STATE_DEFAULT;
  } else {
    return bChecked ? BUTTON_BKGND_RENDER_STATE_SELECTED_NORMAL
                    : BUTTON_BKGND_RENDER_STATE_NORMAL;
  }
}
void Button::onPaintBkgnd(IRenderTarget* r) {
  if (!m_back_render) {
    return;
  }
  
  Rect rc = ui::Rect::MakeXYWH(0, 0, GetWidth(), GetHeight());
  m_back_render->DrawState(r, &rc, getDrawState());
}

void Button::drawText(ButtonDrawContext& c) {
  if (m_text.empty()) {
    return;
  }
  Rect rect;
  GetClientRectWithZeroOffset(&rect);

  std::shared_ptr<ITextRenderBase> p = GetTextRenderOrDefault();
  if (p) {
    p->DrawState(c.r, &rect, getDrawState(), m_text.c_str());
  }
}
void Button::drawIcon(ButtonDrawContext& c) {

}
void Button::drawFocus(ButtonDrawContext& c) {

}

void Button::onSerialize(SerializeParam *pData) {
  Control::onSerialize(pData);

  AttributeSerializer s(pData, "Button");
  s.AddI18nString(XML_TEXT, m_text)->AsData();

  s.AddEnum(XML_BUTTON_AUTOSIZE_TYPE, *(int *)&m_auto_size_type)
      ->AddOption((int)eButtonAutoSizeType::NotDefine, "")
      ->AddOption((int)eButtonAutoSizeType::Content,
                  XML_BUTTON_AUTOSIZE_TYPE_CONTENT)
      ->AddOption((int)eButtonAutoSizeType::BackImage,
                  XML_BUTTON_AUTOSIZE_TYPE_BKIMAGE)
      ->AddOption((int)eButtonAutoSizeType::BackImageHeightAndContentWidth,
                  XML_BUTTON_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH)
      ->SetDefault((int)eButtonAutoSizeType::NotDefine);

  s.AddEnum(XML_BUTTON_ICON_ALIGN, *(int *)&m_icon_align)
      ->AddOption(AlignLeft, XML_ALIGN_LEFT)
      ->AddOption(AlignRight, XML_ALIGN_RIGHT)
      ->AddOption(AlignTop, XML_ALIGN_TOP)
      ->AddOption(AlignBottom, XML_ALIGN_BOTTOM)
      ->AddOption(AlignCenter, XML_ALIGN_CENTER)
      ->SetDefault(AlignLeft);

  s.AddInt(XML_BUTTON_ICON_TEXT_SPACE, m_icon_text_space);
}

void Button::onGetDesiredSize(Size *size) {
  size->width = size->height = 0;
  std::shared_ptr<ITextRenderBase> text_render = GetTextRenderOrDefault();

  eButtonAutoSizeType auto_size = m_auto_size_type;
  if (auto_size == eButtonAutoSizeType::NotDefine) {
    // hack 多语言支持编辑时，m_text包含两段数据
    if (m_text.length() && m_text[0] != 0)
      auto_size = eButtonAutoSizeType::Content;
    else
      auto_size = eButtonAutoSizeType::BackImage;
  }

  // 按钮的auto size可以按背景图标大小来算，也可以按内容的大小来算
  if (auto_size == eButtonAutoSizeType::BackImage) {
    if (m_back_render) {
      *size = m_back_render->GetDesiredSize();
    }
    return;
  }
  
  if (auto_size == eButtonAutoSizeType::Content) {
    REGION4 rcNonClient;
    GetNonClientRegion(&rcNonClient);

    size->width = rcNonClient.left + rcNonClient.right;
    size->height = rcNonClient.top + rcNonClient.bottom;

    Size sIconText = {0, 0};
    Size sizeIcon = {0, 0};
    Size sizeText = {0, 0};

    if (m_fore_render) {
      sizeIcon = m_fore_render->GetDesiredSize();
    }
    if (text_render) {
      sizeText = text_render->GetDesiredSize(m_text.c_str());
    }

    switch (m_icon_align) {
    case AlignLeft:
    case AlignRight: {
      sIconText.width = sizeIcon.width + sizeText.width + m_icon_text_space;
      sIconText.height = std::max(sizeIcon.height, sizeText.height);
    } break;
    case AlignTop:
    case AlignBottom: {
      sIconText.width = std::max(sizeIcon.width, sizeText.width);
      sIconText.height = sizeIcon.height + sizeText.height + m_icon_text_space;
    } break;
    }

    size->width += sIconText.width;
    size->height += sIconText.height;
    return;
  }
  
  if (auto_size == eButtonAutoSizeType::BackImageHeightAndContentWidth) {
    Size sHeight = {0, 0};
    if (m_back_render) {
      sHeight = m_back_render->GetDesiredSize();
    }

    Size sWidth;
    REGION4 rcNonClient;
    GetNonClientRegion(&rcNonClient);

    sWidth.width = rcNonClient.left + rcNonClient.right;
    sWidth.height = rcNonClient.top + rcNonClient.bottom;

    Size sIconText = {0, 0};
    {
      Size sizeIcon = {0, 0};
      Size sizeText = {0, 0};

      if (m_fore_render) {
        sizeIcon = m_fore_render->GetDesiredSize();
      }
      if (text_render) {
        sizeText = text_render->GetDesiredSize(m_text.c_str());
      }

      switch (m_icon_align) {
      case AlignLeft:
      case AlignRight: {
        sIconText.width = sizeIcon.width + sizeText.width + m_icon_text_space;
        sIconText.height = std::max(sizeIcon.height, sizeText.height);
      } break;
      case AlignTop:
      case AlignBottom: {
        sIconText.width = std::max(sizeIcon.width, sizeText.width);
        sIconText.height =
            sizeIcon.height + sizeText.height + m_icon_text_space;
      } break;
      }

      sWidth.width += sIconText.width;
      sWidth.height += sIconText.height;
    }

    size->width = sWidth.width;
    size->height = sHeight.height;
  }
}

} // namespace ui