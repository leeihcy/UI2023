#include "colorres.h"
#include "include/interface/imapattr.h"
#include "include/interface/iuires.h"
#include "include/util/log.h"
#include "src/util/util.h"
#include "res_bundle.h"
#include "uiresource.h"

namespace ui {

//
//	根据参数中提供的原始数据pSaveBits，将自己偏移wNewH色调
//
bool ChangeColorHue(byte &R, byte &G, byte &B, short h, bool bOffsetOrReplace) {
  if (R == G && G == B) // 灰色系不能改变它的色调，永远为160
    return false;

  Color c = Color::MakeRGB(R, G, B);

  HSL hsl;
  c.GetHSL(hsl);

  if (bOffsetOrReplace)
    hsl.hue += h;
  else
    hsl.hue = h;

  while (hsl.hue < MIN_HUE_VALUE)
    hsl.hue += MAX_HUE_VALUE;
  while (hsl.hue >= MAX_HUE_VALUE)
    hsl.hue -= MAX_HUE_VALUE;

  c.SetHSL(hsl);

  R = c.r;
  G = c.g;
  B = c.b;

  return true;
}

void ChangeColorLuminance(byte &R, byte &G, byte &B, short l,
                          float dL) // dL = l/100;
{
  if (l > 0) // 相当于是在背景图上面盖了一张全白的图片（alpha为dL)
  {
    R = R + (byte)((255 - R) * dL);
    G = G + (byte)((255 - G) * dL);
    B = B + (byte)((255 - B) * dL);
  } else if (l < 0) // 相当于是在背景图上面盖了一张全黑的图片(alpha为dL)
  {
    R = R + (byte)(R * dL);
    G = G + (byte)(G * dL);
    B = B + (byte)(B * dL);
  }
#define CHECK_RGB_RANGE(x)                                                     \
  if (x > 255)                                                                 \
    x = 255;                                                                   \
  if (x < 0)                                                                   \
    x = 0;

  CHECK_RGB_RANGE(R);
  CHECK_RGB_RANGE(G);
  CHECK_RGB_RANGE(B);
}

bool ChangeColorHueAndSaturation(byte &R, byte &G, byte &B, short h,
                                 bool bOffsetOrReplace, short s, float dS) {
  if (R == G && G == B) // 灰色系不能改变它的色调，永远为160
    return false;

  Color c = Color::MakeRGB(R, G, B);

  HSL hsl;
  c.GetHSL(hsl);

  // hue
  if (bOffsetOrReplace)
    hsl.hue += h;
  else
    hsl.hue = h;

  while (hsl.hue < MIN_HUE_VALUE)
    hsl.hue += MAX_HUE_VALUE;
  while (hsl.hue >= MAX_HUE_VALUE)
    hsl.hue -= MAX_HUE_VALUE;

  // saturation
  if (s > 0)
    hsl.saturation = (hsl.saturation / (1 - dS));
  else
    hsl.saturation = (hsl.saturation * (1 + dS));

  if (hsl.saturation <= MIN_SATURATION_VALUE)
    hsl.saturation = 0, hsl.hue = 0; // 灰色系了

  if (hsl.saturation > MAX_SATURATION_VALUE)
    hsl.saturation = MAX_SATURATION_VALUE;

  c.SetHSL(hsl);
  R = c.r;
  G = c.g;
  B = c.b;

  return true;
}
bool ChangeColorSaturation(byte &R, byte &G, byte &B, short s, float dS) {
  if (R == G && G == B) // 灰色系不能改变它的色调，永远为160
    return false;

  Color c = ui::Color::MakeRGB(R, G, B);

  HSL hsl;
  c.GetHSL(hsl);

  // saturation
  if (s > 0)
    hsl.saturation = (hsl.saturation / (1 - dS));
  else
    hsl.saturation = (hsl.saturation * (1 + dS));

  if (hsl.saturation <= MIN_SATURATION_VALUE)
    hsl.saturation = 0, hsl.hue = 0; // 灰色系了

  if (hsl.saturation > MAX_SATURATION_VALUE)
    hsl.saturation = MAX_SATURATION_VALUE;

  c.SetHSL(hsl);
  R = c.r;
  G = c.g;
  B = c.b;

  return true;
}

bool ChangeColorHLS(byte &R, byte &G, byte &B, short h, short l, short s,
                    int nFlag) {
  bool bChangeH = nFlag & CHANGE_SKIN_HLS_FLAG_H ? true : false;
  bool bChangeL = nFlag & CHANGE_SKIN_HLS_FLAG_L ? true : false;
  bool bChangeS = nFlag & CHANGE_SKIN_HLS_FLAG_S ? true : false;
  bool bSetHueMode = nFlag & CHANGE_SKIN_HLS_FALG_REPLACE_MODE ? false : true;
  if (l == 0)
    bChangeL = false;
  if (s == 0)
    bChangeS = false;

  if (false == bChangeH && false == bChangeL && false == bChangeS)
    return false;

  float dL = 0, ds = 0;
  if (bChangeL)
    dL = (float)(l / 100.0);
  if (bChangeS)
    ds = (float)(s / 100.0);

  if (bChangeL)
    ChangeColorLuminance(R, G, B, l, dL);

  if (bChangeH && bChangeS) {
    ChangeColorHueAndSaturation(R, G, B, h, bSetHueMode, s, ds);
  } else {
    if (bChangeH)
      ChangeColorHue(R, G, B, h, bSetHueMode);
    if (bChangeS)
      ChangeColorSaturation(R, G, B, s, ds);
  }
  return true;
}

ColorResItem::ColorResItem() {
  m_bUseSkinHLS = true;
}
ColorResItem::~ColorResItem() {
}
IColorResItem *ColorResItem::GetIColorResItem() {
  if (!m_pIColorResItem)
    m_pIColorResItem = std::make_unique<IColorResItem>(this);

  return m_pIColorResItem.get();
}
const char *ColorResItem::GetColorString() { return m_strColor.c_str(); }
// bool ColorResItem::IsMyColor(Color *p) {
//   if (m_pColor && p == m_pColor)
//     return true;

//   return false;
// }
const std::string &ColorResItem::GetColorStringRef() { return m_strColor; }

void ColorResItem::SetId(const char *szId) {
  if (szId)
    m_strId = szId;
  else
    m_strId.clear();
}
const char *ColorResItem::GetId() { return m_strId.c_str(); }
const std::string &ColorResItem::GetIdRef() { return m_strId; }
void ColorResItem::SetUseSkinHLS(bool b) { m_bUseSkinHLS = b; }
bool ColorResItem::GetUseSkinHLS() { return m_bUseSkinHLS; }

Color ColorResItem::GetColor() {
  if (!m_valid) {
    m_color = util::TranslateRGB(m_strColor.c_str(), XML_SEPARATOR);
    m_valid = true;
  }
  return m_color;
}

void ColorResItem::SetColor(const std::string &strColor) {
  m_strColor = strColor;
   m_valid = false;
}

void ColorResItem::SetAttribute(IAttributeMap *attribute_map) {
  bool bUseSkinHLS = true;
  attribute_map->GetAttr_bool(XML_COLOR_USESKINHLS, true, &bUseSkinHLS);
  this->SetUseSkinHLS(bUseSkinHLS);
}

bool ColorResItem::ModifyHLS(short h, short l, short s, int nFlag) {
  if (false == m_bUseSkinHLS)
    return true;

// TODO:
  // if (m_pColor) {
  //   unsigned char R = GetRValue(*m_pOriginColorValue);
  //   unsigned char G = GetGValue(*m_pOriginColorValue);
  //   unsigned char B = GetBValue(*m_pOriginColorValue);

  //   if (ChangeColorHLS(R, G, B, h, l, s, nFlag))
  //     m_pColor->ReplaceRGB(RGB(R, G, B));
  // }
  return true;
}

ColorRes::ColorRes(ResourceBundle *p) {
  m_resource_bundle = p;
  m_pIColorRes = nullptr;
}

ColorRes::~ColorRes() {
  this->Clear();
  SAFE_DELETE(m_pIColorRes);
}

IColorRes &ColorRes::GetIColorRes() {
  if (!m_pIColorRes)
    m_pIColorRes = new IColorRes(this);

  return *m_pIColorRes;
}
long ColorRes::GetCount() { return (long)m_items.size(); }

ColorResItem *ColorRes::GetColorItem(int nIndex) {
  if (nIndex < 0)
    return nullptr;
  if (nIndex >= (int)m_items.size())
    return nullptr;

  return m_items[nIndex];
}

ColorResItem *ColorRes::GetColorItem(const std::string &strID) {
  std::vector<ColorResItem *>::iterator iter = m_items.begin();
  std::vector<ColorResItem *>::iterator iterEnd = m_items.end();

  for (; iter != iterEnd; iter++) {
    ColorResItem *p = *iter;
    if (p->GetIdRef() == strID) {
      return p;
    }
  }
  return nullptr;
}

Color ColorRes::GetColor(const char *id, bool* exist) {
  if (exist) {
     *exist = false; 
  }

  if (!id || !id[0]) {
    return Color::transparent();
  }

  // // 直接翻译，不根据ID去映射
  // if (szColorId[0] == '#') {
  //   Color color = util::TranslateHexColor(szColorId + 1);
  //   *pp = Color::CreateInstance(color);
  //   return;
  // } else if (szColorId[0] == '0' && szColorId[1] == 'x') {
  //   Color color = util::TranslateHexColor(szColorId + 2);
  //   *pp = Color::CreateInstance(color);
  //   return;
  // } else if (strstr(szColorId, ",")) {
  //   Color color = util::TranslateColor(szColorId);
  //   *pp = Color::CreateInstance(color);
  //   return;
  // }

  ColorResItem *pItem = this->GetColorItem(id);
  if (nullptr == pItem) {
    return Color::transparent();
  }

  if (exist) { *exist = true; }
  return pItem->GetColor();

  // bool bFirstTimeCreate = false;
  // pItem->GetColor(pp, &bFirstTimeCreate);
  // if (bFirstTimeCreate && pItem->GetUseSkinHLS()) {
  //   // 检查当前皮肤的HLS
  //   if (m_resource_bundle && m_resource_bundle->GetHLSInfo()) {
  //     SKIN_HLS_INFO *pHLSInfo = m_resource_bundle->GetHLSInfo();
  //     pItem->ModifyHLS(pHLSInfo->h, pHLSInfo->l, pHLSInfo->s, pHLSInfo->nFlag);
  //   }
  // }
}

//
// 从文件中加载一项(由CXmlImageParse::load_from_file中调用)
//
bool ColorRes::LoadItem(IAttributeMap *attribute_map, const char *szValue) {
  std::string strID;

  const char *szText = attribute_map->GetAttr(XML_ID, true);
  if (szText)
    strID = szText;

  ColorResItem *pItem = nullptr;
  if (this->InsertColor(strID, szValue, &pItem)) {
    pItem->SetAttribute(attribute_map);
    return true;
  } else {
    UI_LOG_WARN("insert image m_strId=%s, path=%s failed.", strID.c_str(),
                szValue);
    return false;
  }
}
bool ColorRes::InsertColor(const std::string &strID, const char *szColor,
                           ColorResItem **pItem) {
  if (nullptr == szColor)
    return false;

  ColorResItem *p = this->GetColorItem(strID);
  if (p) {
    UI_LOG_WARN("ColorRes::InsertColor failed, insert item=%s, color=%s",
                strID.c_str(), szColor);
    return false;
  }

  ColorResItem *pColorItem = new ColorResItem;
  pColorItem->SetId(strID.c_str());
  pColorItem->SetColor(szColor);

  this->m_items.push_back(pColorItem);

  if (pItem)
    *pItem = pColorItem;

  return true;
}

bool ColorRes::ModifyColor(const std::string &strID,
                           const std::string &strColor) {
  ColorResItem *p = this->GetColorItem(strID);
  if (p) {
    p->SetColor(strColor);
    return true;
  }

  return false;
}

bool ColorRes::RemoveColor(const std::string &strID) {
  std::vector<ColorResItem *>::iterator iter = m_items.begin();
  std::vector<ColorResItem *>::iterator iterEnd = m_items.end();

  for (; iter != iterEnd; iter++) {
    ColorResItem *p = *iter;
    if (p->GetIdRef() == strID) {
      delete p;
      p = nullptr;
      m_items.erase(iter);
      return true;
    }
  }

  return false;
}
void ColorRes::Clear() {
  std::vector<ColorResItem *>::iterator iter = m_items.begin();
  std::vector<ColorResItem *>::iterator iterEnd = m_items.end();

  for (; iter != iterEnd; iter++) {
    ColorResItem *p = *iter;
    delete p;
    p = nullptr;
  }

  m_items.clear();
}

bool ColorRes::ChangeSkinHLS(short h, short l, short s, int nFlag) {
  std::vector<ColorResItem *>::iterator iter = m_items.begin();
  std::vector<ColorResItem *>::iterator iterEnd = m_items.end();

  for (; iter != iterEnd; iter++) {
    ColorResItem *p = *iter;
    p->ModifyHLS(h, l, s, nFlag);
  }

  return true;
}

const char *ColorRes::GetColorId(Color *p) {
  // std::vector<ColorResItem *>::iterator iter = m_items.begin();
  // for (; iter != m_items.end(); ++iter) {
  //   if ((*iter)->IsMyColor(p))
  //     return (*iter)->GetId();
  // }

  return nullptr;
}
}