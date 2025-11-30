#include "src/d3d12/d3d12_swapchain.h"
#include "src/d3d12/d3d12_app.h"
#include "src/d3d12/d3d12_objects.h"


namespace d3d12 {

static ID3D12Device* GetDevice() {
  return ui::D3D12Application::GetInstance().m_device;
}

bool SwapChain::Create(HWND hwnd, ID3D12CommandQueue *command_queue,
                            ID3D12PipelineState *pipeline_state) {
  if (!createSwapChain(hwnd, command_queue)) {
    return false;
  }

  if (!createDescriptorHeap()) {
    return false;
  }

  if (!createCommandAllocator()) {
    return false;
  }

  if (!createInfightFrames(pipeline_state)) {
    return false;
  }
  if (!createFence()) {
    return false;
  }

  return true;
}

bool SwapChain::createSwapChain(HWND hwnd, ID3D12CommandQueue* command_queue) {
  CComPtr<IDXGIFactory2> factory2;
  HRESULT hr = ui::D3D12Application::GetInstance().m_dxgi_factory->QueryInterface(
      __uuidof(IDXGIFactory2), (void**)&factory2);
  if (FAILED(hr)) {
    return false;
  }

  RECT rc;
  ::GetClientRect(hwnd, &rc);

  DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = SWAPCHAIN_FRAMES;
    swapChainDesc.BufferDesc.Width =  (UINT)(rc.right - rc.left);
    swapChainDesc.BufferDesc.Height = (UINT)(rc.bottom - rc.top);
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    CComPtr<IDXGISwapChain> swapChain;
    hr = factory2->CreateSwapChain(command_queue, &swapChainDesc, &swapChain);
    if (FAILED(hr)) {
      return false;
    }
  hr = swapChain.QueryInterface(&m_swapchain);
  if (FAILED(hr)) {
    return false;
  }

  // 禁用全局快捷键
  factory2->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

  m_swap_frame_index = m_swapchain->GetCurrentBackBufferIndex();

  return true;
}

bool SwapChain::createDescriptorHeap() {
  // Describe and create a render target view (RTV) descriptor heap.
  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
  rtvHeapDesc.NumDescriptors = SWAPCHAIN_FRAMES;
  rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  
  GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtv_heap));
  m_rtv_descriptor_size = GetDevice()->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

  return true;
}

bool SwapChain::createFrameResources() {
  CComPtr<ID3D12Device> device = GetDevice();
  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());

  // Create a RTV for each frame.
  for (UINT n = 0; n < SWAPCHAIN_FRAMES; n++) {
    m_swapchain->GetBuffer(n, IID_PPV_ARGS(&m_swap_frames[n].m_render_target));
    device->CreateRenderTargetView(m_swap_frames[n].m_render_target, nullptr,
                                     rtvHandle);
    rtvHandle.Offset(1, m_rtv_descriptor_size);
  }

  return true;
}

bool SwapChain::createCommandAllocator() {
  GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_command_allocator));
  return true;
}

bool SwapChain::createInfightFrames(ID3D12PipelineState* pipeline_state) {
  for (int i = 0; i < INFLIGHT_FRAMES; i++) {
    m_inflight_frames[i].Create(m_command_allocator, pipeline_state);
  }
  return true;
}

bool InFlightFrame::Create(ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipeline_state) {
  // Create the command list.
  GetDevice()->CreateCommandList(0, 
    D3D12_COMMAND_LIST_TYPE_DIRECT, 
    allocator, 
    pipeline_state, 
    IID_PPV_ARGS(&m_command_list));

  // Command lists are created in the recording state, but there is nothing
  // to record yet. The main loop expects it to be closed, so close it now.
  m_command_list->Close();

  return true;
}

void SwapChain::IncCurrentInflightFrame() {
    m_inflight_frame_index = (m_inflight_frame_index + 1) % INFLIGHT_FRAMES;
}

SwapChainFrame& SwapChain::GetCurrentFrame() {
  assert(m_swap_frame_index < SWAPCHAIN_FRAMES);
  return m_swap_frames[m_swap_frame_index];
}
InFlightFrame& SwapChain::GetCurrentInflightFrame() {
    assert(m_inflight_frame_index < INFLIGHT_FRAMES);
  return m_inflight_frames[m_inflight_frame_index];
}

bool SwapChain::createFence() {
  // Create an empty root signature.
  CComPtr<ID3D12Device> device = GetDevice();
  device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

  // Create an event handle to use for frame synchronization.
  m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
  if (m_fence_event == nullptr) {
    return false;
  }

  return true;
}

void SwapChain::WaitForPreviousFrame(ID3D12CommandQueue *command_queue) {
  m_swap_frame_index = m_swapchain->GetCurrentBackBufferIndex();
  SwapChainFrame &swapchain_frame = GetCurrentFrame();

  // Signal and increment the fence value.

  // Updates a fence to a specified value from the GPU side.
  // 注：Use ID3D12Fence::Signal to set a fence from the CPU side.
  UINT64 fence_value = swapchain_frame.GetFenceValue();
  command_queue->Signal(m_fence, fence_value);

  swapchain_frame.IncFenceValue();

  // Wait until the previous frame is finished.
  if (m_fence->GetCompletedValue() < fence_value) {
    m_fence->SetEventOnCompletion(fence_value, m_fence_event);
    WaitForSingleObject(m_fence_event, INFINITE);
  }
}

} // namespace ui