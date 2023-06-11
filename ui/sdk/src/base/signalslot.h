#ifndef SIGNALSLOT_H
#define SIGNALSLOT_H

#include <functional>
#include <list>

// 信号/槽 实现
// TODO: 1. 支持回调顺序
// TODO: 2. 统一全都支持多对多，不再区分单一连接或者无返回值模式
// TODO: 3. 多于多个slot且带返回值的模式，需要提供一个result combiner进行返回值的统一处理。
//          boost中缺省的combiner是last_value
// TODO: 4. class对象的生命周期处理，已释放的对象，怎么及时断开，weakptr?

namespace ui
{
// 默认是一个function
#define SLOT_FLAG_METHOD   0x1

// 执行顺序
#define SLOT_FLAG_ORDER_EARLY   0x10
#define SLOT_FLAG_ORDER_BEFORE  0x20
#define SLOT_FLAG_ORDER_AFTER   0x40
#define SLOT_FLAG_ORDER_LATE    0x80
#define SLOT_FLAG_ORDER_MASK    0xF0



	template <typename Function, typename... BoundArgs>
	class slot
	{
	public:
		// 函数类型不能作为参数，这里将被转换成函数指针
		slot(Function f, BoundArgs... bound_args) : 
			m_func(f), 
			m_bound_args(bound_args...)
		{
		}
		bool equal(Function f)
		{
			// if (m_flags & SLOT_FLAG_METHOD)
			//	return f == m_func && 
			// return f == m_func;
			return false;
		}
		
		template<typename... Args>
		auto invoke(Args... args)
		{
		    // Class *m_this;
			// return (m_this->*m_func)(args...);
			constexpr size_t num_bound_args = std::tuple_size<decltype(m_bound_args)>::value;
			return invoke2(std::make_index_sequence<num_bound_args>(), args...);
		}

		template<typename... Args, size_t... N>
		auto invoke2(std::index_sequence<N...>, Args... args) {
			return m_func(std::get<N>(m_bound_args)..., args...);
		}

	private:
	    int m_flags = 0;

		// 直接使用函数类型会报错，改为函数指针。
		Function* m_func;

		// 如果是method，则第一个参数是pthis
		std::tuple<BoundArgs...> m_bound_args;
	};


	// 信号、事件封装。
	// 为了使代码更简洁，暂不去支持返回值模板。所有的回调函数都用void返回值
	// 支持多连接的版本multi connections
	template <typename Function, typename... BoundArgs>
	class signal
	{
	public:
		~signal()
		{
			disconnect_all();
		}
		// 提交
		template<typename... UnboundArgs>
		void emit(UnboundArgs... args)
		{
			auto iter = m_connections.begin();
			decltype(iter) iterNext;

			for (; iter != m_connections.end();)
			{
				// 避免在回调中移除自己
				iterNext = iter;
				++iterNext;

				(*iter).invoke(args...);

				iter = iterNext;
			}
		}

		// callback，在每次通过后，判断是否需要执行下一个连接。
		// callback 返回false，立即中断执行，返回true继续执行下一个连接
/*		void emit_foreach(std::function<bool()> callback, Args... args)
		{
			auto iter = m_connections.begin();
			decltype(iter) iterNext;

			for (; iter != m_connections.end();)
			{
				// 避免在回调中移除自己
				iterNext = iter;
				++iterNext;

				(*iter)->invoke(args...);
				bool bContinue = callback();
				if (!bContinue)
					return;

				iter = iterNext;
			}
		}
*/
		/* 暂不去考虑返回值
		Return emit_return_last(Args... args)
		{
			Return ret;
			for (auto i : m_connections)
			{
				ret = i->invoke(args...);
			}

			return ret;
		}
		*/

		// 连接一个成员函数
		/*template <typename Class>
		void connect(Class *pThis, Return (Class::*pFunc)(Args...))
		{
			slot_base<Return, Args...> *p = new method_slot<Return, Class, Args...>(pThis, pFunc);
			m_connections.push_back(p);
		}*/

		void connect(Function f, BoundArgs... bound_args)
		{
			m_connections.push_back(slot<Function, BoundArgs...>(f, bound_args...));						
		}

		// 连接一个全局函数
		/*void connect(Return (*pFunc)(Args...))
		{
			slot_base<Return, Args...> *p = new function_slot<Return, Args...>(pFunc);
			m_connections.push_back(p);
		}*/



		// 断开一个成员函数
		/*template <typename Class>
		void disconnect(Class *pThis, Return (Class::*pFunc)(Args...))
		{
			auto iter = m_connections.begin();
			for (; iter != m_connections.end(); ++iter)
			{
				auto slot = *iter;
				if (slot->type() != slot_method)
					continue;

				if (static_cast<method_slot<Return, Class, Args...> *>(slot)
						->equal(pThis, pFunc))
				{
					(*iter)->release();
					m_connections.erase(iter);
					break;
				}
			}
		}

		// 断开一个对象下的所有连接
		template <typename Class>
		void disconnect(Class *pThis)
		{
			auto iter = m_connections.begin();
			for (; iter != m_connections.end();)
			{
				auto slot = *iter;
				if (slot->type() == slot_method)
				{
					if (static_cast<method_slot<Return, Class, Args...> *>(slot)
							->equal(pThis))
					{
						delete (*iter);
						iter = m_connections.erase(iter);
						continue;
					}
				}

				++iter;
			}
		}

		// 断开一个全局函数
		void disconnect(Return (*pFunc)(Args...))
		{
			auto iter = m_connections.begin();
			for (; iter != m_connections.end(); ++iter)
			{
				auto slot = *iter;
				if (slot->type() != slot_function)
					continue;

				if (static_cast<function_slot<Return, Args...> *>(slot)
						->equal(pFunc))
				{
					delete (*iter);
					m_connections.erase(iter);
					break;
				}
			}
		}
*/
		void disconnect_all()
		{
//			for (auto i : m_connections)
//			{
//				delete i;
//			}
//			m_connections.clear();
		}

	protected:
		std::list<slot<Function, BoundArgs...> > m_connections;
	};

}

#endif
