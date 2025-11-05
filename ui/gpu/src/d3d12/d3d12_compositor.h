#ifndef _UI_GPU_SRC_D3D12_D3D12COMPOSITOR_H_
#define _UI_GPU_SRC_D3D12_D3D12COMPOSITOR_H_

#include "d3d12_swapchain.h"
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
  D3D12SwapChain m_swapchain;
};

} // namespace ui

#endif