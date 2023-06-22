#ifndef SIGNALSLOT_H
#define SIGNALSLOT_H

// #include <functional>
#include <list>
#include <utility>
#include <type_traits>
#include <tuple>


#include "signalslot_internal.h"

// 信号/槽 实现
// 1. 不使用虚函数
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
    // 无模板参数
    // 将信息隐藏在子类型中，在实际使用中，通过强制类型转换为slot_data_impl进行调用。
    struct slot_data {
        using RunPtr = void (*)();
        
        // 在子类中被赋值，指向RunEntry::Run
        RunPtr m_pfn_run;
    };

    // 关键函数：
    // bound 与 unbound 在这里汇合
    // bound: SlotData
    // unbound: slot_data::m_pfn_run_entry
    template<typename , typename>
    struct run_entry;
    template<typename SlotDataImpl, typename Return, typename... UnboundArgs>
    struct run_entry<SlotDataImpl, Return(*)(UnboundArgs...)> {
        using FunctorTraits = typename SlotDataImpl::FunctorTraits;

        // 作为一个函数地址，被记录在slot_data::m_pfn_run中。
        // 在slot_base::emit中被调用
        static Return run(slot_data* base, UnboundArgs... unbound_args) {
            SlotDataImpl* data = static_cast<SlotDataImpl*>(base);

            constexpr size_t num_bound_args = std::tuple_size<
                decltype(data->m_bound_args)>::value;

            return run_impl(
                base, 
                std::make_index_sequence<num_bound_args>(), 
                unbound_args...); 
        }

        template <size_t... N>
        static Return run_impl(
            slot_data* base, 
            std::index_sequence<N...>, 
            UnboundArgs... unbound_args)
        {
            SlotDataImpl* data = static_cast<SlotDataImpl*>(base);
            return FunctorTraits::Run(
                data->m_functor,
                std::get<N>(data->m_bound_args)..., 
                unbound_args...);
            // return data->m_functor(std::get<N>(data->m_bound_args)..., unbound_args...);
        }
    };

    template<typename Functor, typename... BoundArgs>
    struct slot_data_impl : public slot_data {
        using FunctorTraits = functor_traits<Functor>;
        using Return = typename FunctorTraits::return_type;
        using UnboundFunctor = typename bind_args_helper<Functor, BoundArgs...>::unbound_functor;
        using RunEntry = run_entry<slot_data_impl<Functor, BoundArgs...>, UnboundFunctor>;
        
        slot_data_impl(Functor f, BoundArgs... bound_args)
            : m_functor(f), m_bound_args(bound_args...)
        {
            m_pfn_run = (slot_data::RunPtr)&RunEntry::run;
        }

    public:
        // 无论模板传入的是函数类型，还是函数指针，都统一转换成函数指针。
        typename functor_traits<Functor>::Functor m_functor;

        // 如果是method，则第一个参数是pthis
        std::tuple<BoundArgs...> m_bound_args;
    };


    template<typename>
    class slot_base;

    // 只记录最终调用的相关信息，用于对外暴露。
    template <typename Return, typename... UnboundArgs>
    class slot_base<Return(UnboundArgs...)> {
    public:
        slot_base(slot_data* data) : m_data(data) { }
        ~slot_base() {
            delete m_data;
        }

        Return emit(UnboundArgs... unbound_args) {
            // !!
            // 这里通过强制调用一个静态成员函数来切换到RunEntry上下文中，
            // 重新获取到bound的相关参数
            using RunPtr = Return (*)(slot_data*, UnboundArgs...);
            return ((RunPtr)m_data->m_pfn_run)(m_data, unbound_args...);
        }

        slot_data* data() const { return m_data; }
    private:
        slot_data* m_data;
    };

    // 记录原始的函数信息
    template <typename Functor, typename... BoundArgs>
    class slot : public slot_base<
            typename bind_args_helper<Functor, BoundArgs...>::unbound_function>
    {
    public:
        using Return = typename functor_traits<Functor>::return_type;
        using SlotBase = slot_base<
            typename bind_args_helper<Functor, BoundArgs...>::unbound_function>;

        // 为了能够使用构造函数的模板参数推导，这里的参数要与类的模板形参保持一致
        slot(Functor f, BoundArgs... bound_args) : 
            SlotBase(new slot_data_impl<Functor, BoundArgs...>(f, bound_args...))
        { }
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
