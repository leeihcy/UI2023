#ifndef _HTML_BASE_MEMORY_H_
#define _HTML_BASE_MEMORY_H_

#include <atomic>
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
  Unknown = 0,
  Managed = 1,  // 如成员变量、栈变量、全局变量、静态变量
  Pool = 2,     // 资源池维护的变量
  New = 4,      // 单独创建的变量，需要主动释放。
  // Malloc = 8,   // 单独创建的变量，需要主动释放。
  // GC = 0x10,       // gc, 未实现。

  Share = 0x10000,    // 和另一个对象共享，转变成引用计数了。
};

struct ShareCounter {
  void AddRef() {
    m_ref++;
  }
  int Release() {
    m_ref--;
    return m_ref;
  }
  std::atomic_int m_ref = 0;
};

// 1. 根据LifeCycleType自动释放对象。
// 2. 限制指针拷贝，只允许指针转移。
template<class T>
class AutoPtr {
public:
  // 允许隐式转换直接用null构造
  AutoPtr(std::nullptr_t) : m_t(nullptr) {
  }

  AutoPtr(T* t, LifeCycleType type) : m_t(t), m_type(type) {
  }

  AutoPtr(const AutoPtr& o) = delete;
  AutoPtr& operator=(const AutoPtr& o) = delete;

  // 拷贝时，只能转移，避免多个引用。
  AutoPtr(AutoPtr&& o) {
    m_t = o.m_t;
    m_type = o.m_type;
    m_share_counter = o.m_share_counter;
    
    o.m_t = nullptr;
    o.m_type = LifeCycleType::Unknown;
    o.m_share_counter = nullptr;
  }

  // 允许使用派生类构造基类
  template<class Derived>
  AutoPtr(AutoPtr<Derived>&& o) {
    static_assert(std::is_base_of<T, Derived>::value ||
                  std::is_same<T, Derived>::value,
                  "U must be derived from T or same as T");
    m_t = o.m_t;
    m_type = o.m_type;
    m_share_counter = o.m_share_counter;
    
    o.m_t = nullptr;
    o.m_type = LifeCycleType::Unknown;
    o.m_share_counter = nullptr;
  }

  // 释放指针
  ~AutoPtr() {
    destroy();
  }

  void destroy() {
    if (m_share_counter) {
      if (0 != m_share_counter->Release()) {
        m_t = nullptr;
        m_type = LifeCycleType::Unknown;
        m_share_counter = nullptr;
        return;
      }
      delete m_share_counter;
      m_share_counter = nullptr;
    }

    if (m_t) {
      if (m_type == LifeCycleType::New) {
        delete m_t;
      } else if (m_type == LifeCycleType::New) {
        ::free(m_t);
      }
      m_t = nullptr;
    }
    m_type = LifeCycleType::Unknown;
  }

  T* operator->() const { 
    return m_t;
  }
  T* get() const {
    return m_t;
  }
  operator bool() const {
    return !!m_t;
  }

  static AutoPtr<T> from_pool(T* t) {
    return AutoPtr<T>(t, LifeCycleType::Pool);
  }
  static AutoPtr<T> from_managed(T* t) {
    return AutoPtr<T>(t, LifeCycleType::Managed);
  }

  // 左值版本，调用完之后，外部参数将变成null
  template <typename U> static AutoPtr<T> take_new(U *&t) {
    static_assert(std::is_base_of<T, U>::value ||
                  std::is_same<T, U>::value,
                  "U must be derived from T or same as T");
    // static_assert(__is_base_of(T, U) || __is_same(T, U),
    //               "U must be derived from T or same as T");

    // 为了避免一个指针被引用多次，要求转换t的所有权
    T *temp = t;
    t = nullptr;
    return AutoPtr<T>(temp, LifeCycleType::New);
  }

  // 右值版本，直接传递一个new U()参数的场景。
  template <typename U> static AutoPtr<T> take_new(U *&&t) {
    static_assert(std::is_base_of<T, U>::value ||
                  std::is_same<T, U>::value,
                  "U must be derived from T or same as T");
    return AutoPtr<T>(t, LifeCycleType::New);
  }

  template <typename U> void reset(AutoPtr<U>&&o) {
    static_assert(std::is_base_of<T, U>::value ||
                  std::is_base_of<U, T>::value ||
                  std::is_same<T, U>::value,
                  "U should be derived from T or T should be derived from U or same as T");
    destroy();

    m_t = static_cast<T*>(o.m_t);
    m_type = o.m_type;
    m_share_counter = o.m_share_counter;

    o.m_t = nullptr;
    o.m_type = LifeCycleType::Unknown;
    o.m_share_counter = nullptr;
  }

  // 直接透传构造参数new
  // 使用完美转发，支持 && 类型（std::move类型的右值）的构造函数参数。
  template <typename... Args> 
  static AutoPtr<T> make_new(Args&&... args) {
    return AutoPtr<T>(new T(std::forward<Args>(args)...), LifeCycleType::New);
  }

  void share(AutoPtr<T>& o) {
    destroy();
    m_t = o.m_t;
    m_type = o.m_type;

    // 这几种类型生命周期由外部控制，转share counter没有意义
    if (m_type == LifeCycleType::Managed ||
       m_type == LifeCycleType::Pool) {
      return;
    }

    if (o.m_share_counter) {
      m_share_counter = o.m_share_counter;
      m_share_counter->AddRef();
    } else {
      m_share_counter = new ShareCounter();
      m_share_counter->AddRef();

      o.m_share_counter = m_share_counter;
      o.m_share_counter->AddRef();
    }
  }
  
public:
  T* m_t = nullptr;
  LifeCycleType m_type = LifeCycleType::Unknown;

  // 如果转换成了共享引用计数类型，会实例化这个对象。
  ShareCounter* m_share_counter = nullptr;
};

template <typename T>
void swap(AutoPtr<T>& o1, AutoPtr<T>& o2) {
  T* t = o1.m_t;
  LifeCycleType type = o1.m_type;
  ShareCounter* counter = o1.m_share_counter;

  o1.m_t = o2.m_t;
  o2.m_t = t;

  o1.m_type = o2.m_type;
  o2.m_type = type;

  o1.m_share_counter = o2.m_share_counter;
  o2.m_share_counter = counter;
}

}

// alias
template<typename T>
using A=memory::AutoPtr<T>;

}


#endif