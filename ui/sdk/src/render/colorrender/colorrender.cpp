#include "colorrender.h"
#include "include/inc.h"
#include "include/interface/iuires.h"
#include "include/interface/renderlibrary.h"
#include "src/attribute/attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/attribute/rect_attribute.h"
#include "src/attribute/string_attribute.h"
#include "src/object/object.h"
#include "src/render/render_meta.h"
#include "src/util/util.h"

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               ColorRender //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

namespace ui {

//////////////////////////////////////////////////////////////////////////

ColorRender::ColorRender(IColorRender *p) : RenderBase(p) {
  m_pIColorRender = p;
}
ColorRender::~ColorRender() {}

void ColorRender::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_RENDERBASE_DRAWSTATE) {
    DrawState(&((RenderBaseDrawStateMessage *)msg)->draw_state);
    return;
  } else if (msg->message == UI_MSG_SERIALIZE) {
    RenderBase::onRouteMessage(msg);
    OnSerialize(static_cast<SerializeMessage *>(msg)->param);
    return;
  } else if (msg->message == UI_MSG_QUERYINTERFACE) {
    auto *m = static_cast<QueryInterfaceMessage *>(msg);
    if (m->uuid == ColorRenderMeta::Get().UUID()) {
      *(m->pp) = m_pIColorRender;
      return;
    }
  }
  RenderBase::onRouteMessage(msg);
}

void ColorRender::SetBackColor(Color col) { 
  m_back_color.clear();
  m_back_color.push_back(col); 
}
void ColorRender::SetBorderColor(Color col) { 
  m_border_color.clear();
  m_border_color.push_back(col);
}

void ColorRender::SetBackColor(Color color, int index) {
  assert(index < 0xFFFF);
  if (index >= m_back_color.size()) {
    m_back_color.reserve(index+1);
  }
  m_back_color[index] = color;
}
void ColorRender::SetBorderColor(Color color, int index) {
  assert(index < 0xFFFF);
  if (index >= m_border_color.size()) {
    m_border_color.reserve(index+1);
  }
  m_border_color[index] = color;
}

void ColorRender::SetBorder(int b) {
  m_border = b;
}

void ColorRender::OnSerialize(SerializeParam *pData) {
  AttributeSerializer s(pData, "ColorRender");

  // s.AddColor(XML_RENDER_COLOR, m_back_color);
  // s.AddColor(XML_RENDER_BORDERCOLOR, m_border_color);
   s.AddString(XML_RENDER_COLOR,
      Slot(&ColorRender::LoadBackColor, this),
      Slot(&ColorRender::SaveBackColor, this));
  s.AddString(XML_RENDER_BORDERCOLOR,
      Slot(&ColorRender::LoadBorderColor, this),
      Slot(&ColorRender::SaveBorderColor, this));

  s.AddInt(XML_RENDER_BORDER_WIDTH, m_border);
  s.AddBool(XML_RENDER_BORDER_DASH, m_border_dash);
  s.AddRadius(XML_RENDER_RADIUS, m_radius);
}

void ColorRender::DrawState(RENDERBASE_DRAWSTATE *pDrawStruct) {
  IRenderTarget *r = pDrawStruct->pRenderTarget;
  int index = (pDrawStruct->nState) & 0xFFFF;

  Color back_color;
  Color border_color;

  Rect rect = pDrawStruct->rc;
  rect.Deflate(m_deflate_margin);

  if (!m_back_color.empty()) {
    if (index < m_back_color.size())
      back_color = m_back_color[index];
    else
      back_color = m_back_color[0];
  }
  if (!m_border_color.empty()) {
    if (index < m_border_color.size())
      border_color = m_border_color[index];
    else
      border_color = m_border_color[0];
  }

  if (m_radius.IsZero()) {
    if (!back_color.IsTransparnt()) {
      r->FillRect(rect, back_color);
    }
    if (!border_color.IsTransparnt() && m_border > 0) {
      r->StrokeRect(rect, border_color, m_border, m_border_dash);
    }
  } else {
    if (!back_color.IsTransparnt()) {
      r->FillRoundRect(rect, back_color, m_radius);
    }
    if (!border_color.IsTransparnt() && m_border > 0) {
      r->StrokeRoundRect(rect, border_color, m_radius, m_border, m_border_dash);
    }
  }
}

void ColorRender::LoadBackColor(const char *color) {
  loadColor(color, m_back_color);
}
const char *ColorRender::SaveBackColor() {
  return saveColor(m_back_color);
}
void ColorRender::LoadBorderColor(const char *color) {
  loadColor(color, m_border_color);
}
const char *ColorRender::SaveBorderColor() {
  return saveColor(m_border_color);
}

void ColorRender::loadColor(const char *text, std::vector<Color>& target) {
  if (!text)
    return;
  target.clear();

  std::vector<std::string> vColors;
  UI_Split(text, XML_MULTI_SEPARATOR, vColors);
  uint count = (uint)vColors.size();
  if (count == 0) {
    return;
  }
  for (uint i = 0; i < count; i++) {
    target.push_back(util::TranslateColor(vColors[i].c_str()));
  }
}

const char *ColorRender::saveColor(std::vector<Color>& target) {
  std::string &strBuffer = GetTempBufferString();

  char buffer[64] = {0};
  for (uint i = 0; i < target.size(); i++) {
    if (i > 0)
      strBuffer.push_back(XML_MULTI_SEPARATOR);

    target[i].ToWebString(buffer);
    strBuffer.append(buffer);
  }

  return strBuffer.c_str();
}

} // namespace ui