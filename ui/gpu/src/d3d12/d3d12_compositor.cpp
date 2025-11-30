#include "src/d3d12/d3d12_compositor.h"
#include "src/d3d12/d3d12_app.h"

namespace ui {

D3D12Compositor::~D3D12Compositor() {}
std::shared_ptr<IGpuLayer> D3D12Compositor::CreateLayerTexture() {
  return std::shared_ptr<IGpuLayer>();
}

bool D3D12Compositor::Initialize(IGpuCompositorWindow *window) {
  assert(window->GetType() == GpuCompositorWindowType::WindowsHWND);
  HWND hwnd = (HWND) static_cast<IGpuCompositorWindowHWND *>(window)->GetHWND();

  createCommandQueue();
  m_pipeline.Create();

  m_swapchain.Create(hwnd, m_command_queue, m_pipeline.m_pipeline_state);
  
  return false;
}

bool D3D12Compositor::createCommandQueue() {
  D3D12_COMMAND_QUEUE_DESC desc = {
    .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
    .Priority = 0,
    .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
    .NodeMask = 0
  };
  
  HRESULT hr = D3D12Application::GetInstance().m_device->CreateCommandQueue(
    &desc, __uuidof(ID3D12CommandQueue), (void**)&m_command_queue);
  if (FAILED(hr)) {
    return false;
  }

  return true;
}

bool D3D12Compositor::BeginCommit(GpuLayerCommitContext *) {
    m_swapchain.WaitForPreviousFrame(m_command_queue);

#if 0
    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();
#endif
    return true;
}

void D3D12Compositor::EndCommit(GpuLayerCommitContext *) {
#if 0
  // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    m_swapchain.m_swapchain->Present(1, 0);
#endif
  m_swapchain.IncCurrentInflightFrame();
  
}
void D3D12Compositor::Resize(int nWidth, int nHeight) {}

} // namespace ui