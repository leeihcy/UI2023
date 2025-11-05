#include "src/d3d12/d3d12_compositor.h"

namespace ui {

D3D12Compositor::~D3D12Compositor() {

}
std::shared_ptr<IGpuLayer> D3D12Compositor::CreateLayerTexture() {
    return std::shared_ptr<IGpuLayer>();
}

bool D3D12Compositor::Initialize(IGpuCompositorWindow *window) {
    return false;
}

bool D3D12Compositor::BeginCommit(GpuLayerCommitContext *) {
    return false;
}
void D3D12Compositor::EndCommit(GpuLayerCommitContext *) {

}
void D3D12Compositor::Resize(int nWidth, int nHeight) {

}

}