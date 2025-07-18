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
void ColorBorderRender_DrawState(RENDERBASE_DRAWSTATE *pDrawStruct,
                                 Object *pObj, Color color,
                                 const Rect &rBorder) {
  IRenderTarget *pRenderTarget = pDrawStruct->pRenderTarget;
  if (nullptr == pRenderTarget)
    return;

    // 绘制边框，由于直接调用Rectangle创建指定宽度的PEN后进行绘制，会导致PEN的
    // 一半区域位于控件剪裁区域外面。因此在这里用FillRect来实现边框的绘制。
    // 同时也支持了当各个边框大小不一致时的绘制

#if 0
    if (nullptr == pObj)
        return;

    CRegion4 rBorder;
    pObj->GetBorderRegion(&rBorder);
#ifdef _DEBUG
    if (rBorder.left==0 && rBorder.right==0 && rBorder.top==0 && rBorder.bottom==0)
    {
        UI_LOG_WARN("%s 指定了border color，但没有配置border", FUNC_NAME);
    }
#endif
#endif

  Color c(color);
  c.a = 255;
  if (0 != rBorder.left) {
    Rect rcLeft = pDrawStruct->rc;
    rcLeft.right = rcLeft.left + rBorder.left;

    pRenderTarget->DrawRect(&rcLeft, &c);
  }
  if (0 != rBorder.top) {
    Rect rcTop = pDrawStruct->rc;
    rcTop.bottom = rcTop.top + rBorder.top;
    pRenderTarget->DrawRect(&rcTop, &c);
  }
  if (0 != rBorder.right) {
    Rect rcRight = pDrawStruct->rc;
    rcRight.left = rcRight.right - rBorder.right;
    pRenderTarget->DrawRect(&rcRight, &c);
  }
  if (0 != rBorder.bottom) {
    Rect rcBottom = pDrawStruct->rc;
    rcBottom.top = rcBottom.bottom - rBorder.bottom;
    pRenderTarget->DrawRect(&rcBottom, &c);
  }
}

//////////////////////////////////////////////////////////////////////////

ColorRender::ColorRender(IColorRender *p) : RenderBase(p) {
  m_pIColorRender = p;
  m_rcBorder.SetEmpty();
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
void ColorRender::SetBorderRegion(const Rect *prc) {
  if (nullptr == prc) {
    m_rcBorder.SetEmpty();
  } else {
    m_rcBorder.CopyFrom(*prc);
  }
}

void ColorRender::OnSerialize(SerializeParam *pData) {
  AttributeSerializer s(pData, "ColorRender");
  s.AddColor(XML_RENDER_COLOR, m_back_color);
  s.AddColor(XML_RENDER_BORDERCOLOR, m_border_color);
  s.AddRect(XML_RENDER_BORDER, m_rcBorder);
}

void ColorRender::DrawState(RENDERBASE_DRAWSTATE *pDrawStruct) {
  IRenderTarget *pRenderTarget = pDrawStruct->pRenderTarget;
  if (nullptr == pRenderTarget)
    return;

  if (!m_back_color.IsTransparnt()) {
    pRenderTarget->DrawRect(&pDrawStruct->rc, &m_back_color);
  }

  if (!m_border_color.IsTransparnt()) {
    ColorBorderRender_DrawState(pDrawStruct, m_pObject, m_border_color,
                                m_rcBorder);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               SysColorRender //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

SysColorRender::SysColorRender(ISysColorRender *p) : RenderBase(p) {
  m_pISysColorRender = p;

  m_nBkColorIndex = -1;
  m_nBorderColorIndex = -1;
  m_rcBorder.SetEmpty();
}

SysColorRender::~SysColorRender() {}

void SysColorRender::SetBkColor(int nColorIndex) {
  m_nBkColorIndex = nColorIndex;
  // 	if (m_nBkColorIndex != -1)
  // 	{
  // 		m_bkColor = ::GetSysColor(m_nBkColorIndex);
  // 	}
}
void SysColorRender::SetBorderColor(int nColorIndex) {
  m_nBorderColorIndex = nColorIndex;
  // 	if (m_nBorderColorIndex != -1)
  // 	{
  // 		m_borderColor = ::GetSysColor(m_nBorderColorIndex);
  // 	}
}

// void SysColorRender::OnThemeChanged()
// {
// 	UI_LOG_DEBUG("%s,1. %d", FUNC_NAME, m_bkColor);
// 	if (m_nBkColorIndex != -1)
// 	{
// 		UI_LOG_DEBUG("XXXX");
// 		Sleep(100); // 1250, OK, 100, 50 10 BAD
// 		m_bkColor = ::GetSysColor(m_nBkColorIndex);
// 	}
// 	if (m_nBorderColorIndex != -1)
// 	{
// 		UI_LOG_DEBUG("DDDDD");
// 		m_borderColor = ::GetSysColor(m_nBorderColorIndex);
// 	}
// 	UI_LOG_DEBUG("%s,2. %d", FUNC_NAME, m_bkColor);
// }

void SysColorRender::OnSerialize(SerializeParam *pData) {
  AttributeSerializer s(pData, "SysColorRender");
  s.AddInt(XML_RENDER_COLOR, m_nBkColorIndex)->SetDefault(-1);
  s.AddInt(XML_RENDER_BORDERCOLOR, m_nBorderColorIndex)->SetDefault(-1);
  s.AddRect(XML_RENDER_BORDER, m_rcBorder);
}

void SysColorRender::DrawState(RENDERBASE_DRAWSTATE *pDrawStruct) {
#if 0 // defined(OS_WIN)
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
	if (nullptr == pRenderTarget)
		return ;

	if (-1 != m_nBkColorIndex)
    {
        Color c(GetSysColor(m_nBkColorIndex));
        c.a = 255;

		pRenderTarget->DrawRect(&pDrawStruct->rc, &c);
    }

	// 绘制边框，由于直接调用Rectangle创建指定宽度的PEN后进行绘制，会导致PEN的
	// 一半区域位于控件剪裁区域外面。因此在这里用FillRect来实现边框的绘制。
	// 同时也支持了当各个边框大小不一致时的绘制

	if (-1 == m_nBorderColorIndex)
		return;

	Color colorborder = GetSysColor(m_nBorderColorIndex)|0xff000000;
    ColorBorderRender_DrawState(pDrawStruct, m_pObject, colorborder, &m_rcBorder);

// 	CRegion4 rBorder;
// 	m_pObject->GetBorderRegion(&rBorder);
// 	if (0 != rBorder.left)
// 	{
// 		Rect rcLeft = *prc;
// 		rcLeft.right = rBorder.left;
// 		pRenderTarget->FillRect(&rcLeft, colorborder);
// 	}
// 	if (0 != rBorder.top)
// 	{
// 		Rect rcTop = *prc;
// 		rcTop.bottom = rBorder.top;
// 		pRenderTarget->FillRect(&rcTop, colorborder);
// 	}
// 	if (0 != rBorder.right)
// 	{
// 		Rect rcRight = *prc;
// 		rcRight.left = rcRight.right-rBorder.right;
// 		pRenderTarget->FillRect(&rcRight, colorborder);
// 	}
// 	if (0 != rBorder.bottom)
// 	{
// 		Rect rcBottom = *prc;
// 		rcBottom.top = rcBottom.bottom - rBorder.bottom;
// 		pRenderTarget->FillRect(&rcBottom, colorborder);
// 	}
#endif
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