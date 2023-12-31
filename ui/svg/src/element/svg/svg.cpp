#include "svg.h"
#include "sdk/include/interface/iattribute.h"
#include "sdk/include/interface/imapattr.h"
#include "sdk/include/util/util.h"

namespace svg {

// 计算view port位置。将svg内容填充满控件大小（等比例）
void Svg::calcViewPort(RenderContext &ctx) {
  ui::Size view_port_size = ctx.canvas_size;

  if (m_width.unit == ui::Length::Unit::Number && m_height.unit == ui::Length::Unit::Number) { 
    view_port_size.width = m_width.value;
    view_port_size.height = m_height.value;
  } else {
    if (m_width.unit == ui::Length::Unit::Number) {
      view_port_size.height = view_port_size.width = m_width.value;
    }
    if (m_height.unit == ui::Length::Unit::Number) {
      view_port_size.width = view_port_size.height = m_height.value;
    }
  }

  if (view_port_size.width <= 0 || view_port_size.height <= 0) {
    ctx.view_port.width = ctx.view_port.height = 0;
    return;
  }

  float tan_view_port = (float)view_port_size.width / view_port_size.height;
  float tan_canvas =
      (float)ctx.canvas_size.width / (float)ctx.canvas_size.height;

  // 居中绘制
  ctx.view_port.x = 0;
  ctx.view_port.y = 0;
  if (tan_view_port > tan_canvas) {
    ctx.view_port.width = ctx.canvas_size.width;
    ctx.view_port.height = ctx.canvas_size.width / tan_view_port;
  } else {
    ctx.view_port.height = ctx.canvas_size.height;
    ctx.view_port.width = ctx.canvas_size.height * tan_view_port;
  }

  ctx.view_port.x = (ctx.canvas_size.width - ctx.view_port.width) / 2;
  ctx.view_port.y = (ctx.canvas_size.height - ctx.view_port.height) / 2;
}

SkMatrix Svg::calcViewBoxMatrix(RenderContext &ctx) {
  // 计算view port 和 view box的拉伸比
  float fx = (float)ctx.view_port.width / (float)ctx.view_box.width;
  float fy = (float)ctx.view_port.height / (float)ctx.view_box.height;

  // 等比例拉伸
  float fmin = std::min(fx, fy);
  float scale_x = fmin;
  float scale_y = fmin;

  // 放在左上角。TODO: 居中、右对齐
  float translate_x = -m_view_box.x * scale_x;
  float translate_y = -m_view_box.y * scale_y;

  ctx.scale = fmin;
  ctx.translate_x = translate_x;
  ctx.translate_y = translate_y;

  return SkMatrix::Translate(translate_x, translate_y) *
         SkMatrix::Scale(scale_x, scale_y);
}

bool Svg::prepareToRender(RenderContext &ctx) {
  // 1. 计算viewport的区域，按控件大小与width/height等比计算。
  calcViewPort(ctx);
  if (ctx.view_port.width <= 0 || ctx.view_port.height <= 0) {
    return false;
  }

  SkMatrix content_matrix =
      SkMatrix::Translate(ctx.view_port.x, ctx.view_port.y);

  // 没配置view box时，直接使用configed view port
  if (!m_view_box_configed) {
    ui::Size size = getViewPortWithDefault();
    ctx.view_box = {0, 0, (float)size.width, (float)size.height};
  } else {
    ctx.view_box = m_view_box;
  }

  // 可能是指定了空的
  if (ctx.view_box.width <= 0 || ctx.view_box.height <= 0) {
    return false;
  }

  // 将 view box 缩放大 view port
  SkMatrix matrix_view_box = calcViewBoxMatrix(ctx);
  content_matrix.preConcat(matrix_view_box);

  if (!content_matrix.isIdentity()) {
    ctx.canvas->concat(content_matrix);
  }
  return true;
}

void Svg::Render(RenderContext &ctx) {
  if (!prepareToRender(ctx)) {
    return;
  }
  // prepare中设置了viewbox的matrix
  // 在这里保存一下。
  ctx.matrix = ctx.canvas->getTotalMatrix();

  Group::Render(ctx);
}

void Svg::SetAttribute(ui::SerializeParam &data) {
  Group::SetAttribute(data);

  ui::AttributeSerializerWrap s(&data, "Svg");
  s.AddLength("width", m_width)
      ->SetDefault({100, ui::Length::Unit::Percentage});
  s.AddLength("height", m_height)
      ->SetDefault({100, ui::Length::Unit::Percentage});

  // 手动解析view box
  if (data.pMapAttrib) {
    const char *text = data.pMapAttrib->GetAttr("viewBox", true);
    if (!text) {
      text = data.pMapAttrib->GetAttr("viewbox", true);
    }
    if (text) {
      ui::util::ISplitStringEnum *penum = nullptr;
      ui::util::SplitString(text, ' ', &penum);
      if (penum) {
        if (penum->GetCount() == 4) {
          m_view_box.x = atof(penum->GetText(0));
          m_view_box.y = atof(penum->GetText(1));
          m_view_box.width = atof(penum->GetText(2));
          m_view_box.height = atof(penum->GetText(3));
          m_view_box_configed = true;
        }
        penum->Release();
      }
    }
  }
}

ui::Size Svg::getViewPortWithDefault() {
  ui::Size size = {300, 150};
  if (m_width.unit == ui::Length::Unit::Number) {
    size.width = m_width.value;
  }
  if (m_height.unit == ui::Length::Unit::Number) {
    size.height = m_height.value;
  }
  return size;
}

ui::Size Svg::GetDesiredSize() { 
  return getViewPortWithDefault(); 
}

} // namespace svg