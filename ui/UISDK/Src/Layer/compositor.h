#pragma once
#include "layer.h"
namespace UI
{
    class UIApplication;
}

#define MSG_INVALIDATE  161311307
#define MSG_ASYNCTASK   175021413

namespace UI
{
class Compositor
{
public:
	Compositor();
	virtual ~Compositor();

    void  SetUIApplication(UIApplication*);
    void  SetWindowRender(WindowRender*);
    UIApplication*  GetUIApplication();
    bool  CreateRenderTarget(IRenderTarget** pp);

	Layer*  CreateLayer();
    void  SetRootLayer(Layer* pChanged);
	Layer*  GetRootLayer();
    void  BindHWND(HWND);
    HWND  GetHWND();
    void  RequestInvalidate();
    void  DoInvalidate();
    void  Commit(const RectArray& arrDirtyInWindow);

    virtual void  UpdateDirty(__out_opt  RectArray& arrDirtyInWindow) = 0;
    virtual void  Resize(uint nWidth, uint nSize) = 0;

protected:
    virtual Layer*  virtualCreateLayer() = 0;
    virtual void  virtualBindHWND(HWND) = 0;
    virtual void  virtualCommit(const RectArray& arrDirtyInWindow) = 0;

protected:
    UI::UIApplication*  m_pUIApp;
    WindowRender*  m_pWindowRender;

	Layer*  m_pRootLayer;

    HWND  m_hWnd;
private:
    // 限制刷新时postmessage的次数。如果已经post了一个，就不再post
    long  m_lPostInvalidateMsgRef;  
};

}