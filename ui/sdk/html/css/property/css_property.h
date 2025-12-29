#ifndef _UI_SDK_HTML_CSS_PROPERTY_CSSPROPERTY_H_
#define _UI_SDK_HTML_CSS_PROPERTY_CSSPROPERTY_H_

#include "html/css/property/css_value.h"
#include "html/css/property/property_id.h"
namespace html {
class CSSParserContext;
class CSSValue;

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


// 1. 每个属性对应一个派生类，负责flag设置 + Value解析。
// 2. 每个派生类不要添加成员变量，且构造函数要设置为 constexpre，
//    用于构造全局属性表时不要触发构造函数，直接在编译期间完成数组初始化。
//
class CSSProperty {
public:
  static const CSSProperty& Get(CSSPropertyId id);
public:
  constexpr CSSProperty(uint64_t flags) : m_flags(flags) {}
  // constexpr virtual ~CSSProperty(){};

  uint32_t GetFlags() const { return m_flags; }
  bool IsProperty() const { return m_flags & (int)CSSPropertyFlag::Property; }
  bool IsShorthand() const { return m_flags & (int)CSSPropertyFlag::Shorthand; }
  bool IsLonghand() const { return m_flags & (int)CSSPropertyFlag::Longhand; }
  bool IsInherited() const { return m_flags & (int)CSSPropertyFlag::Inherited; }

  // virtual table pointer.
  virtual void foo() {};

protected:
  // int m_id;
  uint64_t m_flags;
};
 
class Variable : public CSSProperty {
public:
  constexpr Variable() : CSSProperty(0) {}
};

class Shorthand : public CSSProperty {
public:
  constexpr Shorthand(int flags) : CSSProperty(flags | (int)CSSPropertyFlag::Shorthand) {}
};

class Longhand : public CSSProperty {
public:
  constexpr Longhand(int flags) : CSSProperty(flags | (int)CSSPropertyFlag::Longhand) {}

  virtual const CSSValue *
  ParseSingleValue(CSSParserContext &context) const {
    return nullptr;
  }
};

class Background final : public Shorthand {
public:
  constexpr Background() : Shorthand(0) {}
};

class BackgroundColor final : public Longhand {
public:
  constexpr BackgroundColor() : Longhand(
      (int)CSSPropertyFlag::Interpolable | 
      (int)CSSPropertyFlag::Compositable | 
      (int)CSSPropertyFlag::Property) {
  }
  const CSSValue * ParseSingleValue(CSSParserContext &context) const override;
};

}

#endif