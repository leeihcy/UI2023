#pragma once

namespace ui
{

//
//  计时器辅助类，在时间到达之后，将给该对象发送WM_TIMER消息
//
class TimerHelper
{
public:
    typedef map<UINT_PTR, TimerItem> _MyMap;
    typedef map<UINT_PTR, TimerItem>::iterator _MyIter;

	static TimerHelper* GetInstance();
	TimerHelper();

	UINT_PTR  SetTimer(int nElapse, IMessage* pNotify);
	UINT_PTR  SetTimer(int nElapse, std::function<bool(UINT_PTR,TimerItem*)> func);
    UINT_PTR  SetTimerById(int nElapse, UINT_PTR nId, IMessage* pNotify);
	UINT_PTR  SetTimer(int nElapse, TimerItem* pItem);
	void  KillTimer(UINT_PTR& nTimerIdRef);
    void  KillTimerById(int nId, IMessage* pNotify);
    void  KillTimerByNotify(IMessage* pNotify);

	static VOID CALLBACK TimerProc( HWND hwnd, unsigned int uMsg, UINT_PTR idEvent,	DWORD dwTime );
	void  OnTimer(UINT_PTR idEvent);

private:
//	CStdCallThunk            m_thunk;
//	map<UINT_PTR, Message*>  m_mapTimerNotify;
	map<UINT_PTR, TimerItem> m_mapTimerItem;
};


}