#pragma once
#include "Inc\Interface\iuiinterface.h"
#include "GifImage.h"

namespace UI
{
class GifTimerManager : public IWaitForHandleCallback
{
public:
	GifTimerManager();
	~GifTimerManager();

public:
	void  Init(UIApplication*  pUIApp);
	void  AddItem(Gif_TimerItem*  pItem);
	void  RemoveItem(int nId);

protected:
	virtual void OnWaitForHandleObjectCallback(HANDLE, LPARAM);
	void  check_timer();
	void  on_item_timer(Gif_TimerItem* pItem);

protected:
	void  StartTimer();
	void  EndTimer();

protected:
	typedef  list<Gif_TimerItem*>::iterator  _MyIter;

	list<Gif_TimerItem*>   m_listTimerItem;
	UIApplication*  m_pUIApplication;
	HANDLE  m_hTimer;
};
}