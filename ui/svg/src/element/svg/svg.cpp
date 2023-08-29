#include "svg.h"
#include "sdk/include/interface/iattribute.h"
#include "sdk/include/util/util.h"
#include "sdk/include/interface/imapattr.h"

namespace svg {


bool Svg::prepareToRender(RenderContext &context) {
  // TODO: 如果不是顶级的svg，可以指定x,y
  int x = 0;
  int y = 0;

  ui::RectXYWH view_port = {x, y, context.canvas_size.width, context.canvas_size.height};
  if (view_port.width == 0 || view_port.height == 0) {
    return false;
  }
  
  SkMatrix content_matrix = SkMatrix::Translate(view_port.x, view_port.y);

  // 没指定，
  if (m_view_box.x == -1 && m_view_box.y == -1 && m_view_box.width == -1 && m_view_box.height == -1) {
    // 直接使用viewport大小
    if (m_width.unit == ui::Length::Unit::Number && m_height.unit == ui::Length::Unit::Number) {
      context.view_box = {0, 0, m_width.value, m_height.value};
    } else {
      // 如果没有view port
      context.view_box = {0, 0, 300, 150};
    }
  } else {
    // 可能是指定了空的 
    if (m_view_box.width == 0 || m_view_box.height == 0) {
      return false;
    }

    context.view_box = m_view_box;

    // 计算view port 和 view box的拉伸比
    float fx = (float)view_port.width / (float)m_view_box.width;
    float fy = (float)view_port.height / (float)m_view_box.height;
    
    // 等比例拉伸
    float fmin = std::min(fx, fy);
    float scale_x = fmin;
    float scale_y = fmin;

    // 放在左上角。TODO: 居中、右对齐
    float translate_x = -m_view_box.x * scale_x;
    float translate_y = -m_view_box.y * scale_y;

    content_matrix.preConcat(SkMatrix::Translate(translate_x, translate_y) * 
      SkMatrix::Scale(scale_x, scale_y));
  }

  if (!content_matrix.isIdentity()) {
    context.canvas->concat(content_matrix);
  }
  return true;

}

void Svg::Render(RenderContext &context) { 
  if (!prepareToRender(context)) {
    return;
  }
  Container::Render(context); 
}

void Svg::SetAttribute(ui::SerializeParam &data) {
  Container::SetAttribute(data);

  ui::AttributeSerializerWrap s(&data, "Svg");
  s.AddLength("width", m_width)
      ->SetDefault({100, ui::Length::Unit::Percentage});
  s.AddLength("height", m_height)
      ->SetDefault({100, ui::Length::Unit::Percentage});

  // 手动解析view box
  if (data.pMapAttrib) {
    const char* text = data.pMapAttrib->GetAttr("viewBox", true);
    if (!text) {
      text = data.pMapAttrib->GetAttr("viewbox", true);
    }
    if (text) {
      ui::util::ISplitStringEnum* penum = nullptr;
      ui::util::SplitString(text, ' ', &penum);
      if (penum) {
        if (penum->GetCount() == 4) {
          m_view_box.x = atoi(penum->GetText(0));
          m_view_box.y = atoi(penum->GetText(1));
          m_view_box.width = atoi(penum->GetText(2));
          m_view_box.height = atoi(penum->GetText(3));
        }
        penum->Release();
      }
    }
  }
}

ui::Size Svg::GetDesiredSize() {
  if (m_width.unit == ui::Length::Unit::Number &&
      m_height.unit == ui::Length::Unit::Number) {
    return {m_width.value, m_height.value};
  }
  return {0, 0};
}

} // namespace svg