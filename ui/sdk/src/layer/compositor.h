#pragma once
#include "layer.h"
namespace ui
{
    class Application;
}

#define MSG_INVALIDATE  161311307
#define MSG_ASYNCTASK   175021413

namespace ui
{
class Compositor
{
public:
	Compositor();
	virtual ~Compositor();

    void  SetUIApplication(Application*);
    void  SetWindowRender(WindowRender*);
    Application*  GetUIApplication();
    bool  CreateRenderTarget(IRenderTarget** pp);

	Layer*  CreateLayer();
    void  SetRootLayer(Layer* pChanged);
	Layer*  GetRootLayer();
#if defined(OS_WIN)
    void  BindHWND(HWND);
    HWND  GetHWND();
#endif
    void  RequestInvalidate();
    void  DoInvalidate();
    void  Commit(const RectArray& arrDirtyInWindow);

    virtual void  UpdateDirty(RectArray& arrDirtyInWindow) = 0;
    virtual void  Resize(uint nWidth, uint nSize) = 0;

protected:
    virtual Layer*  virtualCreateLayer() = 0;
    virtual void  virtualBindHWND(HWND) = 0;
    virtual void  virtualCommit(const RectArray& arrDirtyInWindow) = 0;

protected:
    Application*  m_pUIApp;

	Layer*  m_pRootLayer;
#if defined(OS_WIN)
    HWND  m_hWnd;
    WindowRender*  m_pWindowRender;
#endif

private:
    // 限制刷新时postmessage的次数。如果已经post了一个，就不再post
    long  m_lPostInvalidateMsgRef;  
};

}