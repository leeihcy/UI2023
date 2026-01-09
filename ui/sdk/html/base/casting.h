#ifndef _HTML_BASE_CASTING_H_
#define _HTML_BASE_CASTING_H_

#include <type_traits>
namespace html {

//
// blink/renderer/platform/wtf/casting.h
// 替代dynamic_cast，自定义实现派生类转换，避免RTTI+虚函数开销。
//
// 使用方法示例
// #include "html/base/casting.h"
// 
// template<>
// struct DowncastTraits<CSSNumericLiteralValue> {
//   static bool AllowFrom(const CSSPrimitiveValue& from) {
//     return from.IsNumericLiteral();
//   }
// };
//

template <typename  Derived>
struct DowncastTraits;

namespace internal {

template <typename Derived, typename Base>
struct DowncastTraitsHelper {
};

// Base是基类。
template <typename Derived, typename Base>
requires(!std::is_base_of_v<Derived, Base>)
struct DowncastTraitsHelper<Derived, Base> {
  static bool AllowFrom(const Base& from) { 
    return DowncastTraits<Derived>::AllowFrom(from);
  }
};

// Derived是Base的基类场景。
template <typename Derived, typename Base>
requires(std::is_base_of_v<Derived, Base>)
struct DowncastTraitsHelper<Derived, Base> {
  static bool AllowFrom(const Base& from) { 
    return true;
  }
};

}


template <typename Derived, typename Base>
bool IsA(Base& from) {
  // 首先从编译层面确保两个类是继承关系。
  static_assert(std::is_base_of_v<Base, Derived>, "");

  // 再从运行时层面确认Derived。例如Base::m_type == DERIVED_TYPE
  return internal::DowncastTraitsHelper<Derived, const Base>::AllowFrom(from);
}
template <typename Derived, typename Base>
bool IsA(const Base& from) {
  // 首先从编译层面确保两个类是继承关系。
  static_assert(std::is_base_of_v<Base, Derived>, "");

  // 再从运行时层面确认Derived。例如Base::m_type == DERIVED_TYPE
  return internal::DowncastTraitsHelper<Derived, const Base>::AllowFrom(from);
}

template <typename Derived, typename Base>
bool IsA(Base* from) {
  static_assert(std::is_base_of_v<Base, Derived>, "");
  return from && IsA<Derived>(*from);
}
template <typename Derived, typename Base>
bool IsA(const Base* from) {
  static_assert(std::is_base_of_v<Base, Derived>, "");
  return from && IsA<Derived>(*from);
}

template <typename Derived, typename Base>
Derived* DynamicTo(Base* from) {
  return IsA<Derived>(from) ? static_cast<Derived*>(from):nullptr;
}
template <typename Derived, typename Base>
const Derived* DynamicTo(const Base* from) {
  return IsA<Derived>(from) ? static_cast<const Derived*>(from):nullptr;
}

template <typename Derived, typename Base>
Derived* DynamicTo(Base& from) {
  return IsA<Derived>(from) ? static_cast<Derived*>(&from):nullptr;
}
template <typename Derived, typename Base>
const Derived* DynamicTo(const Base& from) {
  return IsA<Derived>(from) ? static_cast<const Derived*>(&from):nullptr;
}

}

#endif