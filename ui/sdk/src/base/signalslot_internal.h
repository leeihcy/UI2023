#ifndef SIGNALSLOT_INTERNAL_H
#define SIGNALSLOT_INTERNAL_H
namespace ui
{

// function: 函数类型 void()
// functor:  函数指针 void(*)()

#define SLOT_FLAG_METHOD 0x1

// 执行顺序
#define SLOT_FLAG_ORDER_EARLY 0x10
#define SLOT_FLAG_ORDER_BEFORE 0x20
#define SLOT_FLAG_ORDER_AFTER 0x40
#define SLOT_FLAG_ORDER_LATE 0x80
#define SLOT_FLAG_ORDER_MASK 0xF0

    // ------------------------------------------------------
    // 获取函数类型对应的函数指针

    template <typename>
    struct function_traits;
    
    template <typename Return, typename... Args>
    struct function_traits<Return(Args...)>
    {
        using Functor = Return (*)(Args...);
    };

    template <typename, typename>
    struct method_function_traits;

    template <typename Return, typename Class, typename... Args>
    struct method_function_traits<Class, Return(Args...)>
    {
        using Functor = Return (Class::*)(Args...);
    };

    // 获取成员函数指针，自动将BoundArgs...展开成一个ClassPtr和剩下的其它绑定参数
    // int(int, int), Math... ==>  int(Math::)(int, int)
    template <typename Function, typename ClassPtr, typename...>
    struct method_function_to_functor
    {
        // 将指针类型转换成类类型
        static_assert(std::is_pointer<ClassPtr>::value, "Expected a pointer");
        using Class = std::remove_pointer_t<ClassPtr>;

        using Functor = typename method_function_traits<Class, Function>::Functor;
    };


    // ------------------------------------------------------
    template<typename... args>
    struct type_list {
        // using types = args...;
    };

    template<size_t num, typename>
    struct type_list_drop;

    template<size_t N, typename T, typename... otherargs>
    struct type_list_drop<N, type_list<T, otherargs...> > 
    : public type_list_drop<N-1, type_list<otherargs...> > {};

    template<typename... args>
    struct type_list_drop<0, type_list<args...>> {
        using type = type_list<args...>;
    };

    // template<>
    // struct type_list_drop<0, type_list<>> {
    //     using value = type_list<>
    // }

    // ------------------------------------------------------
    // 获取函数指针的各种属性
    template <typename>
    struct functor_traits;
    // 简单函数
    template <typename Return, typename... Args>
    struct functor_traits<Return (*)(Args...)>
    {
        static constexpr bool is_void = false;
        static constexpr bool is_method = false;
        using return_type = Return;
        using Function = Return(Args...);
        using Functor = Return (*)(Args...);
        using args_list = type_list<Args...>;

        template <typename Functor>
        static Return Invoke(Functor functor, Args... args) {
            return functor(args...);
        }
    };

    // 兼容函数类型，让模板类更通用。
    template <typename Return, typename... Args>
    struct functor_traits<Return(Args...)>
    {
        static constexpr bool is_void = false;
        static constexpr bool is_method = false;
        using return_type = Return;
        using Function = Return(Args...);
        using Functor = Return (*)(Args...);
        using args_list = type_list<Args...>;

        template <typename Functor>
        static Return Invoke(Functor functor, Args... args) {
            return functor(args...);
        }
    };

    template <typename... Args>
    struct functor_traits<void (*)(Args...)>
    {
        static constexpr bool is_void = true;
        static constexpr bool is_method = false;
        using return_type = void;
        using Function = void(Args...);
        using Functor = void (*)(Args...);
        using args_list = type_list<Args...>;

        template <typename Functor>
        static void Invoke(Functor functor, Args... args) {
            functor(args...);
        }
    };

    // 兼容函数类型，让模板类更通用。
    template <typename... Args>
    struct functor_traits<void(Args...)>
    {
        static constexpr bool is_void = true;
        static constexpr bool is_method = false;
        using return_type = void;
        using Function = void(Args...);
        using Functor = void (*)(Args...);
        using args_list = type_list<Args...>;

        template <typename Functor>
        static void Invoke(Functor functor, Args... args) {
            functor(args...);
        }
    };

    // 成员函数
    template <typename Return, typename Class, typename... Args>
    struct functor_traits<Return (Class::*)(Args...)>
    {
        static constexpr bool is_void = false;
        static constexpr bool is_method = true;
        using return_type = Return;
        using Function = Return(Args...);
        using Functor = Return (Class::*)(Args...);
        using args_list = type_list<Args...>;

        template <typename Method>
        static Return Invoke(Method method, Class* pthis, Args... args) {
            return (pthis->*method)(args...);
        }
    };
    template <typename Class, typename... Args>
    struct functor_traits<void (Class::*)(Args...)>
    {
        static constexpr bool is_void = true;
        static constexpr bool is_method = true;
        using return_type = void;
        using Function = void(Args...);
        using Functor = void (Class::*)(Args...);
        using args_list = type_list<Args...>;

        template <typename Method>
        static void Invoke(Method method, Class* pthis, Args... args) {
            (pthis->*method)(args...);
        }
    };


    // ------------------------------------------------------

    template<typename, typename>
    struct type_list_unref;

    template<typename Return , typename... Args>
    struct type_list_unref<Return, type_list<Args...>> {
        using Functor = Return(*)(Args...);
        using Function = Return(Args...);
    };

    // 参数绑定的一些处理
    template<typename Functor, typename... BoundArgs>
    struct bind_args_helper {
        static constexpr size_t bounds_count = sizeof...(BoundArgs);

        using return_type = typename functor_traits<Functor>::return_type;
        using args = typename functor_traits<Functor>::args_list;
        using unbound_args = typename type_list_drop<bounds_count, args>::type;
        using unbound_functor = typename type_list_unref<return_type, unbound_args>::Functor;
    };

}

#endif