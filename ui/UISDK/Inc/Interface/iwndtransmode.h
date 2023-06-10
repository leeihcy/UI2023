#ifndef _UI_IWINDOW_TRANSPARENT_MODE_H_
#define _UI_IWINDOW_TRANSPARENT_MODE_H_

namespace UI
{
interface ICustomWindow;

interface IWindowTransparent
{
    virtual BOOL  ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) = 0;
    virtual BOOL  ProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook) = 0;

    virtual WINDOW_TRANSPARENT_TYPE  GetType() = 0;
    virtual bool  RequireAlphaChannel() = 0;
    virtual void  Init(ICustomWindow* pWnd) = 0;
    virtual void  Release() = 0;

    virtual void  Enable(bool b) = 0;
    virtual void  UpdateRgn() = 0;
    virtual bool  Commit() = 0;
};

class  AeroWindowWrap;
interface __declspec(uuid("2D186EE2-267B-474D-9AF7-644D0DC73AF8"))
UIAPI IAeroWindowWrap
{
public:
    IAeroWindowWrap(AeroWindowWrap*  p);

    AERO_MODE  GetAeroMode();
    void  GetBlurRegion(CRegion4* pregion);
    void  UpdateRgn();
	void  SetTransparentMargins(RECT* prcMargin);

    AeroWindowWrap*  m_pImpl;
};


class  LayeredWindowWrap;
interface __declspec(uuid("DCBB2201-67EB-4081-B64B-80066BC3B29C"))
UIAPI ILayeredWindowWrap
{
public:
    ILayeredWindowWrap(LayeredWindowWrap*  p);
    LayeredWindowWrap*  m_pImpl;
};



WINDOW_TRANSPARENT_TYPE  GetTransparentModeTypeFromAttr(IMapAttribute* p);
WINDOW_TRANSPARENT_TYPE  GetTransparentModeTypeFromAttr(const TCHAR* szText);
IWindowTransparent*  CreateTransparentModeByType(WINDOW_TRANSPARENT_TYPE eType);

}

#endif // _UI_IWINDOW_TRANSPARENT_MODE_H_