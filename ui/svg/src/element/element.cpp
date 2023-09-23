#include "element.h"
#include "sdk/include/interface/iattribute.h"

namespace svg {

void Element::SetParent(Element* p) {
  m_parent = p;
}

Element* Element::GetRootElement() {
  Element* ancestor = this;
  while (ancestor && ancestor->m_parent) {
    ancestor = ancestor->m_parent;
  }
  return ancestor;
}

const std::string& Element::GetId() {
  return m_id;
}

void Element::setId(const char* id) {
  if (id) {
    m_id = id;
  } else {
    m_id.clear();
  }
}

const char* Element::getId() {
  return m_id.c_str();
}

void Element::Render(RenderContext &context) {}
void Element::SetAttribute(ui::SerializeParam &data) {
  ui::AttributeSerializerWrap s(&data, "Element");

  s.AddString("id", ui::Slot(&Element::setId, this),
              ui::Slot(&Element::getId, this));

  s.AddColor("fill", ui::Slot(&Element::SetFillColor, this),
             ui::Slot(&Element::GetFillColor, this));
  s.AddString("transform", ui::Slot(&Element::SetTransform, this),
              ui::Slot(&Element::GetTransform, this));

  s.AddColor("stroke", m_stroke);
  s.AddLength("stroke-width", m_stroke_width)
      ->SetDefault(ui::Length::MakeNumber(1));
}

void Element::SetFillColor(ui::Color c) { m_fill.reset(new ui::Color(c)); }
ui::Color Element::GetFillColor() {
  if (m_fill) {
    return *m_fill;
  }
  return ui::Color::transparent();
}

inline bool is_blank(const char *p) {
  return *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r';
}
std::string strim_token(const char *pstart, const char *pend) {
  const char *p = pstart;
  while (pstart < pend && is_blank(pstart)) {
    pstart++;
  }
  while (pstart < pend && is_blank(pend - 1)) {
    pend--;
  }
  return std::string(pstart, pend - pstart);
}

void parse_transform_tokens(const char *text,
                            std::vector<std::string> &tokens) {
  const char *p = text;

  enum STATE {
    START,
    FoundLeftParenthesized,
  };
  STATE state = START;
  const char *psave = p;

  while (*p) {
    switch (state) {
    case START: {
      if (*p == '(') {
        tokens.push_back(strim_token(psave, p));
        state = FoundLeftParenthesized;
        psave = p + 1;
      }
      p++;
      break;
    }

    case FoundLeftParenthesized: {
      if (*p == ')') {
        tokens.push_back(strim_token(psave, p));
        state = START;
        psave = p + 1;
      }
      p++;
      break;
    }
    }
  }
}

inline bool is_separator(const char *p) {
  return *p == ' ' || *p == ',' || *p == '\t' || *p == '\n' || *p == '\r';
}

// 分隔符可是以,或者空格
void split_transform_item_value(const char *text, std::vector<float> &results) {
  const char *p = text;
  const char *psave = p;

  char *temp = nullptr;

  enum STATE { START, FoundSeparator };
  STATE state = START;
  while (*p) {
    bool is_sep = is_separator(p);
    if (state == START) {
      if (is_sep) {
        state = FoundSeparator;

        std::string str(psave, p);
        results.push_back((float)strtod(str.c_str(), &temp));
      }
    } else if (state == FoundSeparator) {
      if (!is_sep) {
        psave = p;
        state = START;
      }
    }
    p++;
  }
  if (p != psave) {
    std::string str(psave, p);
    results.push_back((float)strtod(str.c_str(), &temp));
  }
}

void Element::SetTransform(const char *text) {
  if (!text || !text[0]) {
    return;
  }

  std::vector<std::string> tokens;
  parse_transform_tokens(text, tokens);

  m_matrix.reset(new SkMatrix());

  int count = (int)tokens.size();
  for (int i = 0; i < (count - 1); i += 2) {
    std::string &key = tokens[i];
    std::string &value = tokens[i + 1];

    std::vector<float> scalars;
    split_transform_item_value(value.c_str(), scalars);
    if (scalars.empty()) {
      continue;
    }
  
    SkMatrix m;
    if (key == "matrix") {
      if (scalars.size() != 6) {
        continue;
      }
      m.setAll(scalars[0], scalars[2], scalars[4], scalars[1], scalars[3],
               scalars[5], 0, 0, 1);
    } else if (key == "rotate") {
      // angle, cx, cy
      if (scalars.size() == 3) {
        m.setRotate(scalars[0], scalars[1], scalars[2]);
      }
      else if (scalars.size() == 1) {
        m.setRotate(scalars[0], 0, 0);
      }
    } else if (key == "translate") {
      m.setTranslate(scalars[0], scalars.size() > 1 ? scalars[1] : 0.0f);
    } else if (key == "scale") {
      m.setScale(scalars[0], scalars.size() > 1 ? scalars[1] : 0.0f);
    } else if (key == "skewX") {
      m.setSkewX(tanf(SkDegreesToRadians(scalars[0])));
    } else if (key == "skewY") {
      m.setSkewY(tanf(SkDegreesToRadians(scalars[0])));
    }
    m_matrix->preConcat(m);
  }
}

const char *Element::GetTransform() {
  // not implement
  return nullptr;
}

void Element::PrepareRender(RenderContext &context) {
  if (m_fill) {
    // 如果本身配置了fill属性，则使用自己的颜色
    context.fill = true;
    context.fill_paint.setColor(m_fill->value);
  } else if (!context.fill) {
    // 父对象也没有配置fill属性，则使用默认的黑色。
    context.fill = true;
    context.fill_paint.setColor(ui::Color::black().value);
  }

  if (m_stroke.value) {
    context.stroke = true;
    context.stroke_paint.setColor(m_stroke.value);

    int stroke_width = context.ResolveX(m_stroke_width);
    context.stroke_paint.setStrokeWidth((SkScalar)stroke_width);
  } else {
  }

  if (m_matrix) {
    context.matrix.preConcat(*m_matrix);
  }
  // context.matrix.preConcat(matrix);

  // SkMatrix matrix;
  // matrix.setTranslate(20, 20);
  // context.matrix.preConcat(matrix);
  // context.canvas->concat(matrix);
  context.canvas->setMatrix(context.matrix);

}

} // namespace svg