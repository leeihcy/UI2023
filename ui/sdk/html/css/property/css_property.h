#ifndef _UI_SDK_HTML_CSS_PROPERTY_CSSPROPERTY_H_
#define _UI_SDK_HTML_CSS_PROPERTY_CSSPROPERTY_H_

#include "html/css/property/property_id.h"
#include <_types/_uint64_t.h>
namespace html {
class CSSParserContext;

enum class CSSPropertyFlag : int {
  // 表示该属性的值可以在两个状态之间平滑插值计算，是支持CSS动画/过渡的关键.
  // 例如：color, width, transform
  Interpolable = 0x1,

  // 当该属性的值发生变化时，浏览器通常只需要进行合成（Compositor），跳过代价
  // 高昂的布局（Layout）和绘制（Paint）阶段。
  // 例如：transform, translate, rotate, scale, opacity, filter
  Compositable = 0x2,

  // @rule中的属性
  Descriptor = 0x4,
  // 普通属性
  Property = 0x8,

  // 该属性是一个简写属性，可以被拆成多个Longhand。
  // 例如：margin, background
  Shorthand = 0x10,
  // 例如：margin-top, background-color
  Longhand = 0x20,

  // 表示子元素会继承父元素的值。
  // 例如：font-size, color
  Inherited = 0x40,

  // Visited = 0x80,
  // Internal = 0x100,
  // Animation = 0x200,

  // 当有这三个属性时，表单控件元素将不再使用native样式，使用自定义的css进行绘制。
  // Background,
  // Border,
  // BorderRadius,
};


class CSSProperty {
public:
  static const CSSProperty& Get(CSSPropertyId id);

protected:
  uint64_t m_flags;
};


class Longhand : public CSSProperty {

};

class BackgroundColor final : public Longhand {
  BackgroundColor() {
    m_flags = 
      (int)CSSPropertyFlag::Interpolable | 
      (int)CSSPropertyFlag::Compositable | 
      (int)CSSPropertyFlag::Property;
  }
  bool Parse(CSSParserContext& c);
};

}

#endif