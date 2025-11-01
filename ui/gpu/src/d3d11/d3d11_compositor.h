#ifndef _UI_GPU_SRC_D3D11_GPU_COMPOSITOR_H_
#define _UI_GPU_SRC_D3D11_GPU_COMPOSITOR_H_
#include "gpu/include/api.h"
#include "src/d3d11/inc.h"

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
class D3D11Compositor : public IGpuCompositor {
public:
  D3D11Compositor();
  ~D3D11Compositor();

  bool Initialize(IGpuCompositorWindow*);
  std::shared_ptr<IGpuLayer> CreateLayerTexture() override;
  bool BeginCommit(GpuLayerCommitContext*) override;
  void EndCommit(GpuLayerCommitContext*) override;
  // GpuLayer *GetRootLayerTexture();

  void Resize(int nWidth, int nHeight) override;
  SIZE GetSize() { return m_sizeBackBuffer; }
  // int GetWidth() override { m_sizeBackBuffer.cx; }
  // int GetHeight() override { m_sizeBackBuffer.cy; }

  void ClearStencil();

protected:
  void CreateSwapChain();
  void ReCreateRenderTargetView();
  void ReCreateStencilView();

public:
private:
  // GpuLayer *m_pRootTexture;
  HWND m_hWnd;

  CComPtr<IDXGISwapChain> m_pSwapChain;
  CComPtr<ID3D11RenderTargetView> m_pRenderTargetView;

  CComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
  CComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

  SIZE m_sizeBackBuffer;
};
} // namespace ui
#endif