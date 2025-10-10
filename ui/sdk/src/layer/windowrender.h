#ifndef _UI_SDK_SRC_LAYER_WINDOWRENDER_H_
#define _UI_SDK_SRC_LAYER_WINDOWRENDER_H_

#include <memory>
#include <map>
#include "layer_sync_op.h"
#include "include/interface/iwindow.h"
#include "include/macro/xmldefine.h"
#include "include/util/rect_region.h"
#include "src/layer/layer_sync.h"

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
class Layer;
struct IObjectLayerContent;
struct IListItemLayerContent;
struct SerializeParam;
struct IRenderTarget;
struct Rect;
struct Rect;
class WindowRenderRT;
struct IGpuCompositorWindow;
struct IGpuCompositor;
class Application;
class GpuLayerCommitContext;


class WindowRender {
public:
  WindowRender(Window& w);
  ~WindowRender();

  IWindowRender *GetIWindowRender();
  Application& GetUIApplication();

public:
  void AddInvalidateRect(const Rect*);
  void Paint(const Rect *commit_rect = nullptr);
  void DirectCommit(const DirtyRegion& dirty_region);
   
public:
  void onWindowCreated();

  void OnSerialize(SerializeParam *pData);
  void OnClientSize(unsigned int nWidth, unsigned int nHeight);

  bool CreateRenderTarget(IRenderTarget **pp);

  bool GetRequireAlphaChannel();
  void SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE type);
  GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderType();

  void SetCanCommit(bool b);
  bool CanCommit();
  void SoftwareCommit(IRenderTarget* pRT, const RectRegion & dirty_region_px);
  
  Layer *CreateLayer(IObjectLayerContent *);
  Layer *CreateLayer(IListItemLayerContent *);
  Layer* GetRootLayer();

  void RequestInvalidate();
  void InvalidateNow();

private:
  void on_swap_chain(DirtyRegion dirty_region);
  bool update_dirty(RectRegion *outDirtyInWindow);
  void hardware_update_dirty_recursion(Layer *p);
  void commit(const RectRegion & dirty_region_px);

public:
  Window& m_window;
  std::shared_ptr<WindowRenderRT> m_rt;

private:
  std::unique_ptr<IWindowRender> m_pIWindowRender;

  GRAPHICS_RENDER_LIBRARY_TYPE m_grl_type = GRAPHICS_RENDER_LIBRARY_TYPE_SKIA;

  // 阻塞刷新窗口
  long m_can_commit = 0;

  // 该窗口的渲染是否需要alpha通道。不再根据graphics render lib
  // type来决定。由用户自己设置
  bool m_need_alpha_channel = true;

  // 限制刷新时postmessage的次数。如果已经post了一个，就不再post
  long m_request_invalidate_ref = 0;
  weakptr_factory<WindowRender> m_weakptr_factory = {this};
};

//
// Window Render Stuff On RenderThread Or MainThread
// 如果没有启用RenderThread，则该对象工作在主线程。
//
class WindowRenderRT {
public:
  WindowRenderRT(WindowRender&);
  ~WindowRenderRT();

  void CreateHardwareCompositor(ui::IGpuCompositorWindow *window, ui::Rect rc);
  void Resize(uint width, uint height);
  void HardwareCommit();
  void OnLayerTreeChanged(LayerTreeSyncOperation op);

private:
  void hardwareCommit2(Layer *p, GpuLayerCommitContext *pContext);
  LayerRT* find_layer(LAYERID layer_id);

public:
  WindowRender&  m_window_render; // TODO: delete this var;

  IGpuCompositor *m_gpu_composition = nullptr;
  
  std::shared_ptr<LayerRT> m_root_layer;
  std::map<LAYERID, std::shared_ptr<LayerRT>> m_layer_map;

  ui::weakptr_factory<WindowRenderRT> m_factory = {this};
};


} // namespace ui

#endif