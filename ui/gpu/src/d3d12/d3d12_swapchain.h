#ifndef _UI_GPU_SRC_D3D12_D3D12_SWAPCHAIN_H_
#define _UI_GPU_SRC_D3D12_D3D12_SWAPCHAIN_H_

#include "src/d3d12/inc.h"

namespace ui {

class D3D12SwapChain {
public:
  bool Create(HWND hwnd);

public:
  CComPtr<IDXGISwapChain3>  m_swapchain;

  // Synchronization objects.
  UINT m_frame_index = 0;
};

}

#endif