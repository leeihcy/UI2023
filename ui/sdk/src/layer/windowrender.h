#pragma once
#include "include/base/xmldefine.h"
//#pragma comment(lib, "UICompositor.lib")

//
// 负责窗口的渲染，决定是采用软件方式，还是硬件合成
//
// 2014.3.10
//
// 统一Graphics Render Library的使用，一个窗口不再混合使用，只能使用一种Graphics
// Render Library 否则处处要考虑创建什么类型的渲染库，搞的代码很乱，逻辑也乱
//
namespace ui {
class Object;
class Window;
struct IWindowRender;
struct IWindowCommitListener;
class Compositor;
class Layer;
struct IObjectLayerContent;
struct IListItemLayerContent;
struct SERIALIZEDATA;
struct IRenderTarget;
struct RECT;
struct Rect;

class WindowRender {
public:
  WindowRender(Window& w);
  ~WindowRender();

  IWindowRender *GetIWindowRender();

public:
#if defined(OS_WIN)
  void BindHWND(HWND hWnd);
#endif
  void OnSerialize(SERIALIZEDATA *pData);
  void OnWindowSize(unsigned int nWidth, unsigned int nHeight);

  bool CreateRenderTarget(IRenderTarget **pp);

  bool GetRequireAlphaChannel();
  void SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE eTpye);
  GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderType();
  void SetCanCommit(bool b);
  bool CanCommit();

  void SetCommitListener(IWindowCommitListener *);
  IWindowCommitListener *GetCommitListener();

  void InvalidateNow();
  void OnPaint(Rect *prcInvalid);
  Layer *CreateLayer(IObjectLayerContent *);
  Layer *CreateLayer(IListItemLayerContent *);
  void RequestInvalidate();

private:
  Compositor *get_create_compositor();

public:
  IWindowRender *m_pIWindowRender;
  GRAPHICS_RENDER_LIBRARY_TYPE m_eGRL;
  Window& m_window;

  long m_lRefCanCommit;

  // 该窗口的渲染是否需要alpha通道。不再根据graphics render lib
  // type来决定。由用户自己设置
  bool m_bNeedAlphaChannel;

private:
  Compositor *m_pCompositor;
  IWindowCommitListener *m_pCommitListener;
};

} // namespace ui