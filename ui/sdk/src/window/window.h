#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_

#include "include/macro/uimsg.h"
#include "include/common/signalslot/signal.h"
#include "include/interface/iwindow.h"
#include "src/object/object.h"
#include "src/panel/panel.h"
#include "src/layer/windowrender.h"

#include "window_mouse_key.h"
#include "window_dpi.h"


namespace ui {
class Window;
struct CreateWindowParam;
struct IGpuCompositorWindow;

struct WindowPlatform {
  virtual ~WindowPlatform(){};

  // 用于GPU模块获取创建窗口Surface的参数。
  virtual IGpuCompositorWindow* GetGpuCompositorWindow() = 0;

  virtual void Initialize() {};
  virtual void Release() {};
  virtual bool Create(CreateWindowParam& param) = 0;
  virtual WINDOW_HANDLE GetWindowHandle() = 0;
  virtual void SetTitle(const char *title) = 0;
  virtual void Show() = 0;
  virtual float GetScaleFactor() = 0;
  virtual void GetClientRect(Rect *prect) = 0;
  virtual void GetWindowRect(Rect *prect) = 0;
  virtual void SetWindowPos(int x, int y, int w, int h, SetPositionFlags flags) = 0;
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
  // UIMSG_DM_GETDEFID( OnGetDefId )
  // UIMSG_DM_SETDEFID( OnSetDefId )
  // UIMSG_GETDESIREDSIZE( OnGetDesiredSize )
  // UIMSG_PRECREATEWINDOW( PreCreateWindow )
  UI_END_MSG_MAP_CHAIN_PARENT( Panel )

  void onRouteMessage(ui::Msg *msg);

  IWindow*   GetIWindow() { return m_pIWindow; }
  WindowRender&  GetWindowRender() { return m_window_render; }
  WindowPlatform* GetWindowPlatform() { return m_platform.get(); }

public:
  void OnMessage();
  void Create(const char *szId, const Rect* rect);
  WINDOW_HANDLE GetWindowHandle();
  void SetTitle(const char *title);
  void Show();
  float GetScaleFactor();

  bool CreateUI(const char *szId);

	void  SetGpuComposite(bool b);
	bool  IsGpuComposite();
	void  DirectComposite();

  void  Invalidate(const Rect*);
	bool  IsChildWindow();
	bool  IsWindowVisible();

  virtual bool virtualCommitReq() { return false; }  // 主要是分层窗口的实现与普通窗口不一致
  virtual void virtualInnerInitWindow();

  void Commit(IRenderTarget* pRT, const Rect* prect, int count);
  
  // Object
  void SetObjectPos( int x, int y, int cx, int cy, SetPositionFlags flags) override;

public:
  // platform回调
  void onClose();
  void onDestroy();
  void onPaint(const Rect *dirty);
  void onSize(int width, int height);
  void enterResize(bool);

private:
  // void on_paint(SkCanvas &canvas);
  // void on_erase_bkgnd(SkCanvas &canvas);
  // void swap_buffer();
  void  recursion_on_load_notify(Object* pParent);

  void onCreate(CreateWindowParam& param);

protected:
  long FinalConstruct();
  // BOOL PreCreateWindow(CREATESTRUCT *pcs) { return TRUE; }
  void onSerialize(SerializeParam *pData);
  // void OnSetDefId(IObject *pButton);
  // IObject *OnGetDefId();
  void onEraseBkgnd(IRenderTarget *);

public:
  WindowStyle& GetWindowStyle() { return m_window_style; }

public:
  // 鼠标键盘处理类
  WindowMouseKey m_mouse_key;

  // 缩放辅助类
  WindowDPI m_dpi;
  
  // 平台相关函数。
  std::unique_ptr<WindowPlatform> m_platform;

private:
  IWindow*  m_pIWindow;

  // 窗口的渲染放在这个对象里面，windowbase不负责渲染
  WindowRender m_window_render;

	WindowStyle m_window_style;
    
  int m_width = 0;
  int m_height = 0;

private:
  // 属性 ------------------------
  // 皮肤中配置的窗口标题
  std::string  m_strConfigWindowText;   
};

} // namespace ui

#endif