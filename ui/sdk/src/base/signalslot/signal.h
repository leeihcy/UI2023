#ifndef _SIGNALSLOT_SIGNAL_H_
#define _SIGNALSLOT_SIGNAL_H_

#include <vector>
#include "slot.h"

namespace ui
{
    template<typename>	
	struct emitor;
    template <typename>
	class signal;


	template<typename Return, typename... Args>
	struct emitor<Return(Args...)> {
		static Return emit(
            slot_base<Return(Args...)>** s, 
            int count, 
            Args... args) 
        {
            // 默认返回最后一个调用结果。
            Return r;
         	for (int i = 0; i < count; i++) {
                r = s[i]->emit(args...);
            }
            return r;
		}

        // 由combiner收集调用结果
        template<typename Combiner>
        static void emit_with_combiner(Combiner* combiner,
            slot_base<Return(Args...)>** s, int count, Args... args) 
        {
            if (!combiner) {
                return;
            }
         	for (int i = 0; i < count; i++) {
                combiner->feed(s[i]->emit(args...));
            }
		}
	};
	template<typename... Args>
	struct emitor<void(Args...)> {
		static void emit(slot_base<void(Args...)>** s, int count, Args... args) 
        {
            for (int i = 0; i < count; i++) {
                s[i]->emit(args...);
            }
		}
	};

    // signal
	template <
        typename Return, 
        typename... UnboundArgs>
	class signal<Return(UnboundArgs...)>
	{
        using SlotBase = slot_base<Return(UnboundArgs...)>;
	public:
		~signal()
		{
            for (auto i : m_slots) {
                delete i;
            }
		}

		template<typename Functor, typename... BoundArgs>
		void connect(Functor f, BoundArgs... bound_args)
		{
			m_slots.push_back(new slot<Functor, BoundArgs...>(f, bound_args...));						
		}

        Return emit(UnboundArgs... args) 
		{
            return emitor<Return(UnboundArgs...)>::emit(
                &m_slots[0], 
                m_slots.size(),
                args...);
		}

        // 将所有所回值交给外部处理处理，Combiner对象由外部传递，只需要满足
        // 具备成员函数 void feed(Return) 即可。不一定非要是模板类。
        /*
            template<typename Return>
            struct combiner_last {
                void hit(Return temp) {
                    m_value = temp;
                }
                std::optional<Return> m_value;
            };
        */
        template<typename Combiner>
        void emit_with_combiner(Combiner* return_combiner, UnboundArgs... args) 
		{
            static_assert(!std::is_void_v<Return>);

            emitor<Return(UnboundArgs...)>::emit_with_combiner(
                return_combiner,
                &m_slots[0], 
                m_slots.size(),
                args...);
		}

	protected:
		std::vector<SlotBase* > m_slots;
	};
}

#endif
