#include "textrender.h"

#include "include/interface/iattributemap.h"
#include "include/interface/iuires.h"
#include "include/interface/graphics.h"
#include "include/macro/msg.h"
#include "include/macro/uidefine.h"
#include "include/macro/xmldefine.h"
#include "src/attribute/attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/attribute/flags_attribute.h"
#include "src/resource/font.h"
#include "src/object/object.h"
#include "src/render/render_meta.h"
#include "src/render/renderbase.h"
#include "src/resource/colorres.h"
#include "src/resource/res_bundle.h"
#include "src/util/util.h"
#include <cassert>

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                                  TextRender //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

namespace ui {
#define DEFAULT_DRAWTEXT_FLAG                                                  \
  (DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_NOPREFIX)
TextRenderBase::TextRenderBase(ITextRenderBase *p) : Message(p) {
  m_pITextRenderBase = p;
  m_pObject = nullptr;
  m_nTextRenderType = TEXTRENDER_TYPE_NULL;
  // m_nDrawTextFlag = DEFAULT_DRAWTEXT_FLAG;
  // m_eDrawTextEffect = TEXT_EFFECT_NONE;
  // m_pColorTextBkgnd = nullptr;
  // m_wparamDrawText = m_lparamDrawText = 0;
}
TextRenderBase::~TextRenderBase() {
  // SAFE_RELEASE(m_pColorTextBkgnd);
}

void TextRenderBase::SetTextAlignment(int nDrawFlag) {
  m_nDrawTextFlag = nDrawFlag;
}

void TextRenderBase::Serialize(AttributeSerializer *ps) {
#if 0
  ps->AddFlags(XML_TEXTRENDER_ALIGN, m_nDrawTextFlag)
      // 		->AddFlag(DT_TOP, XML_TEXTRENDER_ALIGN_TOP)   //
      // 0，不放属性里了
      // 		->AddFlag(DT_LEFT, XML_TEXTRENDER_ALIGN_LEFT)
      ->AddFlag(DT_RIGHT, XML_TEXTRENDER_ALIGN_RIGHT)
      ->AddFlag(DT_CENTER, XML_TEXTRENDER_ALIGN_CENTER)
      ->AddFlag(DT_BOTTOM, XML_TEXTRENDER_ALIGN_BOTTOM)
      ->AddFlag(DT_VCENTER, XML_TEXTRENDER_ALIGN_VCENTER)
      ->AddFlag(DT_SINGLELINE, XML_TEXTRENDER_ALIGN_SINGLELINE)
      ->AddFlag(DT_WORDBREAK | DT_EDITCONTROL, XML_TEXTRENDER_ALIGN_MULTILINE)
      ->AddFlag(DT_END_ELLIPSIS, XML_TEXTRENDER_ALIGN_END_ELLIPSIS)
      ->AddFlag(DT_NOPREFIX, XML_TEXTRENDER_ALIGN_NO_PREFIX)
      ->SetDefault(DEFAULT_DRAWTEXT_FLAG);

  ps->AddEnum(XML_TEXTRENDER_EFFECT, *(int *)&m_eDrawTextEffect)
      ->AddOption(TEXT_EFFECT_NONE, XML_TEXTRENDER_EFFECT_NONE)
      ->AddOption(TEXT_EFFECT_HALO, XML_TEXTRENDER_EFFECT_HALO)
      ->AddOption(TEXT_EFFECT_ENDALPHAMASK, XML_TEXTRENDER_EFFECT_ENDALPHAMASK);

  ps->AddString(XML_TEXTRENDER_EFFECT_HALO_SHADOW_COLOR,
                Slot(&TextRenderBase::LoadHaloColor, this),
                Slot(&TextRenderBase::GetHaloColorId, this));
#endif
}

// void TextRenderBase::LoadHaloColor(const char *szColorId) {
//   // _LoadColor(szColorId, m_pColorTextBkgnd);
//   m_wparamDrawText = 3; // 默认模糊半径
// }
// const char *TextRenderBase::GetHaloColorId() {
//   return _SaveColor(m_pColorTextBkgnd);
// }
#if 0
std::shared_ptr<IRenderFont> TextRenderBase::_LoadFont(const char *szFontId) {

    SAFE_RELEASE(pRenderFont);
    IFontRes*  pFontRes = GetSkinFontRes();
    if (!szFontId || !pFontRes)
        return;

	eGraphicsLibraryType eType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
	WindowBase* pWnd = m_pObject->GetWindowObject();
	if (pWnd)
	{
		WindowRender* pRender = pWnd->GetWindowRender();
		if (pRender)
			eType = pRender->GetGraphicsRenderType();
	}

	pFontRes->GetFont(szFontId, eType, &pRenderFont);
  return std::shared_ptr<IRenderFont>();
}
#endif
// const char *TextRenderBase::_SaveFont(IRenderFont *pRenderFont) {
//   if (!pRenderFont)
//     return nullptr;

//   IFontRes *pFontRes = GetSkinFontRes();
//   if (pFontRes) {
//     const char *szId = pFontRes->GetRenderFontId(pRenderFont);
//     if (szId)
//       return szId;
//   }
//   return nullptr;
// }

// std::shared_ptr<IRenderFont> TextRenderBase::_LoadDefalutFont() {
// #if 0
// 	WindowBase* pWnd = m_pObject->GetWindowObject();
// 	if (!pWnd)
// 		return;

// 	IRenderFont* pFont = pWnd->GetWindowDefaultRenderFont();
//     if (pFont)
//     {
// 		if (ppRenderFont)
// 		{
// 			*ppRenderFont = pFont;
// 			(*ppRenderFont)->AddRef();
// 		}
//     }
//     else
//     {
//         IFontRes* pFontRes = GetSkinFontRes();

//         // 可能是没有窗口对象，比如是一个 popup
//         listbox或者menu，窗口还没有创建。获取默认字体
// 		if (pFontRes && m_pObject)
// 		{
// 			eGraphicsLibraryType eType =
// GRAPHICS_RENDER_LIBRARY_TYPE_GDI; 			WindowBase* pWnd =
// m_pObject->GetWindowObject(); 			if (pWnd)
// 			{
// 				WindowRender* pRender = pWnd->GetWindowRender();
// 				if (pRender)
// 					eType =
// pRender->GetGraphicsRenderType();
// 			}

// 			pFontRes->GetDefaultFont(eType, ppRenderFont);
// 		}
//     }
// #else
//   UIASSERT(0);
// #endif
//   return std::shared_ptr<IRenderFont>();
// }

void TextRenderBase::_LoadColor(const char *szColorId, Color *&pColor) {
  // SAFE_RELEASE(pColor);
  // if (!szColorId)
  //   return;

  // IColorRes *pColorRes = GetSkinColorRes();
  // if (!pColorRes)
  //   return;

  // pColorRes->GetColor(szColorId, &pColor);
}
const char *TextRenderBase::_SaveColor(Color *&pColor) {
  // if (!pColor)
  //   return nullptr;

  // IColorRes *pColorRes = GetSkinColorRes();
  // if (pColorRes) {
  //   const char *szId = pColorRes->GetColorId(pColor);
  //   if (szId)
  //     return szId;
  // }

  // char *szBuffer = GetTempBuffer();
  // pColor->ToWebString(szBuffer);
  // return szBuffer;
  return nullptr;
}

void TextRenderBase::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_FINALCONSTRUCT) {
    Message::onRouteMessage(msg);
    m_resouce = static_cast<FinalConstructMessage *>(msg)->resource->GetImpl();
    return;
  } 
  Message::onRouteMessage(msg);
}

Size TextRenderBase::GetDesiredSize(const char *text, unsigned int limit_width) {
  GetTextDesiredSizeMessage msg;
  msg.text = text;
  msg.limit_width = limit_width;
  RouteMessage(&msg);
  return msg.size;
}

IColorRes *TextRenderBase::GetSkinColorRes() {
  ResourceBundle *resource_bundle = GetResource();
  if (!resource_bundle)
    return nullptr;

  return &resource_bundle->GetColorRes().GetIColorRes();
}

ResourceBundle *TextRenderBase::GetResource() {
  if (m_pObject) {
    return m_pObject->GetResource();
  }

  return nullptr;
}

// IFontRes *TextRenderBase::GetSkinFontRes() {
//   ResourceBundle *resource_bundle = GetResource();
//   if (!resource_bundle)
//     return nullptr;

//   return &resource_bundle->GetFontRes().GetIFontRes();
// }

bool TextRenderBase::IsThemeRender() {
  return m_nTextRenderType > TEXTRENDER_TYPE_THEME_FIRST &&
         m_nTextRenderType < TEXTRENDER_TYPE_THEME_LAST;
}

void TextRenderBase::CheckSkinTextureChanged() {
  if (m_nTextRenderType > TEXTRENDER_TYPE_CONTRAST_FIRST &&
      m_nTextRenderType < TEXTRENDER_TYPE_CONTRAST_LAST) {
    assert(false);
#if 0 // 废弃，使用RouteMessage代替。
    static_cast<IMessage *>(m_pITextRenderBase)
        ->SendMessage(UI_MSG_SKINTEXTURECHANGED);
#endif
  }
}

// helper function
void SerializeFont(AttributeSerializer &serialize, FontDesc &font_desc) {
  serialize
      .AddString(XML_TEXTRENDER_FONT "." XML_FONT_FACENAME,
                 font_desc.face)
#if defined(OS_WIN)
      ->SetDefault("Microsoft YaHei")
#elif defined (OS_LINUX)
      ->SetDefault("Noto Sans CJK SC")
#elif defined(OS_MAC)
      ->SetDefault("PingFang SC") // monospace, Songti
#endif
      ;
  serialize
      .AddInt(XML_TEXTRENDER_FONT "." XML_FONT_SIZE, font_desc.size)
      ->SetDefault(14);

  serialize
      .AddEnum(XML_TEXTRENDER_FONT "." XML_FONT_WEIGHT,
               font_desc.weight)
      ->AddOption(SkFontStyle::kInvisible_Weight, "invisible")
      ->AddOption(SkFontStyle::kThin_Weight, "thin")
      ->AddOption(SkFontStyle::kExtraLight_Weight, "extralight")
      ->AddOption(SkFontStyle::kLight_Weight, "light")
      ->AddOption(SkFontStyle::kNormal_Weight, "normal")
      ->AddOption(SkFontStyle::kMedium_Weight, "medium")
      ->AddOption(SkFontStyle::kSemiBold_Weight, "semibold")
      ->AddOption(SkFontStyle::kBold_Weight, "bold")
      ->AddOption(SkFontStyle::kExtraBold_Weight, "extrabold")
      ->AddOption(SkFontStyle::kBlack_Weight, "black")
      ->AddOption(SkFontStyle::kExtraBlack_Weight, "extrablack")
      ->SetDefault(SkFontStyle::kNormal_Weight);

  // serialize.AddBool((attr_prefix + XML_FONT_ITALIC).c_str(),
  // font_desc.italic);
  
}
void SerializeDrawTextParam(AttributeSerializer &serialize,
                            DrawTextParam &draw_ext_param) {
  serialize.AddFlags(XML_TEXTRENDER_ALIGN, draw_ext_param.align)
      ->AddFlag(AlignRight, XML_TEXTRENDER_ALIGN_RIGHT)
      ->AddFlag(AlignCenter, XML_TEXTRENDER_ALIGN_CENTER)
      ->AddFlag(AlignBottom, XML_TEXTRENDER_ALIGN_BOTTOM)
      ->AddFlag(AlignVCenter, XML_TEXTRENDER_ALIGN_VCENTER)
      // ->AddFlag(DT_SINGLELINE, XML_TEXTRENDER_ALIGN_SINGLELINE)
      // ->AddFlag(DT_WORDBREAK | DT_EDITCONTROL, XML_TEXTRENDER_ALIGN_MULTILINE)
      // ->AddFlag(DT_END_ELLIPSIS, XML_TEXTRENDER_ALIGN_END_ELLIPSIS)
      // ->AddFlag(DT_NOPREFIX, XML_TEXTRENDER_ALIGN_NO_PREFIX)
      ;


  SerializeFont(serialize, draw_ext_param.font_desc);
}

SimpleTextRender::SimpleTextRender(ISimpleTextRender *p) : TextRenderBase(p) {
  m_pISimpleTextRender = p;
}
SimpleTextRender::~SimpleTextRender() {}

void SimpleTextRender::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_GETTEXTDESIREDSIZE) {
    onGetDesiredSize(static_cast<GetTextDesiredSizeMessage *>(msg));
  } else if (msg->message == UI_MSG_FINALCONSTRUCT) {
    TextRenderBase::onRouteMessage(msg);
    m_draw_text_param.resource = m_resouce->GetIResource();
    return;
  }
  else if (msg->message == UI_MSG_RENDERBASE_DRAWSTATE) {
    DrawState(&((TextRenderDrawStateMessage *)msg)->draw_state);
    return;
  } else if (msg->message == UI_MSG_SERIALIZE) {
    OnSerialize(static_cast<SerializeMessage *>(msg)->param);
    return;
  }
  TextRenderBase::onRouteMessage(msg);
}

void SimpleTextRender::onGetDesiredSize(GetTextDesiredSizeMessage *msg) {
  assert(msg->limit_width == 0); // TODO:

  msg->size = m_resouce->GetFontRes().MeasureString(m_draw_text_param.font_desc,
                                                    msg->text);
}

// 如果字体指针为空，则取object对象的配置字体。
void SimpleTextRender::OnSerialize(SerializeParam *pData) {
  AttributeSerializer s(pData, "SimpleTextRender");
  TextRenderBase::Serialize(&s);

  // s.AddColor(XML_TEXTRENDER_COLOR, m_draw_text_param.color);
  s.AddString(XML_TEXTRENDER_COLOR, Slot(&SimpleTextRender::LoadColor, this),
              Slot(&SimpleTextRender::GetColor, this));

  SerializeDrawTextParam(s, m_draw_text_param);
}


void SimpleTextRender::LoadColor(const char *szText) {
  m_vTextColor.clear();

  if (!szText || !szText[0]) {
    m_vTextColor.push_back(Color::black());
    return;
  }

  std::vector<std::string> vColors;
  UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
  int nCount = (int)vColors.size();

  for (int i = 0; i < nCount; i++) {
    if (!vColors[i].empty()) {
      m_vTextColor.push_back(util::TranslateColor(vColors[i].c_str()));
    }
  }
}
const char *SimpleTextRender::GetColor() {
  std::string &strBuffer = GetTempBufferString();
  char buffer[64] = {0};
  for (int i = 0; i < m_vTextColor.size(); i++) {
    if (i > 0)
      strBuffer.push_back(XML_MULTI_SEPARATOR);

    m_vTextColor[i].ToWebString(buffer);
    strBuffer.append(buffer);
  }

  return strBuffer.c_str();
}

void SimpleTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE *pDrawStruct) {
  IRenderTarget *pRenderTarget = pDrawStruct->ds_renderbase.pRenderTarget;
  if (nullptr == pRenderTarget || nullptr == pDrawStruct->szText)
    return;

  int nRealState = (pDrawStruct->ds_renderbase.nState) & 0xFFFF;
  Color text_color;
  if (!m_vTextColor.empty()) {
    if (nRealState < m_vTextColor.size()) {
      text_color = m_vTextColor[nRealState];
    } else {
      text_color = m_vTextColor[0];
    }
  }
  m_draw_text_param.color = text_color;

  if (strlen(pDrawStruct->szText) > 0) {
    // param.nFormatFlag = pDrawStruct->nDrawTextFlag == -1
    //                         ? m_nDrawTextFlag
    //                         : pDrawStruct->nDrawTextFlag;
    m_draw_text_param.bound = pDrawStruct->ds_renderbase.rc;
    m_draw_text_param.text = pDrawStruct->szText;

    // param.nEffectFlag = m_eDrawTextEffect;
    // if (m_pColorTextBkgnd)
    //   param.bkcolor = *m_pColorTextBkgnd;
    // param.wParam = m_wparamDrawText;
    // param.lParam = m_lparamDrawText;
    pRenderTarget->DrawString(m_draw_text_param);
  }
}

//////////////////////////////////////////////////////////////////////////
#if 0
ContrastColorTextRender::ContrastColorTextRender(IContrastColorTextRender *p)
    : TextRenderBase(p) {
  m_pIContrastColorTextRender = p;

  m_pColorText = nullptr;
  m_bNeedCalcColor = false;
}
ContrastColorTextRender::~ContrastColorTextRender() {
  // SAFE_RELEASE(m_pColorText);
}

// 如果字体指针为空，则取object对象的配置字体。
void ContrastColorTextRender::OnSerialize(SerializeParam *pData) {
  {
    AttributeSerializer s(pData, "ContrastColorTextRender");
    TextRenderBase::Serialize(&s);

    s.AddString(XML_TEXTRENDER_FONT,
                Slot(&ContrastColorTextRender::LoadFont, this),
                Slot(&ContrastColorTextRender::GetFontId, this));
    s.AddString(XML_TEXTRENDER_COLOR,
                Slot(&ContrastColorTextRender::LoadColor, this),
                Slot(&ContrastColorTextRender::GetColorId, this));
  }

  if (pData->IsLoad()) {
    // if (!m_pRenderFont) {
    //   m_pRenderFont = _LoadDefalutFont();
    // }

    // 如果没有配置背景色，则默认设置一个
    // if (m_eDrawTextEffect == TEXT_EFFECT_HALO) {
    //   if (m_pColorText && !m_pColorTextBkgnd) {
    //     SAFE_RELEASE(m_pColorTextBkgnd);
    //     if (m_pColorText->m_col == 0xFFFFFFFF) {
    //       m_pColorTextBkgnd = Color::CreateInstance(0xFF000000);
    //     } else {
    //       m_pColorTextBkgnd = Color::CreateInstance(0xFFFFFFFF);
    //     }
    //   }
    //   m_wparamDrawText = 3; // 默认模糊半径
    // }
  }
}

void ContrastColorTextRender::SetRenderFont(
    std::shared_ptr<IRenderFont> pFont) {
  m_pRenderFont = pFont;
}

void ContrastColorTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE *pDrawStruct) {
  UIASSERT(0);
#if 0
    if (nullptr == m_pObject)
        return;

    if (nullptr == m_pColorText)
    {
        unsigned int dwColor = m_pObject->CalcContrastTextColor();
        m_pColorText = Color::CreateInstance(dwColor);

        if (m_eDrawTextEffect == TEXT_EFFECT_HALO)
        {
            SAFE_RELEASE(m_pColorTextBkgnd);
            if (dwColor == 0xFFFFFFFF)
            {
                m_pColorTextBkgnd = Color::CreateInstance(0xFF000000);
            }
            else
            {
                m_pColorTextBkgnd = Color::CreateInstance(0xFFFFFFFF);
            }
        }
    }

    IRenderTarget* pRenderTarget = pDrawStruct->ds_renderbase.pRenderTarget;
    if (nullptr == pRenderTarget || nullptr == pDrawStruct->szText)
        return ;

    if (m_pRenderFont && _tcslen(pDrawStruct->szText) > 0)
    {
        DRAWTEXTPARAM  param;
        param.szText = pDrawStruct->szText;
        param.prc = &pDrawStruct->ds_renderbase.rc;
        param.color = m_pColorText->m_col;
        param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;

        param.nEffectFlag = m_eDrawTextEffect;
        if (m_pColorTextBkgnd)
            param.bkcolor = *m_pColorTextBkgnd;
        param.wParam = m_wparamDrawText;
        param.lParam = m_lparamDrawText;
        pRenderTarget->DrawString(m_pRenderFont, &param);
    }
#endif
}

int ContrastColorTextRender::OnSkinTextureChanged(unsigned int, int, int) {
  // SAFE_RELEASE(m_pColorText);
  return 0;
}

//////////////////////////////////////////////////////////////////////////

ContrastColorListTextRender::ContrastColorListTextRender(
    IContrastColorListTextRender *p)
    : TextRenderBase(p) {
  m_nCount = 0;
  m_pIContrastColorListTextRender = p;
}
ContrastColorListTextRender::~ContrastColorListTextRender() { this->Clear(); }
void ContrastColorListTextRender::Clear() {
  // for (int i = 0; i < m_nCount; i++) {
  //   SAFE_RELEASE(m_vTextColor[i]);
  // }

  m_vTextColor.clear();
  m_nCount = 0;
}

int ContrastColorListTextRender::OnSkinTextureChanged(unsigned int, int, int) {
  // for (int i = 0; i < m_nCount; i++) {
  //   SAFE_RELEASE(m_vTextColor[i]);
  //   m_vTextColor[i] = nullptr;
  // }

  return 0;
}
void ContrastColorListTextRender::OnSerialize(SerializeParam *pData) {
  {
    AttributeSerializer s(pData, "ContrastColorListTextRender");
    TextRenderBase::Serialize(&s);

    s.AddInt(XML_TEXTRENDER_COLORLIST_COUNT,
             Slot(&ContrastColorListTextRender::SetCount, this),
             Slot(&ContrastColorListTextRender::GetCount, this));

    // s.AddString(XML_TEXTRENDER_FONT,
    //             Slot(&ContrastColorListTextRender::LoadFont, this),
    //             Slot(&ContrastColorListTextRender::GetFontId, this));
  }

  // if (pData->IsLoad() && !m_pRenderFont) {
  //   m_pRenderFont = _LoadDefalutFont();
  // }
}

void ContrastColorListTextRender::DrawState(
    TEXTRENDERBASE_DRAWSTATE *pDrawStruct) {
  UIASSERT(0);
#if 0
    if (0 == m_nCount || nullptr == m_pObject)
        return;

    int nRealState = LOWORD(pDrawStruct->ds_renderbase.nState);
    if (nRealState >= m_nCount)
        nRealState = 0;


    if (nullptr  == m_vTextColor[nRealState])
        m_vTextColor[nRealState] = Color::CreateInstance(m_pObject->CalcContrastTextColor());

    DRAWTEXTPARAM  param;

    param.prc = &pDrawStruct->ds_renderbase.rc;
    param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;
    if (m_vTextColor[nRealState])
        param.color = m_vTextColor[nRealState]->m_col;
    param.szText = pDrawStruct->szText;
    pDrawStruct->ds_renderbase.pRenderTarget->DrawString(m_pRenderFont, &param);
#endif
}

IRenderFont *ContrastColorListTextRender::GetRenderFont() {
  return m_pRenderFont.get();
}

void ContrastColorListTextRender::SetRenderFont(
    std::shared_ptr<IRenderFont> pFont) {
  m_pRenderFont = pFont;
}

void ContrastColorListTextRender::SetCount(int nCount) {
  this->Clear();
  m_nCount = nCount;

  for (int i = 0; i < m_nCount; i++) {
    m_vTextColor.push_back(nullptr);
  }
}
int ContrastColorListTextRender::GetCount() { return (int)m_vTextColor.size(); }
#endif
//////////////////////////////////////////////////////////////////////////
 
#if 0
//////////////////////////////////////////////////////////////////////////
FontColorListTextRender::FontColorListTextRender(IFontColorListTextRender *p)
    : TextRenderBase(p) {
  m_nCount = 0;
  m_pIFontColorListTextRender = p;
}
FontColorListTextRender::~FontColorListTextRender() { this->Clear(); }
void FontColorListTextRender::Clear() {
  // for (int i = 0; i < m_nCount; i++) {
  //   SAFE_RELEASE(m_vTextColor[i]);
  //   SAFE_RELEASE(m_vTextFont[i]);
  // }
  // m_vTextFont.clear();
  // m_vTextColor.clear();
  // m_nCount = 0;
}

void FontColorListTextRender::OnSerialize(SerializeParam *pData) {
  {
    AttributeSerializer s(pData, "FontColorListTextRender");
    TextRenderBase::Serialize(&s);

    s.AddInt(XML_TEXTRENDER_FONTCOLORLIST_COUNT,
             Slot(&FontColorListTextRender::SetCount, this),
             Slot(&FontColorListTextRender::GetCount, this));

    s.AddString(XML_TEXTRENDER_COLOR,
                Slot(&FontColorListTextRender::LoadColor, this),
                Slot(&FontColorListTextRender::GetColor, this));

    s.AddString(XML_TEXTRENDER_FONT,
                Slot(&FontColorListTextRender::LoadFont, this),
                Slot(&FontColorListTextRender::GetFont, this));
  }
}

void FontColorListTextRender::LoadColor(const char *szText) {
  // if (!szText)
  //   return;

  // std::vector<std::string> vColors;
  // UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
  // int nCount = (int)vColors.size();

  // if (0 == m_nCount)
  //   this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小

  // IColorRes *pColorRes = GetSkinColorRes();
  // if (!pColorRes)
  //   return;

  // for (int i = 0; i < m_nCount && i < nCount; i++) {
  //   if (!vColors[i].empty()) {
  //     pColorRes->GetColor(vColors[i].c_str(), &m_vTextColor[i]);
  //   }
  // }
}
const char *FontColorListTextRender::GetColor() {
  // std::string &strBuffer = GetTempBufferString();
  // for (int i = 0; i < m_nCount; i++) {
  //   if (i > 0)
  //     strBuffer.push_back(XML_MULTI_SEPARATOR);

  //   const char *szTemp = _SaveColor(m_vTextColor[i]);
  //   if (szTemp)
  //     strBuffer.append(szTemp);
  // }

  // return strBuffer.c_str();
  return nullptr;
}

// void FontColorListTextRender::LoadFont(const char *szText) {
//   IFontRes *pFontRes = GetSkinFontRes();

//   if (szText && m_pObject) {
//     std::vector<std::string> vFont;
//     UI_Split(szText, XML_MULTI_SEPARATOR, vFont);
//     int nCount = (int)vFont.size();

//     if (0 == m_nCount)
//       this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小

// #if 0
// 		eGraphicsLibraryType eType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
// 		WindowBase* pWnd = m_pObject->GetWindowObject();
// 		if (pWnd)
// 		{
// 			WindowRender* pRender = pWnd->GetWindowRender();
// 			if (pRender)
// 				eType = pRender->GetGraphicsRenderType();
// 		}

//         for(int i = 0; i < m_nCount && i < nCount; i++ )
//         {
//             if(! vFont[i].empty() )
//             {
// 				pFontRes->GetFont(vFont[i].c_str(), eType, &m_vTextFont[i]);
//             }
//         }
// #endif
//   }

// #if 0
// 	WindowBase* pWnd = m_pObject->GetWindowObject();

//     for(int i = 0; i < m_nCount; i++ )
//     {
//         if(nullptr == m_vTextFont[i])
//         {
// 			if (pWnd)
// 				m_vTextFont[i] = pWnd->GetWindowDefaultRenderFont();
//             if (m_vTextFont[i])
//                 m_vTextFont[i]->AddRef();
//         }
//     }
// #else
//   UIASSERT(false);
// #endif
// }

// const char *FontColorListTextRender::GetFont() {
//   std::string &strBuffer = GetTempBufferString();
//   for (int i = 0; i < m_nCount; i++) {
//     if (i > 0)
//       strBuffer.push_back(XML_MULTI_SEPARATOR);

//     const char *szTemp = _SaveFont(m_vTextFont[i].get());
//     if (szTemp)
//       strBuffer.append(szTemp);
//   }

//   return strBuffer.c_str();
// }

void FontColorListTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE *pDrawStruct) {
  if (0 == m_nCount)
    return;

  int nRealState = (pDrawStruct->ds_renderbase.nState) & 0xFFFF;
  if (nRealState >= m_nCount)
    nRealState = 0;

  assert(0);
#if 0 // TODO:
  DRAWTEXTPARAM param;
  if (m_vTextColor[nRealState])
    param.color = *m_vTextColor[nRealState];

  param.nFormatFlag = pDrawStruct->nDrawTextFlag == -1
                          ? m_nDrawTextFlag
                          : pDrawStruct->nDrawTextFlag;
  param.szText = pDrawStruct->szText;
  param.prc = &(pDrawStruct->ds_renderbase.rc);
  pDrawStruct->ds_renderbase.pRenderTarget->DrawString(m_vTextFont[nRealState].get(),
                                                       &param);
#endif
}
IRenderFont *FontColorListTextRender::GetRenderFont() {
  if (m_vTextFont.size() > 0)
    return m_vTextFont[0].get();

  return nullptr;
}

void FontColorListTextRender::SetRenderFont(
    std::shared_ptr<IRenderFont> pRenderFont) {
  if (m_vTextFont.size() > 0) {
    m_vTextFont[0] = pRenderFont;
  }
}

void FontColorListTextRender::SetCount(int nCount) {
  // this->Clear();
  // m_nCount = nCount;

  // for (int i = 0; i < m_nCount; i++) {
  //   m_vTextColor.push_back(nullptr);
  //   m_vTextFont.push_back(nullptr);
  // }
}

int FontColorListTextRender::GetCount() { return m_nCount; }

void FontColorListTextRender::SetColor(int nIndex, unsigned int col) {
  // nIndex = nIndex & 0xFFFF;
  // if (nIndex >= m_nCount)
  //   return;

  // if (m_vTextColor[nIndex]) {
  //   SAFE_RELEASE(m_vTextColor[nIndex]);
  // }
  // m_vTextColor[nIndex] = Color::CreateInstance(col);
}
void FontColorListTextRender::SetFont(int nIndex,
                                      std::shared_ptr<IRenderFont> pFont) {
  if (nIndex >= m_nCount)
    return;

  m_vTextFont[nIndex] = pFont;
}
#endif
//////////////////////////////////////////////////////////////////////////
#if 0
ThemeTextRenderBase::ThemeTextRenderBase(IThemeTextRenderBase* p):TextRenderBase(p)
{
    m_pIThemeTextRenderBase = p;
	m_hTheme = nullptr;
    m_bNoTheme = false;
}
ThemeTextRenderBase::~ThemeTextRenderBase()
{
	if (m_hTheme)
	{
		::CloseThemeData( m_hTheme );
		m_hTheme = nullptr;
	}
}
void ThemeTextRenderBase::OnInit()
{
	this->CreateTheme();
}
void ThemeTextRenderBase::OnThemeChanged()
{
	this->CreateTheme();
}
void ThemeTextRenderBase::CreateTheme()
{
    if (m_hTheme)
    {
        ::CloseThemeData(m_hTheme);
        m_hTheme = nullptr;
    }
    if (!m_bNoTheme)
    {
        const char* pThemeName = m_pIThemeTextRenderBase->GetThemeName();
        if (pThemeName)
        {
            m_hTheme = ::OpenThemeData(m_pObject->GetHWND(), pThemeName);
        }
    }
}

void  ThemeTextRenderBase::Serialize(AttributeSerializerWrap* ps)
{
	AttributeSerializer* psImpl = ps->GetImpl();
	TextRenderBase::Serialize(psImpl);
	
	psImpl->AddBool(XML_RENDER_THEME_DISABLE, this,
		memfun_cast<pfnBoolSetter>(&ThemeTextRenderBase::SetNoTheme),
		memfun_cast<pfnBoolGetter>(&ThemeTextRenderBase::GetNoTheme));
}

#endif

} // namespace ui