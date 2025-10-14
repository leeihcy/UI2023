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

void ColorRender::SetBkColor(Color col) { m_back_color = col; }
void ColorRender::SetBorderColor(Color col) { m_border_color = col; }
void ColorRender::SetBorder(int b) {
  m_border = b;
}

void ColorRender::OnSerialize(SerializeParam *pData) {
  AttributeSerializer s(pData, "ColorRender");
  s.AddColor(XML_RENDER_COLOR, m_back_color);
  s.AddColor(XML_RENDER_BORDERCOLOR, m_border_color);
  s.AddInt(XML_RENDER_BORDER_WIDTH, m_border);
  s.AddRadius(XML_RENDER_RADIUS, m_radius);
}

void ColorRender::DrawState(RENDERBASE_DRAWSTATE *pDrawStruct) {
  IRenderTarget *r = pDrawStruct->pRenderTarget;

  if (m_radius.IsZero()) {
    if (!m_back_color.IsTransparnt()) {
      r->FillRect(pDrawStruct->rc, m_back_color);
    }
    if (!m_border_color.IsTransparnt()) {
      r->StrokeRect(pDrawStruct->rc, m_border_color, m_border);
    }
  } else {
    if (!m_back_color.IsTransparnt()) {
      r->FillRoundRect(pDrawStruct->rc, m_back_color, m_radius);
    }
    if (!m_border_color.IsTransparnt()) {
      r->StrokeRoundRect(pDrawStruct->rc, m_border_color, m_radius, m_border);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               GradientRender //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

GradientRender::GradientRender(IGradientRender *p) : RenderBase(p) {
  m_pIGradientRender = p;
  m_pBorderColor = nullptr;
  m_rcBorder.SetEmpty();

  m_pColorFrom = nullptr;
  m_pColorTo = nullptr;
  m_nGradientDir = RENDER_COLOR_GRADIENT_DIR_H;
}
GradientRender::~GradientRender() {
  // SAFE_RELEASE(m_pColorFrom);
  // SAFE_RELEASE(m_pColorTo);
  //   SAFE_RELEASE(m_pBorderColor);
}

void GradientRender::OnSerialize(SerializeParam *pData) {
  // AttributeSerializer s(pData, "GradientRender");
  // s.AddString(XML_RENDER_COLORFROM,
  //     Slot(&GradientRender::LoadFromColor, this),
  //     Slot(&GradientRender::SaveFromColor, this));
  // s.AddString(XML_RENDER_COLORTO,
  //     Slot(&GradientRender::LoadToColor, this),
  //     Slot(&GradientRender::SaveToColor, this));
  // s.AddString(XML_RENDER_BORDERCOLOR,
  //     Slot(&GradientRender::LoadBorderColor, this),
  //     Slot(&GradientRender::SaveBorderColor, this));
  // s.AddRect(XML_RENDER_BORDER, m_rcBorder);
  // s.AddEnum(XML_RENDER_COLOR_GRADIENT_DIR, *(int*)&m_nGradientDir)
  //     ->AddOption(RENDER_COLOR_GRADIENT_DIR_H,
  //     XML_RENDER_COLOR_GRADIENT_DIR_H)
  //     ->AddOption(RENDER_COLOR_GRADIENT_DIR_V,
  //     XML_RENDER_COLOR_GRADIENT_DIR_V);
}

void GradientRender::DrawState(RENDERBASE_DRAWSTATE *pDrawStruct) {
  //   IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
  // if(nullptr == pRenderTarget || nullptr == m_pColorFrom || nullptr ==
  // m_pColorTo) 	return;

  // Rect rc = {0,0, this->m_pObject->GetWidth(), this->m_pObject->GetHeight()};

  // if (m_nGradientDir == RENDER_COLOR_GRADIENT_DIR_H)
  // {
  // 	pRenderTarget->GradientFillH(&pDrawStruct->rc, m_pColorFrom->m_col,
  // m_pColorTo->m_col);
  // }
  // else if (m_nGradientDir == RENDER_COLOR_GRADIENT_DIR_V)
  // {
  // 	pRenderTarget->GradientFillV(&pDrawStruct->rc, m_pColorFrom->m_col,
  // m_pColorTo->m_col);
  // }

  //   if (m_pBorderColor)
  //   {
  //       ColorBorderRender_DrawState(pDrawStruct, m_pObject,
  //       m_pBorderColor->m_col, m_rcBorder);
  //   }
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               ColorListRender //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

ColorListRender::ColorListRender(IColorListRender *p) : RenderBase(p) {
  m_pIColorListRender = p;
  m_nCount = 0;
}

ColorListRender::~ColorListRender() { this->Clear(); }
void ColorListRender::Clear() {
  m_vBkColor.clear();
  m_vBorderColor.clear();
}


void ColorListRender::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_RENDERBASE_DRAWSTATE) {
    DrawState(&((RenderBaseDrawStateMessage *)msg)->draw_state);
    return;
  } else if (msg->message == UI_MSG_SERIALIZE) {
    OnSerialize(static_cast<SerializeMessage *>(msg)->param);
    return;
  } else if (msg->message == UI_MSG_QUERYINTERFACE) {
    auto *m = static_cast<QueryInterfaceMessage *>(msg);
    if (m->uuid == ColorListRenderMeta::Get().UUID()) {
      *(m->pp) = m_pIColorListRender;
      return;
    }
  }
  RenderBase::onRouteMessage(msg);
}

void ColorListRender::SetStateColor(int nState, Color colorBk, bool bSetBk,
                                    Color colBorder, bool bSetBorder) {
  nState = (nState)&0xFFFF;
  if (m_nCount <= nState) {
    return;
  }

  if (bSetBk) {
    m_vBkColor[nState] = colorBk;
  }
  if (bSetBorder) {
    m_vBorderColor[nState] = colBorder;
  }
}

void ColorListRender::SetCount(int n) {
  if (n < 0)
  	return;

  this->Clear();

  for(int i = 0; i < n; i++ )
  {
  	m_vBkColor.reserve(n);
  	m_vBorderColor.reserve(n);
  }
  m_nCount = n;
}
int ColorListRender::GetCount() { return m_nCount; }

void ColorListRender::LoadBkColor(const char *szText) {
  if (!szText)
    return;

  ColorRes *pColorRes = GetSkinColorRes();
  if (nullptr == pColorRes)
    return;

  std::vector<std::string> vColors;
  UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
  int nCount = (int)vColors.size();

  if (0 == m_nCount) {
    //  如果未显示指定count，则自动取这里的大小
    this->SetCount(nCount);
  }

  for (int i = 0; i < m_nCount && i < nCount; i++) {
    if (!vColors[i].empty()) {
      m_vBkColor[i] = util::TranslateColor(vColors[i].c_str());
    }
  }
}
const char *ColorListRender::SaveBkColor() {
  std::string &strBuffer = GetTempBufferString();

  char buffer[64] = {0};
  for (int i = 0; i < m_nCount; i++) {
    if (i > 0)
      strBuffer.push_back(XML_MULTI_SEPARATOR);

    m_vBkColor[i].ToWebString(buffer);
    strBuffer.append(buffer);
  }

  return strBuffer.c_str();
}
void ColorListRender::LoadBorderColor(const char *szText) {
  ColorRes *pColorRes = GetSkinColorRes();
  if (nullptr == pColorRes)
    return;

  if (!szText)
    return;

  std::vector<std::string> vColors;
  UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
  int nCount = (int)vColors.size();

  if (0 == m_nCount) {
    //  如果未显示指定count，则自动取这里的大小
    this->SetCount(nCount);
  }

  for (int i = 0; i < m_nCount && i < nCount; i++) {
    if (!vColors[i].empty()) {
      m_vBorderColor[i] = util::TranslateColor(vColors[i].c_str());
    }
  }
}

const char *ColorListRender::SaveBorderColor() {
  bool bHasValue = false;
  std::string &strBuffer = GetTempBufferString();

  char buffer[64] = {0};
  for (int i = 0; i < m_nCount; i++) {
    if (i > 0)
      strBuffer.push_back(XML_MULTI_SEPARATOR);

    m_vBorderColor[i].ToWebString(buffer);
    bHasValue = true;
    strBuffer.append(buffer);
  }

  if (bHasValue)
    return strBuffer.c_str();
  return nullptr;
}

void ColorListRender::OnSerialize(SerializeParam *pData) {
  AttributeSerializer s(pData, "ColorListRender");

  s.AddInt(XML_RENDER_COLORLIST_COUNT,
      Slot(&ColorListRender::SetCount, this),
      Slot(&ColorListRender::GetCount, this));
  s.AddString(XML_RENDER_COLOR,
      Slot(&ColorListRender::LoadBkColor, this),
      Slot(&ColorListRender::SaveBkColor, this));
  s.AddString(XML_RENDER_BORDERCOLOR,
      Slot(&ColorListRender::LoadBorderColor, this),
      Slot(&ColorListRender::SaveBorderColor, this));

  s.AddInt(XML_RENDER_BORDER_WIDTH, m_border);
  s.AddRadius(XML_RENDER_RADIUS, m_radius);
}

void ColorListRender::DrawState(RENDERBASE_DRAWSTATE *pDrawStruct) {
  IRenderTarget *r = pDrawStruct->pRenderTarget;
  if (nullptr == r)
    return;

  if (0 == m_nCount)
    return;

  int nRealState = (pDrawStruct->nState) & 0xFFFF;
  if (nRealState >= m_nCount)
    nRealState = 0;

  Color back_color = m_vBkColor[nRealState];
  Color border_color = m_vBorderColor[nRealState];

  if (m_radius.IsZero()) {
    if (!back_color.IsTransparnt()) {
      r->FillRect(pDrawStruct->rc, back_color);
    }
    if (!border_color.IsTransparnt()) {
      r->StrokeRect(pDrawStruct->rc, border_color, m_border);
    }
  } else {
    if (!back_color.IsTransparnt()) {
      r->FillRoundRect(pDrawStruct->rc, back_color, m_radius);
    }
    if (!border_color.IsTransparnt()) {
      r->StrokeRoundRect(pDrawStruct->rc, border_color, m_radius, m_border);
    }
  }
}

} // namespace ui