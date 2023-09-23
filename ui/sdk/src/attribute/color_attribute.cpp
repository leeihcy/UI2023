#include "color_attribute.h"
#include "attribute.h"
#include "include/inc.h"
#include "src/application/uiapplication.h"
#include "src/object/object.h"
#include "src/resource/res_bundle.h"

namespace ui {
AttributeBase *CreateColorAttribute() { return new ColorAttribute(); }

ColorAttribute::ColorAttribute() : m_default(Color::transparent()) {}

void ColorAttribute::SetBindValue(void *p) { m_bind_value = (Color *)p; }

void ColorAttribute::Bind(slot<void(Color)>&& setter, slot<Color()>&& getter) {
  m_setter.connect(std::forward<slot<void(Color)>>(setter));
  m_getter.connect(std::forward<slot<Color()>>(getter));
}

Color ColorAttribute::parse_color(const char *text) {
  if (!text || !text[0]) {
    return m_default;
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
  else if (text[0] == 'i' && text[1] == 'd' && text[2] == '(' &&
           text[last_index] == ')') {
    if (!m_pSkinRes) {
      return m_default;
    }
    std::string str(text + 2, length - 3);
    ColorRes &colorRes = m_pSkinRes->GetColorRes();
    return colorRes.GetColor(str.c_str(), nullptr);
  }
  // simple color text
  else if (strcmp("transparent", text) == 0) {
    return Color::MakeARGB(0, 0, 0, 0);
  } else if (strcmp("none", text) == 0) {
    return Color::MakeARGB(0, 0, 0, 0);
  }

  else if (strcmp("black", text) == 0) {
    return Color::MakeRGB(0, 0, 0);
  } else if (strcmp("silver", text) == 0) {
    return Color::MakeRGB(192, 192, 192);
  } else if (strcmp("gray", text) == 0 || strcmp("grey", text) == 0) {
    return Color::MakeRGB(128, 128, 128);
  } else if (strcmp("white", text) == 0) {
    return Color::MakeRGB(255, 255, 255);
  } else if (strcmp("maroon", text) == 0) {
    return Color::MakeRGB(128, 0, 0);
  } else if (strcmp("red", text) == 0) {
    return Color::MakeRGB(255, 0, 0);
  } else if (strcmp("purple", text) == 0) {
    return Color::MakeRGB(128, 0, 128);
  } else if (strcmp("fuchsia", text) == 0) {
    return Color::MakeRGB(255, 0, 255);
  } else if (strcmp("green", text) == 0) {
    return Color::MakeRGB(0, 128, 0);
  } else if (strcmp("lime", text) == 0) {
    return Color::MakeRGB(0, 255, 0);
  } else if (strcmp("olive", text) == 0) {
    return Color::MakeRGB(128, 128, 0);
  } else if (strcmp("yellow", text) == 0) {
    return Color::MakeRGB(255, 255, 0);
  } else if (strcmp("navy", text) == 0) {
    return Color::MakeRGB(0, 0, 128);
  } else if (strcmp("blue", text) == 0) {
    return Color::MakeRGB(0, 0, 255);
  } else if (strcmp("teal", text) == 0) {
    return Color::MakeRGB(0, 128, 128);
  } else if (strcmp("aqua", text) == 0) {
    return Color::MakeRGB(0, 255, 255);
  }

  // TODO:
  else if (strcmp("orange", text) == 0) {
    return Color::MakeRGB(255, 165, 0);
  }
  return m_default;
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

  Color c = parse_color(szColorId);
  SetColor(c);
}

void ColorAttribute::Reset() {
  if (m_default_configed) {
    SetColor(m_default);
  } 
}

void ColorAttribute::SetColor(Color c) {
  if (m_bind_value) {
    *m_bind_value = c;
  }
  else if (!m_setter.empty()) {
    m_setter.emit(c);
  }
}
Color ColorAttribute::GetColor() {
  if (m_bind_value) {
    return *m_bind_value;
  }
  else if (!m_getter.empty()) {
    return m_getter.emit();
  }
  return m_default;
}

const char *ColorAttribute::Get() {
  Color c = GetColor();

  char *szBuffer = GetTempBuffer();
  c.ToWebString(szBuffer);
  return szBuffer;
}

void ColorAttribute::SetDefault(Color c) { 
  m_default_configed = true;
  m_default = c; 
}

bool ColorAttribute::IsDefaultValue() {
  if (!m_default_configed) {
    return false;
  }

  if (m_bind_value) {
    return (*m_bind_value) == m_default;
  }

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