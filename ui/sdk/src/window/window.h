#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_

#include "include/interface/iobject.h"
#include "include/macro/msg.h"
#include "include/macro/uimsg.h"
#include "include/common/signalslot/signal.h"
#include "include/interface/iwindow.h"
#include "src/object/message.h"
#include "src/object/object.h"
#include "src/panel/panel.h"
#include "src/layer/windowrender.h"

#include "window_mouse_key.h"
#include "window_dpi.h"
#include <cstddef>
#include <memory>


namespace ui {
class Window;
class ResourceBundle;
struct CreateWindowParam;
struct IGpuCompositorWindow;

struct WindowPlatform {
  virtual ~WindowPlatform(){};

  // 用于GPU模块获取创建窗口Surface的参数。
  virtual IGpuCompositorWindow* GetGpuCompositorWindow() = 0;

  virtual void Initialize() {};
  virtual void Release() {};
  virtual bool Create(CreateWindowParam& param) = 0;
  virtual void PostCreate() { }

  virtual WINDOW_HANDLE GetWindowHandle() = 0;
  virtual void SetTitle(const char *title) = 0;

  virtual bool IsWindowVisible() = 0;
  virtual void Show(bool active=true) = 0;
  virtual void Hide() = 0;
  
  // 返回窗口像素大小。
  virtual void GetClientRect(Rect *prect) = 0;
  virtual void GetWindowRect(Rect *prect) = 0;

  // 返回非客户区的区域位置。
  virtual void UpdateNonClientRegion(Rect* region) = 0;
  // 获取当前窗口所有屏幕的工作区域范围
  virtual void GetMonitorWorkArea(Rect* rect) = 0;
  virtual void SetWindowPos(int x, int y, int w, int h, SetPositionFlags flags) = 0;
  // virtual void Invalidate(const Rect* prect) = 0;
  virtual bool IsChildWindow() = 0;
  virtual void Commit2(const FrameBuffer& fb, const RectRegion &dirty_region_px) = 0;
};

// Window不再继承自控件
class Window : public Message {
public:
  Window(IWindow *p);
  ~Window();

  void onRouteMessage(ui::Msg *msg);

  IWindow*   GetIWindow() { return m_pIWindow; }
  WindowRender&  GetWindowRender() { return m_window_render; }
  WindowPlatform* GetWindowPlatform() { return m_platform.get(); }
  RootObject& GetRootObject();
  ResourceBundle& GetResource() { return *m_resource; }
  Application& GetApplication();

public:
  void OnMessage();
  void Create(const char *szId, const Rect* rect);
  WINDOW_HANDLE GetWindowHandle();
  void SetTitle(const char *title);
  void Show();
  void Hide();
  bool IsVisible();

  float GetScaleFactor();
  Size GetDesiredSize();

	void  SetHardwareComposite(bool b);
	bool  IsHardwareComposite();
	void  DirectComposite();

	bool  IsChildWindow();
	bool  IsWindowVisible();

  // virtual bool virtualCommitReq() { return false; }  // 主要是分层窗口的实现与普通窗口不一致
  // virtual void virtualInnerInitWindow();

  // Object
  void SetWindowPos(int x, int y, int cx, int cy, SetPositionFlags flags);

  void SetMouseCapture(Object*);
  void ReleaseMouseCapture(Object*);

public:
  // platform回调
  void onClose();
  void onDestroy();
  void onPaint(const Rect *commit_rect_px);
  void onSize(int width_px, int height_px);
  void enterResize(bool);

private:
  // prop

private:
  void recursion_on_load_notify(Object* pParent);

  void postCreate(CreateWindowParam& param);

protected:
  void FinalConstruct(FinalConstructMessage *msg);
  void onSerialize(SerializeParam *pData);
  
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
  IWindow*  m_pIWindow;  // not null
  ResourceBundle* m_resource = nullptr;  // not null

  // 作为window <--> object的桥梁
  IRootObjectPtr m_root;

  // 窗口的渲染放在这个对象里面，windowbase不负责渲染
  WindowRender m_window_render;

	WindowStyle m_window_style;
    
  int m_window_width_px = 0;
  int m_window_height_px = 0;

private:
  // 属性 ------------------------
  // 皮肤中配置的窗口标题
  std::string  m_strConfigWindowText;  
};

} // namespace ui

#endif