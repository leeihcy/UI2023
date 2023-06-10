#pragma once
namespace UI
{


class CTimerProcThunk
{
public:
    CStdCallThunk thunk;

    BOOL Init(TIMERPROC proc, void* pThis)
    {
        return thunk.Init((DWORD_PTR)proc, pThis);
    }
    TIMERPROC  GetTIMERPROC()
    {
        return (TIMERPROC)thunk.GetCodeAddress();
    }
};


// 一次性定时器，
class CTimeout
{
public:
    CTimeout();
    ~CTimeout();

    void  Start(int nElapse);
    void  Cancel();

    void  Init(ITimeoutCallback* pCallback);
    void  SetParam(long lId, WPARAM wParam = 0, LPARAM lParam = 0);

private:
    static void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    void   OnTimer();

private:
    CTimerProcThunk   m_thunk;
    UINT_PTR  m_nTimerId;

    ITimeoutCallback*  m_pCallback;
    long  m_lId;
    WPARAM  m_wParam;
    LPARAM  m_lParam;
};

}