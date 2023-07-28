#include "stdafx.h"
#include "giftimermanager.h"
#include "src/application/uiapplication.h"

using namespace ui;
GifTimerManager::GifTimerManager()
{
	m_pUIApplication = nullptr;
	m_hTimer = nullptr;
}

GifTimerManager::~GifTimerManager()
{
	if (m_hTimer)
	{
		EndTimer();
	}
}

void GifTimerManager::Init(Application* pUIApp)
{
	m_pUIApplication = pUIApp;
}

void  GifTimerManager::StartTimer()
{
	if (m_hTimer)
		return;

	int nPeriod = 30;  // 计时器周期

	m_hTimer = CreateWaitableTimer(nullptr, FALSE, nullptr);

	IWaitForHandlesMgr* pMgr = m_pUIApplication->GetWaitForHandlesMgr();
	if (pMgr)
		pMgr->AddHandle(m_hTimer, static_cast<IWaitForHandleCallback*>(this), 0);

	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart = -1000*10*nPeriod;   // 第一次响应延迟时间。负值表示一个相对的时间，代表以100纳秒为单位的相对时间，（如从现在起的5ms，则设置为-50000）
	SetWaitableTimer(m_hTimer, &liDueTime, nPeriod, nullptr, nullptr, 0);
}

void  GifTimerManager::EndTimer()
{
	if (m_hTimer)
	{
		IWaitForHandlesMgr* pMgr = m_pUIApplication->GetWaitForHandlesMgr();
		if (pMgr)
			pMgr->RemoveHandle(m_hTimer);

		// CancelWaitableTimer 
		CloseHandle(m_hTimer);
		m_hTimer = nullptr;
	}
}

void GifTimerManager::OnWaitForHandleObjectCallback( HANDLE, LPARAM )
{
	check_timer();
}

// 检查列表中的timer item，如果到达响应时间点，则触发on_timer
void GifTimerManager::check_timer()
{
	_MyIter iter = m_listTimerItem.begin();
	for (; iter != m_listTimerItem.end();)
	{
		Gif_TimerItem* pItem = *iter;
		if (pItem->get_remain() <= 0)
		{
			this->on_item_timer(pItem);

			if (pItem->nRepeat != -1 && 0 == --pItem->nRepeat)
			{
				// 用完了循环次数，删除
				delete pItem;
				pItem = nullptr;

				iter = m_listTimerItem.erase(iter);
				continue;	
			}
			else
			{
				// 重新计算下一次的响应时间
				pItem->update_repeat();    // gifimage在on_timer响应中只修改了wait值，在这里对start值做更新，
			}
		}
		iter++;
	}
}


void  GifTimerManager::on_item_timer(Gif_TimerItem* pItem)
{
	GifImageRender* pImageDrawItem = (GifImageRender*)pItem->pData;
	if (nullptr == pImageDrawItem)
		return;

	pImageDrawItem->on_tick(pItem);
}

void  GifTimerManager::AddItem(Gif_TimerItem*  pItem)
{
	if (!pItem)
		return;

	_MyIter iter = m_listTimerItem.begin();
	_MyIter iterEnd = m_listTimerItem.end();
	for (; iter != iterEnd; iter++)
	{
		if ((*iter)->nId == pItem->nId)
			return;
	}

	Gif_TimerItem* pNewItem = new Gif_TimerItem;
	memcpy(pNewItem, pItem, sizeof(Gif_TimerItem));
	m_listTimerItem.push_back(pNewItem);

//	update_wait_time();
	if (!m_hTimer)
	{
		StartTimer();
	}
	on_item_timer(pNewItem);  // 立即刷新第一帧，否则会先显示一下空白， 如粘贴gif时
}


//
//	由其它线程（主线程）调用，转发给gif动画线程来处理 --> 
//
void GifTimerManager::RemoveItem(int id)
{
	_MyIter iter = m_listTimerItem.begin();
	for (; iter != m_listTimerItem.end();)
	{
		Gif_TimerItem* pItem = (*iter);
		if (nullptr == pItem)
		{
			iter ++;
			continue;
		}

		if (pItem->nId == id)
		{
			delete pItem;
			pItem = nullptr;
			iter = m_listTimerItem.erase(iter);

			if (0 == m_listTimerItem.size())
			{
				EndTimer();
			}
			break;
		}
		else
		{
			iter ++ ;
		}
	}
}

