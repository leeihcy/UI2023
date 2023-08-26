#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_

#include "include/macro/uimsg.h"
#include "include/common/signalslot/signal.h"
#include "include/interface/iwindow.h"
#include "src/object/object.h"
#include "src/panel/panel.h"
#include "src/layer/windowrender.h"

namespace ui {
class Window;

struct WindowPlatform {
  virtual ~WindowPlatform(){};
  virtual void Initialize() {};
  virtual void Release() {};
  virtual bool Create(const Rect &rect) = 0;
  virtual void SetTitle(const char *title) = 0;
  virtual void Show() = 0;
  virtual void GetClientRect(Rect *prect) = 0;
  virtual void GetWindowRect(Rect *prect) = 0;
  virtual void Invalidate(const Rect* prect) = 0;
  virtual bool IsChildWindow() = 0;
  virtual bool IsWindowVisible() = 0;
  virtual void Commit(IRenderTarget* pRT, const Rect* prect, int count) = 0;
};

class Window : public Panel {
public:
  Window(IWindow *p);
  ~Window();

  UI_BEGIN_MSG_MAP()
  UIMSG_ERASEBKGND( OnEraseBkgnd )
  // UIMSG_DM_GETDEFID( OnGetDefId )
  // UIMSG_DM_SETDEFID( OnSetDefId )
  // UIMSG_GETDESIREDSIZE( OnGetDesiredSize )
  UIMSG_QUERYINTERFACE(Window)
  UIMSG_SERIALIZE(OnSerialize)
  // UIMSG_PRECREATEWINDOW( PreCreateWindow )
  UI_END_MSG_MAP_CHAIN_PARENT( Panel )

  void RouteMessage(ui::Msg *msg);

  IWindow*   GetIWindow() { return m_pIWindow; }
  WindowRender&  GetWindowRender() { return m_window_render; }

public:
  void OnMessage();
  void Create(const Rect &rect);
  void SetTitle(const char *title);
  void Show();

  bool CreateUI(const char *szId);

	void  SetGpuComposite(bool b);
	bool  IsGpuComposite();
	void  DirectComposite();

  void  Invalidate(const Rect*);
	bool  IsChildWindow();
	bool  IsWindowVisible();

  virtual bool  virtualCommitReq() { return false; }  // 主要是分层窗口的实现与普通窗口不一致
  void Commit(IRenderTarget* pRT, const Rect* prect, int count);
  
public:
  // platform回调
  void onClose();
  void onDestroy();
  void onPaint(Rect *dirty);
  void onSize(int width, int height);

private:
  // void on_paint(SkCanvas &canvas);
  // void on_erase_bkgnd(SkCanvas &canvas);
  // void swap_buffer();

protected:
  long FinalConstruct();
  // BOOL PreCreateWindow(CREATESTRUCT *pcs) { return TRUE; }
  void OnSerialize(SerializeParam *pData);
  // void OnSetDefId(IObject *pButton);
  // IObject *OnGetDefId();
  void OnEraseBkgnd(IRenderTarget *);

private:
  IWindow*  m_pIWindow;

  // 平台相关函数。
  std::unique_ptr<WindowPlatform> m_platform;

  int m_width = 0;
  int m_height = 0;

  // 窗口的渲染放在这个对象里面，windowbase不负责渲染
  WindowRender m_window_render;

	WindowStyle  m_windowStyle;
	
  std::string  m_strConfigWindowText;   // 皮肤中配置的窗口标题
};

} // namespace ui

#endif