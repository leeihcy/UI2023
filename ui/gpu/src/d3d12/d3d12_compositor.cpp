#include "src/d3d12/d3d12_compositor.h"
#include "src/d3d12/d3d12_app.h"
#include "src/d3d12/d3d12_layer.h"

namespace ui {
static ID3D12Device *GetDevice() {
  return ui::D3D12Application::GetInstance().m_device;
}

D3D12Compositor::D3D12Compositor() : m_swapchain(*static_cast<d3d12::IBridge*>(this)) {

}
D3D12Compositor::~D3D12Compositor() {}

static void ReleaseGpuLayer(IGpuLayer* p) {
  if (p) {
    static_cast<D3D12Layer*>(p)->Release();
  }
}
std::shared_ptr<IGpuLayer> D3D12Compositor::CreateLayerTexture() {
  auto p = std::shared_ptr<D3D12Layer>(
      new D3D12Layer(*static_cast<d3d12::IBridge *>(this)), ReleaseGpuLayer);
  return p;
}

bool D3D12Compositor::Initialize(IGpuCompositorWindow *window) {
  assert(window->GetType() == GpuCompositorWindowType::WindowsHWND);
  m_hWnd = (HWND) static_cast<IGpuCompositorWindowHWND *>(window)->GetHWND();

  createCommandQueue();
  m_pipeline.Create();

  return true;
}

bool D3D12Compositor::createCommandQueue() {
  D3D12_COMMAND_QUEUE_DESC desc = {
    .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
    .Priority = 0,
    .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
    .NodeMask = 0
  };
  
  HRESULT hr = GetDevice()->CreateCommandQueue(
    &desc, __uuidof(ID3D12CommandQueue), (void**)&m_command_queue);
  if (FAILED(hr)) {
    return false;
  }

  return true;
}

bool D3D12Compositor::BeginCommit(GpuLayerCommitContext *) {
  if (m_swapchain.NeedReCreated()) {
    m_swapchain.Create(m_hWnd, m_command_queue, m_pipeline.m_pipeline_state);
  } else {
    m_swapchain.WaitForPreviousFrame(m_command_queue);
  }

  d3d12::InFlightFrame& infight_frame = m_swapchain.GetCurrentInflightFrame();
  d3d12::SwapChainFrame& swapchain_frame = m_swapchain.GetCurrentFrame();

  ID3D12GraphicsCommandList* command_list = infight_frame.m_command_list;

  infight_frame.BeginDraw(m_pipeline.m_pipeline_state);
  
    // Set necessary state.
  command_list->SetGraphicsRootSignature(m_pipeline.m_root_signature);
  command_list->RSSetViewports(1, &m_viewport);
  command_list->RSSetScissorRects(1, &m_scissor_rect);

  // Indicate that the back buffer will be used as a render target.
  auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      swapchain_frame.m_render_target, D3D12_RESOURCE_STATE_PRESENT,
      D3D12_RESOURCE_STATE_RENDER_TARGET);
  command_list->ResourceBarrier(1, &barrier);

  m_swapchain.SetCurrentRenderTarget(command_list);

  command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  return true;
}

void D3D12Compositor::EndCommit(GpuLayerCommitContext *) {
  d3d12::InFlightFrame& infight_frame = m_swapchain.GetCurrentInflightFrame();
  d3d12::SwapChainFrame& swapchain_frame = m_swapchain.GetCurrentFrame();

  // Indicate that the back buffer will now be used to present.
   auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      swapchain_frame.m_render_target,
      D3D12_RESOURCE_STATE_RENDER_TARGET,
      D3D12_RESOURCE_STATE_PRESENT);
  infight_frame.m_command_list->ResourceBarrier(1, &barrier);

  infight_frame.EndDraw(m_command_queue);
  m_swapchain.EndDraw();
  m_swapchain.IncCurrentInflightFrame();
}
void D3D12Compositor::Resize(int width, int height) {
  m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
  m_scissor_rect = CD3DX12_RECT(0, 0, static_cast<LONG>(width), static_cast<LONG>(height));

  // m_swapchain.MarkNeedReCreate();
  m_swapchain.Resize(width, height);
}

ID3D12CommandQueue* D3D12Compositor::GetCommandQueue() {
  return m_command_queue;
}
ID3D12GraphicsCommandList* D3D12Compositor::GetCurrentCommandList() {
  return m_swapchain.GetCurrentInflightFrame().m_command_list;
}

void D3D12Compositor::DeviceWaitIdle() {
  ID3D12Fence *pFence = nullptr;
  UINT64 fenceValue = 1;
  HRESULT hr =
      GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));

  m_command_queue->Signal(pFence, fenceValue);

  if (pFence->GetCompletedValue() < fenceValue) {
    HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
    pFence->SetEventOnCompletion(fenceValue, eventHandle);
    WaitForSingleObject(eventHandle, INFINITE);
    CloseHandle(eventHandle);
  }

  pFence->Release();
}
} // namespace ui