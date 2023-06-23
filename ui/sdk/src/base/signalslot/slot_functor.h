#ifndef _SIGNALSLOT_INTERNAL_H_
#define _SIGNALSLOT_INTERNAL_H_
namespace ui
{
    // ------------------------------------------------------
    template <typename>
    struct functor_traits;

    // 简单函数
    template <typename Return, typename... Args>
    struct functor_traits<Return (*)(Args...)>
    {
        using Function = Return(Args...);
        using Functor = Return(*)(Args...);

        static Return Run(Functor functor, Args... args) {
            return functor(args...);
        }
    };

    // 兼容函数类型，让模板类更通用。
    template <typename Return, typename... Args>
    struct functor_traits<Return(Args...)>
    {
        using Function = Return(Args...);
        using Functor = Return(*)(Args...);

        static Return Run(Functor functor, Args... args) {
            return functor(args...);
        }
    };

    // 成员函数
    template <typename Return, typename Class, typename... Args>
    struct functor_traits<Return (Class::*)(Args...)>
    {
        // static constexpr bool is_void = false;
        // static constexpr bool is_method = true;
        // using ReturnType = Return;
        using Function = Return(Args...);
        using Functor = Return(Class::*)(Args...);

        static Return Run(Functor method, Class* pthis, Args... args) {
            return (pthis->*method)(args...);
        }
    };

    // ------------------------------------------------------
    // 从原函数类型中，去掉绑定的参数类型，构造未绑定的参数的函数类型。
    // N为已绑定的参数个数
    template<size_t N, typename Return, typename... Args>
    struct make_unbound;

    template<typename Return>
    struct make_unbound<0, Return> {
        using Functor = Return(*)();
        using Function = Return();
    };
    template<typename Return, typename FirstArg, typename... Args>
    struct make_unbound<0, Return, FirstArg, Args...> {
        using Functor = Return(*)(FirstArg, Args...);
        using Function = Return(FirstArg, Args...);
    };
    template<size_t N, typename Return, typename FirstArg, typename... OtherArgs>
    struct make_unbound<N, Return, FirstArg, OtherArgs...> : public make_unbound<N-1, Return, OtherArgs...> {};


    // 参数绑定处理，从 Functor + Bound 推理出 Unbound Functor
    template<typename, typename...>
    struct functor_bound;

    template<typename Return, typename... Args, typename... BoundArgs>
    struct functor_bound<Return(Args...), BoundArgs...> {
        static constexpr size_t bounds_count = sizeof...(BoundArgs);
        using UnboundFunctor = typename make_unbound<bounds_count, Return, Args...>::Functor;
        using UnboundFunction = typename make_unbound<bounds_count, Return, Args...>::Function;
    };

    template<typename Return, typename... Args, typename... BoundArgs>
    struct functor_bound<Return(*)(Args...), BoundArgs...> {
        static constexpr size_t bounds_count = sizeof...(BoundArgs);
        using UnboundFunctor = typename make_unbound<bounds_count, Return, Args...>::Functor;
        using UnboundFunction = typename make_unbound<bounds_count, Return, Args...>::Function;
    };

    template<typename Return, typename Class, typename... Args, typename... BoundArgs>
    struct functor_bound<Return(Class::*)(Args...), BoundArgs...> {
        static constexpr size_t bounds_count = sizeof...(BoundArgs);
        // 成员函数，将this作为第一个参数。
        using UnboundFunctor = typename make_unbound<bounds_count, Return, Class*, Args...>::Functor;
        using UnboundFunction = typename make_unbound<bounds_count, Return, Class*, Args...>::Function;

    };
}

#endif