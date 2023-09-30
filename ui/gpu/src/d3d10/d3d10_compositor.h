#ifndef _UI_GPU_SRC_D3D10_GPU_COMPOSITOR_H_
#define _UI_GPU_SRC_D3D10_GPU_COMPOSITOR_H_
#include "gpu/include/api.h"
#include <d3d10.h>
#include <dxgi.h>

//
// 如何开启DX10的抗锯齿？
//
// 1. CreateSwapChain时，指定SampleDesc的Count大于1
// 2. 在hlsl中指定SetRasterizerState, 将MultisampleEnable = TRUE;
//
// 如何实现剪裁？
// 1.
// 通过RSSetViewport来限制绘制区域是不对的。这种方式是将整个内容缩放绘制到新的
//    viewport当中，而不是我们想要的仅剪裁的效果。
//
// 2. RSSetScissorRects
//    The scissor rectangles will only be used if ScissorEnable is set to true
//    in the rasterizer state
//

namespace ui {
class GpuLayer;
class D3D10Compositor : public IGpuCompositor {
public:
  D3D10Compositor(HWND hWnd);
  ~D3D10Compositor();

  virtual void Release() override { delete this; }
  IGpuLayer *CreateLayerTexture();
  bool BeginCommit();
  void EndCommit();
  void SetRootLayerTexture(IGpuLayer *p);
  GpuLayer *GetRootLayerTexture();

  void Resize(int nWidth, int nHeight);
  SIZE GetSize() { return m_sizeBackBuffer; }

  void ClearStencil();

protected:
  void CreateSwapChain();
  void ReCreateRenderTargetView();
  void ReCreateStencilView();

public:
private:
  GpuLayer *m_pRootTexture;
  HWND m_hWnd;

  IDXGISwapChain *m_pSwapChain;
  ID3D10RenderTargetView *m_pRenderTargetView;

  ID3D10Texture2D *m_pDepthStencilBuffer;
  ID3D10DepthStencilView *m_pDepthStencilView;

  SIZE m_sizeBackBuffer;
};
} // namespace ui
#endif