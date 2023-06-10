#pragma once
#include "..\wndtransmodebase.h"

namespace UI
{

class AntiWindowWrap : public WndTransModeBase
{
public:
    AntiWindowWrap ();
    ~AntiWindowWrap ();

    VIRTUAL_BEGIN_MSG_MAP(LayeredWindowWrap)
		MESSAGE_HANDLER(WM_NCPAINT, _OnNcPaint)
    END_MSG_MAP()

    virtual WINDOW_TRANSPARENT_TYPE  GetType()
    {
        return WINDOW_TRANSPARENT_TYPE_ANTIALIASING;
    }
    virtual void  UpdateRgn();
    virtual void  Init(ICustomWindow* pWnd);
	virtual void  Enable(bool b);

protected:
	LRESULT  _OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    C9Region  m_9region;
    bool      m_bEnable;
};

}