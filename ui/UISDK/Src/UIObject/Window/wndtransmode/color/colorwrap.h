#pragma once
#include "..\wndtransmodebase.h"

namespace UI
{

class ColorMaskWindowWrap : public WndTransModeBase
{
public:
    ColorMaskWindowWrap();
    ~ColorMaskWindowWrap();

    virtual WINDOW_TRANSPARENT_TYPE  GetType()
    {
        return WINDOW_TRANSPARENT_TYPE_MASKCOLOR;
    }
    virtual void  UpdateRgn();
    virtual void  Init(ICustomWindow* pWnd);
	virtual void  Enable(bool b);

public:
    void   GetRgnData(BYTE* pScan0, int nPitch, RECT* prc, vector<POINT>& vPt);

private:
    C9Region  m_9region;
    Color*  m_pColMask;   // 需要进行透明处理的颜色
};

}