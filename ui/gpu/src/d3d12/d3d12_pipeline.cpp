#include "src/d3d12/d3d12_pipeline.h"
#include "src/d3d12/d3d12_app.h"

namespace ui {

void D3D12Pipeline::createDescriptorHeap() {
  D3D12_DESCRIPTOR_HEAP_DESC desc = {
    .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
    .NumDescriptors = 2,
    .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
    .NodeMask = 0
  };

  HRESULT hr = D3D12Application::GetInstance().m_device->CreateDescriptorHeap(
      &desc, __uuidof(ID3D12DescriptorHeap), (void **)&m_rtv_descriptor_heap);
  if (FAILED(hr)) {
    return false;
  }
  return true;
}

} // namespace ui