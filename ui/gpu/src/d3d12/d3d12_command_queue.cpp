#include "src/d3d12/d3d12_command_queue.h"
#include "d3d12_app.h"

namespace ui {

bool D3D12CommandQueue::Create() {
  D3D12_COMMAND_QUEUE_DESC desc = {
    .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
    .Priority = 0,
    .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
    .NodeMask = 0
  };
  
  HRESULT hr = D3D12Application::GetInstance().m_device->CreateCommandQueue(
    &desc, IID_ID3D12CommandQueue, (void**)&m_command_queue);
  if (FAILED(hr)) {
    return false;
  }

  return true;
}

}