#include "html/css/property/css_property_instances.h"

#include "html/css/property/css_property.h"

namespace html {

// 1. 整个数组编译期间完成初始化
// 2. 使用constexpr构造函数，在编译期间完成初始化，子类不能引入带构造函数的成员变量。
// 3. 不需要再额外引用对象地址，浪费一个指针存储空间，例如早期的版本存在浪费：
//    BackgroundColor g_BackgroundColor;
//    CSSProperty g_all_css_properties[] = {&g_BackgroundColor};
// 4. 虚函数表指针如何初始化的？
//    直接使用 CSSProperty g_properties[COUNT] = {Variable()... } 的方式有问题，
//    会导致虚函数表指针数据丢失，都指向了CSSProperty类的虚表。
//    但如果使用CSSPropertyUnion + 构造函数 + reinterpret_cast的方式，
//    就能解决虚函数表指针问题。
//    虚表是编译期间构造的，也能在编译期间完成对象虚表指针初始化。
//    所有的属性子类都没有成员变量，不存在浪费内存空间的问题。
//    这也限制了所有子类不能使用多重继续，否则虚表指针将没对齐。
//

constexpr int CSS_PROPERTY_COUNT = (int)CSSPropertyId::CSSPropertyCount;

// 虚表指针 + uint64_t
constexpr size_t kCSSPropertyUnionBytes = 16;

// 每个对象的内存地址位置按16倍数对齐。
union alignas(kCSSPropertyUnionBytes) CSSPropertyUnion;


static_assert(kCSSPropertyUnionBytes == sizeof(CSSProperty));
static_assert(kCSSPropertyUnionBytes == sizeof(Variable));
static_assert(kCSSPropertyUnionBytes == sizeof(BackgroundColor));

#define DECLARE_UNION_ITEM2(ClassName, varName) \
  constexpr CSSPropertyUnion(ClassName&& arg) : varName(std::move(arg)) { } \
  ClassName varName;

#define DECLARE_UNION_ITEM(ClassName) \
  DECLARE_UNION_ITEM2(ClassName, __##ClassName)

// 每个对象的内存地址位置按16倍数对齐。
union alignas(kCSSPropertyUnionBytes) CSSPropertyUnion {
  // Sample: 
  // constexpr CSSPropertyUnion(Variable&& arg) : variable(std::move(arg)) { }
  // Variable variable;

  DECLARE_UNION_ITEM(Variable);
  DECLARE_UNION_ITEM(Background);
  DECLARE_UNION_ITEM(BackgroundColor);
};

constexpr CSSPropertyUnion g_css_properties[CSS_PROPERTY_COUNT] = {
  Variable(),
  Background(),
  BackgroundColor(),
};


// static 
const CSSProperty& GetCSSPropertyInternal(CSSPropertyId id) {
  int n = (int)id;

  if (n < 0 && n >= CSS_PROPERTY_COUNT) {
    assert(false);
    abort();
  }
  
  return reinterpret_cast<const CSSProperty&>(g_css_properties[(int)id]);
}

}