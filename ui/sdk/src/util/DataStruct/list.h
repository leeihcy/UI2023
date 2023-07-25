#pragma once

// ps: stl中的list存在一个问题：在遍历的过程中，如果去删除一个对象会崩溃。
//     因此重新写一个list，在遍历中删除也没有问题
template <class T>
struct UIListItem
{
	UIListItem() 
	{
		m_pPrev = m_pNext = nullptr;
	}
	UIListItem*  GetPrev()
	{
		return m_pPrev;
	}
	UIListItem*  GetNext()
	{ 
		return m_pNext; 
	}
	void  SetPrev(UIListItem* p) 
	{ 
		m_pPrev = p;
	}
	void  SetNext(UIListItem* p)
	{
		m_pNext = p;
	}

	T& operator*()
	{
		return m_data;
	}

	T  m_data;
private:
	UIListItem*  m_pPrev;
	UIListItem*  m_pNext;
};
template <class T>
class UIList
{
public:
	UIList()
	{
		m_pFirst = nullptr;
		m_pLast = nullptr;
		m_nCount = 0;
	}
	~UIList()
	{
		UIListItem<T>* p = m_pFirst;
		UIListItem<T>* pNext = nullptr;

		while (p)
		{
			pNext = p->GetNext();    
			delete p;
			p = pNext;
		}
		m_nCount = 0;
		m_pFirst = m_pLast = nullptr;
	}

	UIListItem<T>*  Find(T t)
	{
		UIListItem<T>* pItem = m_pFirst;
		while (pItem)
		{
			if (pItem->m_data == t)
				return pItem;

			pItem = pItem->GetNext();
		}
		return nullptr;
	}

	T  At(int nPos)
	{
		if (nPos < 0 || nPos >= m_nCount)
			return nullptr;

		UIListItem<T>* pItem = m_pFirst;
		int i = 0;
		while (pItem)
		{
			if (nPos == i++)
				return pItem->m_data;

			pItem = pItem->GetNext();
		}
		return (T)nullptr;
	}

	void  Remove(T t)
	{
		UIListItem<T>* pItem = Find(t);
		if (pItem)
		{
			if (pItem->GetPrev())
				pItem->GetPrev()->SetNext(pItem->GetNext());
			else
				m_pFirst = pItem->GetNext();

			if (pItem->GetNext())
				pItem->GetNext()->SetPrev(pItem->GetPrev());
			else
				m_pLast = pItem->GetPrev();

			delete pItem;
			m_nCount--;
		}
	}

	void  Add(T t)
	{
		UIListItem<T>* p = new UIListItem<T>;
		p->m_data = t;

		if (m_pLast)
		{
			m_pLast->SetNext(p);
			p->SetPrev(m_pLast);
		}
		m_pLast = p;
		if (nullptr == m_pFirst)
			m_pFirst = p;

		m_nCount++;
	}

	int  GetCount() { return m_nCount; }

	void  DELETE_ALL()
	{
		UIListItem<T>* p = m_pFirst;
		UIListItem<T>* pNext = nullptr;

		while (p)
		{
			pNext = p->GetNext();
			if (p->m_data) {
                delete p->m_data;
                p->m_data = nullptr;
            }
			delete p;
			p = pNext;
		}
		m_nCount = 0;
		m_pFirst = m_pLast = nullptr;
	}
	void  RELEASE_ALL()
	{
		UIListItem<T>* p = m_pFirst;
		UIListItem<T>* pNext = nullptr;

		while (p)
		{
			pNext = p->GetNext();
			if (p->m_data) {
                p->m_data->Release();
                p->m_data = nullptr;
            }
			delete p;
			p = pNext;
		}
		m_nCount = 0;
		m_pFirst = m_pLast = nullptr;
	}
	void  Clear()
	{
		UIListItem<T>* p = m_pFirst;
		UIListItem<T>* pNext = nullptr;

		while (p)
		{
			pNext = p->GetNext();
			delete p;
			p = pNext;
		}
		m_nCount = 0;
		m_pFirst = m_pLast = nullptr;
	}

	UIListItem<T>*  GetFirst()
	{
		return m_pFirst;
	}
	UIListItem<T>*  GetLast()
	{
		return m_pLast;
	}

private:
	UIListItem<T>*  m_pFirst;
	UIListItem<T>*  m_pLast;
	int  m_nCount;
};

