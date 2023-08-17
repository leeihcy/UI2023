#ifndef _WEAKPTR_WEAKPTR_H_
#define _WEAKPTR_WEAKPTR_H_

#include <memory>
#include <atomic>

namespace ui
{
template<typename T>
class weak_ptr;

// 所有weakptr中将引用该对象，保存着对象T是否析构。
class weakptr_data {
public:
    void kill() {
        m_live = false;
    }
    bool is_lived() const {
        return m_live;
    }
public:
    std::atomic<bool> m_live = true;
};

// 放在对象T中，作为成员变量。当对象T析构时也触发该对象的析构，
// 将weakptr_data中的live标志设为false
// 示例：
// class Math {
//   weakptr_factory<Math> m_weakptr_factory = {this};    
// }
//
template<typename T>
class weakptr_factory
{
public:
    weakptr_factory(T* ptr) : 
        m_ptr(ptr), m_data(new weakptr_data()) {
    }
    ~weakptr_factory() {
        m_data->kill();
    }
    weak_ptr<T> get() const {
        return weak_ptr<T>(m_ptr, m_data);
    }
private:
    T* m_ptr;
    std::shared_ptr<weakptr_data> m_data;
};

// 用于控制对象的生命周期
// 当对象已释放时，可通过 bool 判断出来，避免出现野指针崩溃。
template<typename T>
class weak_ptr {
public:
    operator bool() {
        return m_ptr && m_data && m_data->is_lived();
    }
    T* operator->() {
        return m_ptr;
    }
    T& operator*() {
        return *m_ptr;
    }
    T* get() {
        return m_ptr;
    }

    weak_ptr() {
    }
    ~weak_ptr() {
        m_ptr = nullptr;
        m_data.reset();
    }
    weak_ptr(const weak_ptr<T>& copy) {
        m_ptr = copy.m_ptr;
        m_data = copy.m_data;
    }
    weak_ptr<T>& operator=(const weak_ptr<T>& copy) {
        m_data = copy.m_data;
        m_ptr = copy.m_ptr;
        return *this;
    }

    weak_ptr(weak_ptr<T>&& copy) {
        m_ptr = copy.m_ptr;
        copy.m_ptr = nullptr;
        m_data.swap(copy.m_data);
    }
    weak_ptr<T>& operator=(weak_ptr<T>&& copy) {
        m_ptr = copy.m_ptr;
        copy.m_ptr = nullptr;
        m_data.swap(copy.m_data);
        return *this;
    }
  
protected:
    weak_ptr(T* ptr, std::shared_ptr<weakptr_data> data) {
        // printf(" weak_ptr(T, shared_ptr) 0x%p\n", this);

        m_ptr = ptr;
        m_data = data;
    }

private:
    T* m_ptr = nullptr;
    std::shared_ptr<weakptr_data> m_data;

    friend class weakptr_factory<T>;
};

}

#endif