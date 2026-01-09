#ifndef _HTML_BASE_MEMORY_H_
#define _HTML_BASE_MEMORY_H_

#include <cstddef>
#include <stdlib.h>
#include <type_traits>

namespace html {

namespace memory {

// blink使用gc来绘制对象的生命周期。目前这还没有引入gc，使用枚举来控制。
//
// 要达成的目标：
//   1. 外部直接使用指针进行操作，不需要 std::shared_ptr/std::unique_ptr
//   2. 指针可以是全局或静态的这种，不需要内存分配。也可以是由Pool分配的，由Pool统一分配。
//      也可以是new/malloc的，需要进行delete/free.
//   3. 外部不用管生命周期的操作，解决内存泄露和重复释放问题
//
enum class LifeCycleType {
  Unknown,
  Managed,  // 如成员变量、栈变量、全局变量、静态变量
  Pool,     // 资源池维护的变量
  // Malloc,   // 单独创建的变量，需要主动释放。
  New,      // 单独创建的变量，需要主动释放。
  // GC,       // gc, 未实现。
};

// 1. 根据LifeCycleType自动释放对象。
// 2. 限制指针拷贝，只允许指针转移。
template<class T>
class UniquePtr {
public:
  // 允许隐式转换直接用null构造
  UniquePtr(std::nullptr_t) : m_t(nullptr) {
  }

  UniquePtr(T* t, LifeCycleType type) : m_t(t), m_type(type) {
  }

  UniquePtr(const UniquePtr& o) = delete;
  UniquePtr& operator=(const UniquePtr& o) = delete;

  // 拷贝时，只能转移，避免多个引用。
  UniquePtr(UniquePtr&& o) {
    m_t = o.m_t;
    m_type = o.m_type;
    
    o.m_t = nullptr;
    o.m_type = LifeCycleType::Unknown;
  }

  // 允许使用派生类构造基类
  template<class Derived>
  UniquePtr(UniquePtr<Derived>&& o) {
    static_assert(std::is_base_of<T, Derived>::value ||
                  std::is_same<T, Derived>::value,
                  "U must be derived from T or same as T");
    m_t = o.m_t;
    m_type = o.m_type;
    
    o.m_t = nullptr;
    o.m_type = LifeCycleType::Unknown;
  }

  // 释放指针
  ~UniquePtr() {
    destroy();
  }

  void destroy() {
    if (!m_t) {
      return;
    }
    if (m_type == LifeCycleType::New) {
      delete m_t;
    } else if (m_type == LifeCycleType::New) {
      ::free(m_t);
    }
    m_t = nullptr;
  }

  T* operator->() const { 
    return m_t;
  }
  T* get() const {
    return m_t;
  }
  operator bool() {
    return !!m_t;
  }

  static UniquePtr<T> from_pool(T* t) {
    return UniquePtr<T>(t, LifeCycleType::Pool);
  }
  static UniquePtr<T> from_managed(T* t) {
    return UniquePtr<T>(t, LifeCycleType::Managed);
  }

  // 左值版本，调用完之后，外部参数将变成null
  template <typename U> static UniquePtr<T> take_new(U *&t) {
    static_assert(std::is_base_of<T, U>::value ||
                  std::is_same<T, U>::value,
                  "U must be derived from T or same as T");
    // static_assert(__is_base_of(T, U) || __is_same(T, U),
    //               "U must be derived from T or same as T");

    // 为了避免一个指针被引用多次，要求转换t的所有权
    T *temp = t;
    t = nullptr;
    return UniquePtr<T>(temp, LifeCycleType::New);
  }

  // 右值版本，直接传递一个new U()参数的场景。
  template <typename U> static UniquePtr<T> take_new(U *&&t) {
    static_assert(__is_base_of(T, U) || __is_same(T, U),
                  "U must be derived from T or same as T");
    return UniquePtr<T>(t, LifeCycleType::New);
  }

  template <typename U> void reset(UniquePtr<U>&&o) {
    static_assert(__is_base_of(T, U) || __is_same(T, U),
                  "U must be derived from T or same as T");
    destroy();

    m_t = o.m_t;
    m_type = o.m_type;
    o.m_t = nullptr;
    o.m_type = LifeCycleType::Unknown;
  }

  // 直接透传构造参数new
  // 使用完美转发，支持 && 类型（std::move类型的右值）的构造函数参数。
  template <typename... Args> 
  static UniquePtr<T> make_new(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...), LifeCycleType::New);
  }
  
public:
  T* m_t = nullptr;
  LifeCycleType m_type = LifeCycleType::Unknown;
};

}
}
#define U html::memory::UniquePtr

#endif