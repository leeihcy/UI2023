#ifndef _UI_BINDABLE_H_
#define _UI_BINDABLE_H_
#include <list>
#include <string>

// 业务数据属性绑定到控件
// 目前是采用单头文件直接引用的方式，不导出。
namespace ui
{
	class bindable;

	interface IBindSourceChangedListener
	{
	public:
		virtual void OnBindSourceChanged(long key, bindable* src, void* context) = 0;
	};


	class bindable
	{
	public:
		bindable()
		{

		}
		~bindable()
		{
			destroy();
		}

		struct Observer
		{
			IBindSourceChangedListener* listener;
			long key;
			void* context;
		};
		//     struct UIObserver
		//     {
		//         UIObserver(DependencyObject* listener, DependencyProperty& prop):
		//             listener(listener),
		//             prop(prop)
		//         {
		//         }
		//         DependencyObject* listener;
		//         DependencyProperty& prop;
		//     };

		enum value_type
		{
			prop_value_int,
			prop_value_string,
		};
		virtual  value_type  get_type() = 0;

	public:
		void AddObserver(IBindSourceChangedListener* listener, long key, int options, void* context)
		{
			Observer* p = find(listener, key);
			if (!p)
			{
				p = new Observer;
				p->listener = listener;
				p->key = key;
				m_list.push_back(p);
			}
			p->context = context;

			listener->OnBindSourceChanged(key, this, context);
		}
		//     void addObserver(DependencyObject* listener, DependencyProperty& prop)
		//     {
		//         UIObserver* p = new UIObserver(listener, prop);
		//         m_uiList.push_back(p);
		//     }

		void RemoveObserver(IBindSourceChangedListener* p, long key, void* pContext)
		{
			std::list<Observer*>::iterator iter = m_list.begin();
			for (; iter != m_list.end(); ++iter)
			{
				Observer* pIter = *iter;
				if (pIter->key == key && pIter->listener == p)
				{
					delete pIter;
					m_list.erase(iter);
					break;
				}
			}
		}

	protected:
		void  NotifyPropertyChanged()
		{
			std::list<Observer*>::iterator iter = m_list.begin();
			for (; iter != m_list.end(); ++iter)
			{
				Observer* pIter = *iter;

				pIter->listener->OnBindSourceChanged(
					pIter->key, this, pIter->context);
			}
		}


	private:
		void  destroy()
		{
			std::list<Observer*>::iterator iter = m_list.begin();
			for (; iter != m_list.end(); ++iter)
			{
				delete *iter;
			}
			m_list.clear();
		}
		Observer* find(IBindSourceChangedListener* p, long key)
		{
			if (!p)
				return nullptr;

			std::list<Observer*>::iterator iter = m_list.begin();
			for (; iter != m_list.end(); ++iter)
			{
				Observer* pIter = *iter;
				if (pIter->key == key && pIter->listener == p)
					return pIter;
			}
			return nullptr;
		}

	private:
		std::list<Observer*>  m_list;
		// list<UIObserver*>  m_uiList;
	};


	class bindable_int : public bindable
	{
	public:
		virtual  value_type  get_type() override {
			return prop_value_int;
		}

		bindable_int()
		{
			value = 0;
		}
		bindable_int(const bindable_int& o)
		{
			set(o.value);
		}

		bindable_int& operator=(int n)
		{
			set(n);
			return *this;
		}

		void  set(int n)
		{
			if (!pre_set(n))
				return;

			value = n;
			post_set();
		}
		int  get()
		{
			return value;
		}
		operator int()
		{
			return value;
		}
	private:
		bool pre_set(int& n)
		{
			return true;
		}
		void post_set()
		{
			NotifyPropertyChanged();
		}
	private:
		int  value;
	};


	class bindable_string_holder;
	class bindable_string : public bindable
	{
	public:
		virtual  value_type  get_type() override {
			return prop_value_string;
		}

		bindable_string()
		{
		}
		bindable_string(const bindable_string& o)
		{
			set(o.value.c_str());
		}

		bindable_string& operator=(LPCWSTR n)
		{
			set(n);
			return *this;
		}

		void  set(LPCWSTR n)
		{
			if (!pre_set(n))
				return;

			if (n)
				value = n;
			else
				value.clear();

			post_set();
		}
		const std::wstring& get()
		{
			return value;
		}
		operator LPCWSTR()
		{
			return value.c_str();
		}		
	private:
		bool pre_set(LPCWSTR& n)
		{
			return true;
		}
		void post_set()
		{
			NotifyPropertyChanged();
		}
	private:
		std::wstring  value;  // bindable_string_holder* TBD 
	};
}

#endif  // _UI_BINDABLE_H_