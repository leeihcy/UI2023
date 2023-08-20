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
struct IResource;

// 在std unique_ptr的基础上，封装自动创建和自动销毁的逻辑

template <class T> class unique_ptr : public std::unique_ptr<T, void (*)(T *)> {
public:
  using super = std::unique_ptr<T, void (*)(T *)>;
  
  unique_ptr(IResource *p) : super(T::create(p), T::destroy) {}

  // static void deletor(T* p) {
  //   p->destroy();
  // }
  // // 基类指针
  // unique_ptr(T* p) : super(p, unique_ptr<T>::deletor) {}
};

} // namespace ui
#endif