#ifndef SIGNALSLOT_H
#define SIGNALSLOT_H

// #include <functional>
#include <list>
#include <utility>
#include <type_traits>

#include "signalslot_internal.h"

// 信号/槽 实现
// 1. 不使用new/delete，只用栈对象。
// 2. 兼容普通函数与成员函数
// 3. 兼容函数指针与函数类型
//
// TODO: 1. 支持回调顺序
// TODO: 2. 统一全都支持多对多，不再区分单一连接或者无返回值模式
// TODO: 3. 多于多个slot且带返回值的模式，需要提供一个result combiner进行返回值的统一处理。
//          boost中缺省的combiner是last_value
// TODO: 4. class对象的生命周期处理，已释放的对象，怎么及时断开，weakptr?

namespace ui
{
    template <typename>
    class invoke_base;

    // template <typename Return, typename... Args>
    // class invoke_base<Return(Args...)> {
    // public:
    //     virtual Return invoke(Args... args) = 0;
    // };

    template <
        typename Functor,
        typename... BoundArgs>
    class slot 
    // : 
    //     public invoke_base<
    //         bind_args_helper<Functor, BoundArgs>::unbound_functor>
    {
        using Return = typename functor_traits<Functor>::return_type;
	
    public:
        // 为了能够使用构造函数的模板参数推导，这里的参数要与类的模板形参保持一致
        slot(Functor f, BoundArgs... bound_args) : 
            m_functor(f), m_bound_args(bound_args...)
        {
        }

        // Return invoke(bind_args_helper<Functor, BoundArgs>::unbound_args::types... unbound_args) override
        template<typename... UnboundArgs>
        Return invoke(UnboundArgs... unbound_args) 
        {
            constexpr size_t num_bound_args = std::tuple_size<decltype(m_bound_args)>::value;
            return _invoke(std::make_index_sequence<num_bound_args>(), unbound_args...); 
        }

    private:
        template <size_t... N, typename... Args>
        auto _invoke(std::index_sequence<N...>, Args... args)
        {
            return functor_traits<Functor>::Invoke(
                m_functor, std::get<N>(m_bound_args)..., args...);
        }

    private:
        int m_flags = 0;

        // 无论模板传入的是函数类型，还是函数指针，都统一转换成函数指针。
        typename functor_traits<Functor>::Functor m_functor;

        // 如果是method，则第一个参数是pthis
        std::tuple<BoundArgs...> m_bound_args;
    };

#if 0
    template<bool has_return_type, typename Function>	
	struct emitor;

	template<typename Function>
	struct emitor<true, Function> {
		using ReturnType = typename function_traits<Function>::return_type;

		template<typename... UnboundArgs>
		ReturnType operator()(UnboundArgs... args) {
         	/*auto iter = m_connections.begin();
			decltype(iter) iterNext;
            
            ReturnType ret;
			for (; iter != m_connections.end();)
			{
				// 避免在回调中移除自己
				iterNext = iter;
				++iterNext;

				ret = (*iter).invoke(args...);

				iter = iterNext;
			}
			return ret;*/
            std::thread t;
			return 1;
		}
	};
	template<typename Function>
	struct emitor<false, Function> {
		template<typename... UnboundArgs>
		void operator()(UnboundArgs... args) {

		}
	};

	
#endif

#if 0
    // 信号、事件封装。
	// 为了使代码更简洁，暂不去支持返回值模板。所有的回调函数都用void返回值
	// 支持多连接的版本multi connections
	template <typename FunctionWithUnboundArgs>
	class signal
	{
		using Return = typename function_traits<FunctionWithUnboundArgs>::return_type;
	public:
		~signal()
		{
		}

		template<typename Functor, typename... BoundArgs>
		void connect(Functor f, BoundArgs... bound_args)
		{
			m_connections.push_back(slot<Functor, BoundArgs...>(f, bound_args...));						
		}

		// 提交
		// template< typename... UnboundArgs>
	    // Return emit(UnboundArgs... args) 
		// {
		// 	constexpr bool is_void=function_traits<Function>::is_void;

		// 	// 当ReturnType是void时，可以直接return void
		// 	return emitor<!is_void, Function>()(args...);
		// }

	protected:
		std::list<slot<Function, BoundArgs...> > m_connections;
	};
    #endif
}

#endif
