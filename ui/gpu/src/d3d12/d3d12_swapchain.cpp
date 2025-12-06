#include "src/d3d12/d3d12_swapchain.h"
#include "src/d3d12/d3d12_app.h"
#include "src/d3d12/d3d12_objects.h"
#include "src/d3d12/shader/shader_types.h"
#include "src/util.h"

namespace d3d12 {

static ID3D12Device* GetDevice() {
  return ui::D3D12Application::GetInstance().m_device;
}

SwapChain::SwapChain(IBridge& bridge) : m_bridge(bridge) {
  memset(&m_srv_heap_handle_tail, 0, sizeof(m_srv_heap_handle_tail));
}
SwapChain::~SwapChain() {
  DestroyImages();
  if (m_fence_event) {
    CloseHandle(m_fence_event);
    m_fence_event = nullptr;
  }
}

void SwapChain::MarkNeedReCreate() { 
  m_need_recreate = true; 
  ui::Log("[d3d12] MarkNeedReCreate");
}

void SwapChain::DestroyImages() {
  destroyFrameResources();
}

void SwapChain::Resize(int width, int height) {
  m_bridge.DeviceWaitIdle();

  if (!m_swapchain) {
    return;
  }
  m_width = width;
  m_height = height;

  // 释放旧后台缓冲区资源
  destroyFrameResources();

  //  调整交换链缓冲区
  // 当调用 ResizeBuffers 时：
  // 交换链内部会释放对旧后台缓冲区的引用
  // 但你的 m_renderTargets[i] 仍然持有对旧资源的引用
  // 如果不手动 Release()，这些资源永远不会被释放 → 内存泄漏

  DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
  m_swapchain->GetDesc(&swapChainDesc);

  HRESULT hr = m_swapchain->ResizeBuffers(SWAPCHAIN_FRAMES, width, height,
                                          swapChainDesc.BufferDesc.Format,
                                          swapChainDesc.Flags);
  if (FAILED(hr)) {
    // 处理设备丢失：TDR或驱动更新
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
      // HandleDeviceLost();
      return;
    }
  }

  // 重新获取后台缓冲区和创建RTV
  m_swap_frame_index = m_swapchain->GetCurrentBackBufferIndex();
  createFrameResources();
}
bool SwapChain::Create(HWND hwnd, ID3D12CommandQueue *command_queue,
                            ID3D12PipelineState *pipeline_state) {
  if (!createSwapChain(hwnd, command_queue)) {
    return false;
  }
  if (!createDescriptorHeap()) {
    return false;
  }
  if (!createFrameResources()) {
      return false;
  }
  if (!createInfightFrames(pipeline_state)) {
    return false;
  }
  if (!createFence()) {
    return false;
  }
  
  m_need_recreate = false;
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
  m_width = rc.right - rc.left;
  m_height = rc.bottom - rc.top;

  DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = SWAPCHAIN_FRAMES;
    swapChainDesc.BufferDesc.Width =  (UINT)m_width;
    swapChainDesc.BufferDesc.Height = (UINT)m_height;
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
  m_rtv_heap_handle_tail = m_rtv_heap->GetCPUDescriptorHandleForHeapStart();
  return true;
}

bool SwapChain::createFrameResources() {
  CComPtr<ID3D12Device> device = GetDevice();

  UINT rtv_descriptor_size =
      device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

  // Create a RTV for each frame.
  for (UINT n = 0; n < SWAPCHAIN_FRAMES; n++) {
    SwapChainFrame& frame = m_swap_frames[n];

    m_swapchain->GetBuffer(n, IID_PPV_ARGS(&frame.m_render_target));
    frame.m_heap_ptr = m_rtv_heap_handle_tail;
    device->CreateRenderTargetView(frame.m_render_target, nullptr,
                                   frame.m_heap_ptr);
 
    m_rtv_heap_handle_tail.Offset(1, rtv_descriptor_size);

    m_swap_frames[n].Create(m_bridge, m_width, m_height, n);
  }

  return true;
}
void SwapChain::destroyFrameResources() {
  CComPtr<ID3D12Device> device = GetDevice();

  for (UINT n = 0; n < SWAPCHAIN_FRAMES; n++) {
    m_swap_frames[n].m_render_target.Release();
  }
}

bool SwapChain::createInfightFrames(ID3D12PipelineState* pipeline_state) {
  for (int i = 0; i < INFLIGHT_FRAMES; i++) {
    m_inflight_frames[i].Create(pipeline_state);
  }
  return true;
}

void InFlightFrame::BeginDraw(ID3D12PipelineState *pipeline_state) {
  m_command_allocator->Reset();
  m_command_list->Reset(m_command_allocator, pipeline_state);
}

void InFlightFrame::EndDraw(ID3D12CommandQueue *command_queue) {
  m_command_list->Close();
  
  ID3D12CommandList *ppCommandLists[] = {m_command_list};
  command_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

bool InFlightFrame::Create(ID3D12PipelineState* pipeline_state) {
  GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_command_allocator));

  // Create the command list.
  GetDevice()->CreateCommandList(0, 
    D3D12_COMMAND_LIST_TYPE_DIRECT, 
    m_command_allocator, 
    pipeline_state, 
    IID_PPV_ARGS(&m_command_list));

  // Command lists are created in the recording state, but there is nothing
  // to record yet. The main loop expects it to be closed, so close it now.
  m_command_list->Close();

  return true;
}

bool SwapChain::createShaderResourceViewHeap() {
  ID3D12Device* device = GetDevice();

  // 创建描述符堆
  D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
  srvHeapDesc.NumDescriptors = 1;
  srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  srvHeapDesc.NodeMask = 0;

  HRESULT hr = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srv_heap));
  m_srv_heap_handle_tail = m_srv_heap->GetCPUDescriptorHandleForHeapStart();

  return SUCCEEDED(hr);
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

void SwapChain::SetCurrentRenderTarget(ID3D12GraphicsCommandList *command_list) {
  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(GetCurrentFrame().m_heap_ptr);
  command_list->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

  const float clearColor[] = {0.0f, 0.0f, 0.0f, 0.0f};  // r g b [a]
  command_list->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void SwapChain::EndDraw() {
  m_swapchain->Present(1, 0);
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

bool SwapChainFrame::Create(IBridge& bridge, int width, int height, int index) {
  if (m_frame_buffer) {
    m_frame_buffer.Release();
  }

  // 在C++端填充矩阵后，必须转置（XMMatrixTranspose）后再传入，
  // 因为HLSL默认期望列优先矩阵，
  // 而DirectXMath库的矩阵默认是行优先。
  d3d12::FrameData constants;
  constants.view = DirectX::XMMatrixIdentity();
  
  // XMMatrixOrthographicLH: 创建以原点为中心的正交投影
  //   X轴：[-ViewWidth/2, ViewWidth/2]
  //   Y轴：[-ViewHeight/2, ViewHeight/2]
  //   Z轴：[NearZ, FarZ]
  //
  // XMMatrixOrthographicOffCenterLH: 分别指定左、右、上、下边界, 可以创建不以原点为中心的投影
  //
  // 投影矩阵的核心作用，就是将顶点从观察空间（View Space） 转换到裁剪空间（Clip Space）。
  // 而裁剪空间经过透视除法后，就得到了归一化设备坐标（Normalized Device Coordinates, NDC）。
  // NDC: 
  // X轴: [-1.0, 1.0]
  // Y轴: [-1.0, 1.0]
  // Z轴: [0.0, 1.0]
  //
  // 即使顶点在NDC空间，最终在渲染目标（如屏幕）上的位置还需要经过视口变换。
  // 通过 RSSetViewports 设置一个视口，它将NDC的 [-1, 1] 范围映射到你指定的屏幕矩形区域。
  // 这是将NDC坐标与最终显示窗口连接起来的关键步骤。
  //

  // 2D坐标（0，0）映射为NDC的（-1.0，1.0），
  // 2D坐标（w，h）映射为NDC的（1.0，-1.0）
  constants.ortho = DirectX::XMMatrixOrthographicOffCenterLH(
          0, (float)width, (float)height,0,  0.f, 2000.f);
  constants.ortho = DirectX::XMMatrixTranspose(constants.ortho);
  const UINT buffer_size = sizeof(constants);

  // DEFAULT: GPU专用
  CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
  auto desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
  GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                       D3D12_RESOURCE_STATE_COMMON, nullptr,
                                       IID_PPV_ARGS(&m_frame_buffer));

  d3d12::UploadContext upload_context(bridge);
  upload_context.UploadResource(
      m_frame_buffer, &constants, buffer_size, D3D12_RESOURCE_STATE_COMMON,
      D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

  // 为资源命名便于调试
  wchar_t name[25];
  swprintf_s(name, L"Render Target %d", index);
  m_render_target->SetName(name);

  return true;
}

} // namespace ui