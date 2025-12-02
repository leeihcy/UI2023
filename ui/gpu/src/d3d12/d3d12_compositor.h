#ifndef _UI_GPU_SRC_D3D12_D3D12COMPOSITOR_H_
#define _UI_GPU_SRC_D3D12_D3D12COMPOSITOR_H_

#include "d3d12_swapchain.h"
#include "d3d12_pipeline.h"
#include "include/api.h"
#include "src/d3d12/d3d12_bridge.h"
#include "src/d3d12/d3dx12.h"

namespace ui {

class D3D12Compositor : public IGpuCompositor, public d3d12::IBridge {
public:
  D3D12Compositor();
  ~D3D12Compositor() override;

  bool Initialize(IGpuCompositorWindow *window);

protected:
  std::shared_ptr<IGpuLayer> CreateLayerTexture() override;

  bool BeginCommit(GpuLayerCommitContext *) override;
  void EndCommit(GpuLayerCommitContext *) override;
  void Resize(int nWidth, int nHeight) override;

  // d3d12::IBridge
  void DeviceWaitIdle() override;
  ID3D12CommandQueue* GetCommandQueue() override;
  ID3D12GraphicsCommandList* GetCurrentCommandList() override;
  
private:
  bool createCommandQueue();

private:
  HWND m_hWnd = nullptr;

  CComPtr<ID3D12CommandQueue> m_command_queue;
  d3d12::SwapChain m_swapchain;
  d3d12::Pipeline m_pipeline;

  CD3DX12_VIEWPORT m_viewport;
  CD3DX12_RECT m_scissor_rect;
};

} // namespace ui

#endif