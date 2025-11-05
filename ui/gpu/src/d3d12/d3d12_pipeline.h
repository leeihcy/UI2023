#ifndef _UI_GPU_SRC_D3D12_D3D12_COMMAND_QUEUE_H_
#define _UI_GPU_SRC_D3D12_D3D12_COMMAND_QUEUE_H_

#include "src/d3d12/inc.h"

namespace ui {

class D3D12Pipeline {
public:
  
  bool createDescriptorHeap();

public:
  // render target view (RTV) descriptor heap
  CComPtr<ID3D12DescriptorHeap> m_rtv_descriptor_heap;
};

}

#endif