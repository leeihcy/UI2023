#ifndef _SIGNALSLOT_SLOT_H_
#define _SIGNALSLOT_SLOT_H_

#include <tuple>
#include <type_traits>
#include <utility>

#include "slot_functor.h"

// 术语：
// slot:     一个回调函数，包含绑定的参数列表
// signal:   一组同一函数类型的回调函数，相当于slot数组。
// function: 函数类型 void()
// functor:  函数指针 void(*)()
// combiner: connect多个slot时，多个返回值的合并处理对象
//
// 实现目标
// 1. 不使用虚函数
// 2. 兼容普通函数与成员函数
// 3. 兼容函数指针与函数类型
// 4. 多于多个slot且带返回值的模式，提供一个combiner进行返回值的统一处理。
// 5. signal默认支持多个slot
//
// TODO: 1. 支持回调顺序
// TODO: 2. class对象的生命周期处理，已释放的对象，怎么及时断开，weakptr?
// TODO: 3. 引用类型、&&类型、const类型兼容
// TODO: 4. 支持lambda
//
namespace ui {
template <typename, typename> struct run_helper;
template <typename> class slot;

// 无模板参数
// 将信息隐藏在子类型中，在实际使用中，通过强制类型转换为slot_data_impl进行调用。
struct slot_data {
  using RunPtr = void (*)();
  using Deletor = void (*)(const slot_data *);

  void delete_() { m_pfn_delete(this); }

  // 在子类中被赋值，指向RunHelper::Run
  RunPtr m_pfn_run = nullptr;
  // 由于未使用虚函数，这里指向子类的一个销毁函数进行析构。
  Deletor m_pfn_delete = nullptr;
};

template <typename Functor, typename... BoundArgs>
struct slot_data_impl : public slot_data {
  using FunctorTraits = functor_traits<Functor>;
  using Self = slot_data_impl<Functor, BoundArgs...>;
  using UnboundFunctor =
      typename functor_bound<Functor, BoundArgs...>::UnboundFunctor;
  using RunHelper = run_helper<Self, UnboundFunctor>;

  slot_data_impl(Functor f, BoundArgs... bound_args)
      : m_functor(f), m_bound_args(bound_args...) {
    // printf(" slot_data_impl 0x%p\n", this);

    m_pfn_run = (slot_data::RunPtr)&RunHelper::run;
    m_pfn_delete = &Self::deletor;
  }
  ~slot_data_impl() {
    // printf("~slot_data_impl 0x%p\n", this);
  }

  static auto create(Functor f, BoundArgs... bound_args) {
    return new slot_data_impl(f, bound_args...);
  }
  static void deletor(const slot_data *pthis) {
    delete static_cast<const Self *>(pthis);
  }

public:
  // 无论模板传入的是函数类型，还是函数指针，都统一转换成函数指针。
  typename functor_traits<Functor>::Functor m_functor;

  // 如果是method，则第一个参数是pthis
  std::tuple<BoundArgs...> m_bound_args;
};

// 记录最终调用的参数信息，用于保存到signal中
template <typename Return, typename... UnboundArgs>
class slot<Return(UnboundArgs...)> {
  using Self = slot<Return(UnboundArgs...)>;

public:
  slot(slot_data *data) : m_data(data) {
    // printf(" slot 0x%p\n", this);
  }

  slot(Self &&s) {
    this->m_data = s.m_data;
    s.m_data = nullptr;
  }

  slot() = delete;
  slot(const slot<Return(UnboundArgs...)> &) = delete;
  Self &operator=(const Self &) = delete;

  ~slot() {
    // printf("~slot 0x%p\n", this);
    if (m_data) {
      m_data->delete_();
    }
  }

  Return emit(UnboundArgs... unbound_args) {
    // !!
    // 这里通过强制调用一个静态成员函数来切换到RunHelper上下文中，
    // 重新获取到bound的相关参数
    using RunPtr = Return (*)(slot_data *, UnboundArgs...);
    return ((RunPtr)m_data->m_pfn_run)(m_data, unbound_args...);
  }

  slot_data *data() const { return m_data; }

private:
  slot_data *m_data = nullptr;
};

// 辅助类
// 用于构造slot对象，从bound形式变成unbound形式
template <typename Functor, typename... BoundArgs>
class Slot
    : public slot<
          typename functor_bound<Functor, BoundArgs...>::UnboundFunction> {
public:
  using SlotBase =
      slot<typename functor_bound<Functor, BoundArgs...>::UnboundFunction>;

  // 为了能够使用构造函数的模板参数推导，这里的参数要与类的模板形参保持一致
  Slot(Functor f, BoundArgs... bound_args)
      : SlotBase(
            slot_data_impl<Functor, BoundArgs...>::create(f, bound_args...)) {}
};
// 辅助函数
template <typename Functor, typename... BoundArgs>
auto bind_slot(Functor f, BoundArgs... bound_args) {
  using UnboundFunction =
      typename functor_bound<Functor, BoundArgs...>::UnboundFunction;
  return slot<UnboundFunction>(
      slot_data_impl<Functor, BoundArgs...>::create(f, bound_args...));
}

// 关键函数：
// bound 与 unbound 在这里汇合
template <typename SlotDataImpl, typename Return, typename... UnboundArgs>
struct run_helper<SlotDataImpl, Return (*)(UnboundArgs...)> {
  using FunctorTraits = typename SlotDataImpl::FunctorTraits;

  // 作为一个函数地址，被记录在slot_data::m_pfn_run中。
  // 在slot_base::emit中被调用
  static Return run(slot_data *base, UnboundArgs... unbound_args) {
    SlotDataImpl *data = static_cast<SlotDataImpl *>(base);

    constexpr size_t num_bound_args =
        std::tuple_size<decltype(data->m_bound_args)>::value;

    return run_impl(base, std::make_index_sequence<num_bound_args>(),
                    unbound_args...);
  }

  template <size_t... N>
  static Return run_impl(slot_data *base, std::index_sequence<N...>,
                         UnboundArgs... unbound_args) {
    SlotDataImpl *data = static_cast<SlotDataImpl *>(base);
    return FunctorTraits::Run(
        data->m_functor, std::get<N>(data->m_bound_args)..., unbound_args...);
  }
};
} // namespace ui

#endif
