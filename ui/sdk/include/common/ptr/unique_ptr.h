#ifndef _UI_SDK_INCLUDE_COMMON_PTR_UNIQUEPTR_H_
#define _UI_SDK_INCLUDE_COMMON_PTR_UNIQUEPTR_H_
#include <memory>
//
// 2023/8/17 21:26 libo
// 
// 针对ui对象的unique_ptr，避免出现祼指针的ui对象
//
// 这类ui对象需要实现以下两个函数：
//  static T* create() { return new T(); }
//  static void destroy(T* p) { delete p; }


namespace ui {
struct IResBundle;

template <class T> class unique_ptr : public std::unique_ptr<T, void (*)(T *)> {
  using super = std::unique_ptr<T, void (*)(T *)>;

public:
  // 构造函数没有参数，如 IApplication
  unique_ptr() : super(T::create(), T::destroy) {}

  // 构造函数需要传递一个资源对象，如 IWindow
  unique_ptr(IResBundle *p) : super(T::create(p), T::destroy) {}
};

} // namespace ui
#endif