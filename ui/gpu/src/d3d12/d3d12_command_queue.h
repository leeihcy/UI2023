#ifndef _UI_GPU_SRC_D3D12_D3D12_COMMAND_QUEUE_H_
#define _UI_GPU_SRC_D3D12_D3D12_COMMAND_QUEUE_H_

#include "src/d3d12/inc.h"

namespace ui {

class D3D12CommandQueue {
public:
  bool Create();

public:
  CComPtr<ID3D12CommandQueue>  m_command_queue;
};

}

#endif