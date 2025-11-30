#ifndef _UI_GPU_SRC_D3D12_D3D12_SWAPCHAIN_H_
#define _UI_GPU_SRC_D3D12_D3D12_SWAPCHAIN_H_

#include "src/d3d12/inc.h"

namespace d3d12 {

constexpr int SWAPCHAIN_FRAMES = 2;
constexpr int INFLIGHT_FRAMES = 2;

class InFlightFrame {
public:
  bool Create(ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipeline_state);
public:
  CComPtr<ID3D12GraphicsCommandList> m_command_list;
};


class SwapChainFrame {
public:
  bool Create();
  void IncFenceValue() { m_fence_values++; }
  UINT64 GetFenceValue() { return m_fence_values; }

public:
  CComPtr<ID3D12Resource> m_render_target;
  UINT64 m_fence_values = 0;
};

class SwapChain {
public:
  bool Create(HWND hwnd, ID3D12CommandQueue* command_queue, ID3D12PipelineState* pipeline_state);

  void IncCurrentInflightFrame();
  void IncCurrentSemaphores();
  void SetCurrentImageIndex(uint32_t);
  uint32_t GetCurrentImageIndex();

  SwapChainFrame& GetCurrentFrame();
  InFlightFrame& GetCurrentInflightFrame();
  // GpuSemaphores *GetCurrentSemaphores();

  void WaitForPreviousFrame(ID3D12CommandQueue* command_queue);

private:
  bool createSwapChain(HWND hwnd, ID3D12CommandQueue* command_queue);
  bool createDescriptorHeap();
  bool createFrameResources();
  bool createCommandAllocator();
  bool createInfightFrames(ID3D12PipelineState* pipeline_state);
  bool createFence();

public:
  CComPtr<IDXGISwapChain3>  m_swapchain;
  CComPtr<ID3D12DescriptorHeap> m_rtv_heap;
  UINT m_rtv_descriptor_size = 0;

  CComPtr<ID3D12CommandAllocator> m_command_allocator;

  SwapChainFrame m_swap_frames[SWAPCHAIN_FRAMES];
  UINT m_swap_frame_index = 0;

  InFlightFrame m_inflight_frames[INFLIGHT_FRAMES];
  UINT m_inflight_frame_index = 0;

  HANDLE m_fence_event = nullptr;
  CComPtr<ID3D12Fence> m_fence;
};

}

#endif