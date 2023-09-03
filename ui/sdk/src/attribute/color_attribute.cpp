#include "color_attribute.h"
#include "attribute.h"
#include "include/inc.h"
#include "src/application/uiapplication.h"
#include "src/object/object.h"
#include "src/resource/res_bundle.h"

namespace ui {
AttributeBase *CreateColorAttribute() { return new ColorAttribute(); }

void ColorAttribute::SetBindValue(void *p) { m_bind_value = (Color *)p; }
// void ColorAttribute::SetBindValue2(void **p) { m_ppBindValue = (Color **)p; }


Color ColorAttribute::parse_color(const char* text) {
  if (!text || !text[0]) {
    return Color::transparent();
  }

  size_t length = strlen(text);
  size_t last_index = length - 1;

  // #
  if (text[0] == '#') {
    return util::TranslateHexColor(text + 1);
  }
  // 0x
  else if (text[0] == '0' && text[1] == 'x') {
    return util::TranslateHexColor(text + 2);
  }
  // id()
  else if (text[0] == 'i' && text[1] == 'd' && text[2] == '(' && text[last_index] == ')') {
    if (!m_pSkinRes) {
      return Color::transparent();
    }
    std::string str(text+2, length-3);
    ColorRes &colorRes = m_pSkinRes->GetColorRes();
    return colorRes.GetColor(str.c_str(), nullptr);
  }
  // simple color text
  else if (strcmp("black", text) == 0) { return Color::MakeRGB(0,0,0); }
  else if (strcmp("silver", text) == 0) { return Color::MakeRGB(192,192,192); }
  else if (strcmp("gray", text) == 0) { return Color::MakeRGB(128,128,128); }
  else if (strcmp("white", text) == 0) { return Color::MakeRGB(255,255,255); }
  else if (strcmp("maroon", text) == 0) { return Color::MakeRGB(128,0,0); }
  else if (strcmp("red", text) == 0) { return Color::MakeRGB(255,0,0); }
  else if (strcmp("purple", text) == 0) { return Color::MakeRGB(128,0,128); }
  else if (strcmp("fuchsia", text) == 0) { return Color::MakeRGB(255,0,255); }
  else if (strcmp("green", text) == 0) { return Color::MakeRGB(0,128,0); }
  else if (strcmp("lime", text) == 0) { return Color::MakeRGB(0,255,0); }
  else if (strcmp("olive", text) == 0) { return Color::MakeRGB(128,128,0); }
  else if (strcmp("yellow", text) == 0) { return Color::MakeRGB(255,255,0); }
  else if (strcmp("navy", text) == 0) { return Color::MakeRGB(0,0,128); }
  else if (strcmp("blue", text) == 0) { return Color::MakeRGB(0,0,255); }
  else if (strcmp("teal", text) == 0) { return Color::MakeRGB(0,128,128); }
  else if (strcmp("aqua", text) == 0) { return Color::MakeRGB(0,255,255); }

  // TODO: 
  else if (strcmp("orange", text) == 0) { return Color::MakeRGB(255, 165, 0); }
  return Color::transparent();
}

void ColorAttribute::Set(const char *szColorId) {
  
//   if (m_ppBindValue) {
//     if (!szColorId)
//       return;
      // if (m_pSkinRes) {
//     ColorRes &colorRes = m_pSkinRes->GetColorRes();
//     colorRes.GetColor(szColorId, m_ppBindValue);
//     }
// // #ifdef EDITOR_MODE
// //     if (*m_ppBindValue) {
// //       (*m_ppBindValue)->SetResId(szColorId);
// //     }
// // #endif
//   } else 
  
  if (m_bind_value) {
    if (!szColorId)
      return;

      *m_bind_value = parse_color(szColorId);

// #ifdef EDITOR_MODE
//     if (m_ppBindValue) {
//       m_bind_value->SetResId(szColorId);
//     }
// #endif
  }
}

void ColorAttribute::Reset() {
  // if (m_ppBindValue) {
  //   *m_ppBindValue = nullptr;
  // } else if (m_bind_value) {
  // }
}

const char *ColorAttribute::Get() {
//   if (m_ppBindValue && *m_ppBindValue) {
// #ifdef EDITOR_MODE
//     const char *szText = (*m_ppBindValue)->GetResId();
//     if (szText)
//       return szText;
// #endif

//     char *szBuffer = GetTempBuffer();
//     (*m_ppBindValue)->ToWebString(szBuffer);
//     return szBuffer;
//   } else 
if (m_bind_value) {
// #ifdef EDITOR_MODE
//     const char *szText = m_bind_value->GetResId();
//     if (szText)
//       return szText;
// #endif

    char *szBuffer = GetTempBuffer();
    m_bind_value->ToWebString(szBuffer);
    return szBuffer;
  }

  return nullptr;
}

bool ColorAttribute::IsDefaultValue() {
  // if (m_ppBindValue) {
  //   if (*m_ppBindValue == nullptr)
  //     return true;
  // } else if (m_bind_value) {
  // }

  return false;
}

void ColorAttribute::Editor(SerializeParam *pData, AttributeEditorProxy *p,
                            EditorAttributeFlag e) {
  p->Color2Editor(this, e);
}

IColorAttribute *ColorAttribute::GetIColorAttribute() {
  if (!m_pIColorAttribute) {
    m_pIColorAttribute = std::make_unique<IColorAttribute>(this);
  }
  return m_pIColorAttribute.get();
}

} // namespace ui