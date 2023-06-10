#pragma once
#include "listctrlmkmgr.h"

namespace UI
{

class PopupListCtrlMKMgr : public SingleSelListCtrlMKMgr
{
public:
    PopupListCtrlMKMgr();

protected:
    virtual BOOL  DoProcessMessage(UIMSG* pMsg);

    void  OnMouseMove(UIMSG* pMsg);
    void  OnMouseLeave(UIMSG* pMsg);
    void  OnLButtonUp(UIMSG* pMsg);
    void  OnLButtonDown(UIMSG* pMsg);

    void  OnKeyDown(UIMSG* pMsg, bool* pbInterestMsg);
    void  OnKeyDown_down(UIMSG* pMsg);
    void  OnKeyDown_up(UIMSG* pMsg);

protected:
    POINT  m_ptLastMousePos;
};


}

