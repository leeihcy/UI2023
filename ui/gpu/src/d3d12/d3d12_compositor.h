#ifndef _UI_GPU_SRC_D3D12_D3D12COMPOSITOR_H_
#define _UI_GPU_SRC_D3D12_D3D12COMPOSITOR_H_

#include "d3d12_swapchain.h"
#include "d3d12_pipeline.h"
#include "include/api.h"


namespace ui {

class D3D12Compositor : public IGpuCompositor {
public:
  ~D3D12Compositor() override;

  bool Initialize(IGpuCompositorWindow *window);

protected:
  std::shared_ptr<IGpuLayer> CreateLayerTexture() override;

  bool BeginCommit(GpuLayerCommitContext *) override;
  void EndCommit(GpuLayerCommitContext *) override;
  void Resize(int nWidth, int nHeight) override;

private:
  bool createCommandQueue();

private:
  CComPtr<ID3D12CommandQueue> m_command_queue;
  d3d12::SwapChain m_swapchain;
  d3d12::Pipeline m_pipeline;
};

} // namespace ui

#endif