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
  // for (int i = 0; i < m_nCount; i++)
  // {
  // 	SAFE_RELEASE(m_vBkColor[i]);
  // 	SAFE_RELEASE(m_vBorderColor[i]);
  // }
  // m_vBkColor.clear();
  // m_vBorderColor.clear();
}

void ColorListRender::SetStateColor(int nState, Color colorBk, bool bSetBk,
                                    Color colBorder, bool bSetBorder) {
  // nState = (nState) & 0xFFFF;
  // if (m_nCount <= nState)
  // {
  // 	return;
  // }

  // if (bSetBk)
  // {
  // 	SAFE_RELEASE(m_vBkColor[nState]);
  //       m_vBkColor[nState] = Color::CreateInstance(colorBk);
  // }
  // if (bSetBorder)
  // {
  // 	SAFE_RELEASE(m_vBorderColor[nState]);
  //       m_vBorderColor[nState] = Color::CreateInstance(colBorder);
  // }
}

void ColorListRender::SetCount(int n) {
  // if (n < 0)
  // 	return;

  // this->Clear();

  // for(int i = 0; i < n; i++ )
  // {
  // 	m_vBkColor.push_back((Color*)nullptr);
  // 	m_vBorderColor.push_back((Color*)nullptr);
  // }
  // m_nCount = n;
}
int ColorListRender::GetCount() { return m_nCount; }

void ColorListRender::LoadBkColor(const char *szText) {
  // if (!szText)
  //     return;

  // ColorRes* pColorRes = GetSkinColorRes();
  // if (nullptr == pColorRes)
  //     return;

  // std::vector<std::string> vColors;
  // UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
  // int nCount = (int)vColors.size();

  // if (0 == m_nCount)
  //     this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小

  // for (int i = 0; i < m_nCount && i < nCount; i++ )
  // {
  //     if (!vColors[i].empty())
  //     {
  //         pColorRes->GetColor(vColors[i].c_str(), &m_vBkColor[i]);
  //     }
  // }
}
const char *ColorListRender::SaveBkColor() {
  // std::string&  strBuffer = GetTempBufferString();
  // for (int i = 0; i < m_nCount; i++)
  // {
  //     if (i > 0)
  //         strBuffer.push_back(XML_MULTI_SEPARATOR);

  //     const char* szTemp = _GetColorId(m_vBkColor[i]);
  //     if (szTemp)
  //         strBuffer.append(szTemp);
  // }

  // return strBuffer.c_str();
  return nullptr;
}
void ColorListRender::LoadBorderColor(const char *szText) {
  // ColorRes* pColorRes = GetSkinColorRes();
  // if (nullptr == pColorRes)
  //     return;

  // if (!szText)
  //     return;

  // std::vector<std::string> vColors;
  // UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
  // int nCount = (int)vColors.size();

  // if (0 == m_nCount)
  //     this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小

  // for (int i = 0; i < m_nCount && i < nCount; i++ )
  // {
  //     if (!vColors[i].empty())
  //     {
  //         pColorRes->GetColor(vColors[i].c_str(), &m_vBorderColor[i]);
  //     }
  // }
}

const char *ColorListRender::SaveBorderColor() {
  bool bHasValue = false;
  std::string &strBuffer = GetTempBufferString();
  for (int i = 0; i < m_nCount; i++) {
    if (i > 0)
      strBuffer.push_back(XML_MULTI_SEPARATOR);

    const char *szTemp = _GetColorId(m_vBorderColor[i]);
    if (szTemp && szTemp[0]) {
      bHasValue = true;
      strBuffer.append(szTemp);
    }
  }

  if (bHasValue)
    return strBuffer.c_str();
  return nullptr;
}

void ColorListRender::OnSerialize(SerializeParam *pData) {
  // AttributeSerializer s(pData, "ColorListRender");

  // s.AddInt(XML_RENDER_COLORLIST_COUNT,
  //     Slot(&ColorListRender::SetCount, this),
  //     Slot(&ColorListRender::GetCount, this));
  // s.AddString(XML_RENDER_COLOR,
  //     Slot(&ColorListRender::LoadBkColor, this),
  //     Slot(&ColorListRender::SaveBkColor, this));
  // s.AddString(XML_RENDER_BORDERCOLOR,
  //     Slot(&ColorListRender::LoadBorderColor, this),
  //     Slot(&ColorListRender::SaveBorderColor, this));
}

void ColorListRender::DrawState(RENDERBASE_DRAWSTATE *pDrawStruct) {
  //   IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
  // if (nullptr == pRenderTarget)
  // 	return;

  //   if (0 == m_nCount)
  //       return;

  // int nRealState = (pDrawStruct->nState) & 0xFFFF;
  // if (nRealState >= m_nCount)
  // 	nRealState = 0;

  // if (nullptr == m_vBorderColor[nRealState])   // 不绘制边框
  // {
  // 	if (m_vBkColor[nRealState])
  // 	{
  // 		pRenderTarget->DrawRect(&pDrawStruct->rc,
  // m_vBkColor[nRealState]);
  // 	}
  // }
  // else                           // 绘制边框
  // {
  // 	if (m_vBkColor[nRealState])
  // 		pRenderTarget->Rectangle(&pDrawStruct->rc,
  // m_vBorderColor[nRealState], m_vBkColor[nRealState], 1, false); 	else
  // 		pRenderTarget->Rectangle(&pDrawStruct->rc,
  // m_vBorderColor[nRealState], 0,1,true);
  // }
}

} // namespace ui