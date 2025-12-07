#ifndef _UI_GPU_SRC_D3D12_D3D12_SWAPCHAIN_H_
#define _UI_GPU_SRC_D3D12_D3D12_SWAPCHAIN_H_

#include "src/d3d12/inc.h"
#include "src/d3d12/d3d12_bridge.h"
#include "src/d3d12/d3dx12.h"
#include "src/d3d12/d3d12_objects.h"

namespace d3d12 {

constexpr int SWAPCHAIN_FRAMES = 2;
constexpr int INFLIGHT_FRAMES = 2;

class InFlightFrame {
public:
  bool Create(ID3D12PipelineState* pipeline_state);
  void BeginDraw(ID3D12PipelineState* pipeline_state);
  void EndDraw(ID3D12CommandQueue*);

private:
  // 每个帧在GPU上执行时都需要独立的CommandAllocator
  // CommandAllocator在GPU执行命令列表期间不能被重置
  CComPtr<ID3D12CommandAllocator> m_command_allocator;

public:
  CComPtr<ID3D12GraphicsCommandList> m_command_list;
};


class SwapChainFrame {
public:
  bool Create(IBridge& bridge, int width, int height, int index);
  bool Destroy();

  void IncFenceValue() { m_fence_values++; }
  UINT64 GetFenceValue() { return m_fence_values; }

public:
  CComPtr<ID3D12Resource> m_render_target;
  UINT m_descriptor_heap_index = -1;

  UINT64 m_fence_values = 1;

  CComPtr<ID3D12Resource> m_frame_buffer;
};

class SwapChain {
public:
  SwapChain(IBridge& bridge);
  ~SwapChain();
  
  bool Create(HWND hwnd, ID3D12CommandQueue* command_queue, ID3D12PipelineState* pipeline_state);
  void DestroyImages();
  void Resize(int width, int height);
  void MarkNeedReCreate();
  bool NeedReCreated() { return m_need_recreate; }

  void IncCurrentInflightFrame();
  void IncCurrentSemaphores();
  void SetCurrentImageIndex(uint32_t);
  uint32_t GetCurrentImageIndex();

  SwapChainFrame& GetCurrentFrame();
  InFlightFrame& GetCurrentInflightFrame();
  // GpuSemaphores *GetCurrentSemaphores();

  void SetCurrentRenderTarget(ID3D12GraphicsCommandList* command_list);
  void EndDraw();
  void WaitForPreviousFrame(ID3D12CommandQueue* command_queue);
  
  bool createShaderResourceViewHeap();

private:
  bool createSwapChain(HWND hwnd, ID3D12CommandQueue* command_queue);
  bool createDescriptorHeap();
  bool createFrameResources();
  bool createInfightFrames(ID3D12PipelineState* pipeline_state);
  bool createFence();

  void destroyFrameResources();

public:
  IBridge& m_bridge;
  int m_width = 0;
  int m_height = 0;
  bool m_need_recreate = true;

  CComPtr<IDXGISwapChain3>  m_swapchain;

  // Render Target View
  DescriptorHeapStack m_rtv_heap;

  // Shader Resource View
  DescriptorHeapStack m_srv_heap;

  SwapChainFrame m_swap_frames[SWAPCHAIN_FRAMES];
  UINT m_swap_frame_index = 0;

  InFlightFrame m_inflight_frames[INFLIGHT_FRAMES];
  UINT m_inflight_frame_index = 0;

  HANDLE m_fence_event = nullptr;
  CComPtr<ID3D12Fence> m_fence;
};

}

#endif