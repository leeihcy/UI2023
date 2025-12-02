#ifndef _UI_GPU_SRC_D3D12_D3D12OBJECTS_H_
#define _UI_GPU_SRC_D3D12_D3D12OBJECTS_H_
#include "src/d3d12/d3d12_bridge.h"
#include "src/d3d12/d3dx12.h"
#include "src/d3d12/inc.h"


namespace d3d12 {

class UploadContext {
public:
  UploadContext(IBridge &bridge);
  ~UploadContext();

  bool Create();
  CComPtr<ID3D12Resource> CreateUploadBuffer(UINT64 size);

  void UploadResource(ID3D12Resource *resource, const void *data, UINT64 size,
                      D3D12_RESOURCE_STATES current_state,
                      D3D12_RESOURCE_STATES dest_state);
  void UploadTexure(ID3D12Resource *resource, const void *data, UINT64 size,
                      D3D12_RESOURCE_STATES current_state,
                      D3D12_RESOURCE_STATES dest_state);

  ID3D12GraphicsCommandList* GetCommandList() { return m_command_list; }
  void FlushAndFinish();

private:
  IBridge &m_bridge;

  CComPtr<ID3D12CommandAllocator> m_allocator;

  CComPtr<ID3D12GraphicsCommandList> m_command_list;
  CComPtr<ID3D12Fence> m_fence;
  HANDLE m_fence_event = nullptr;
  UINT m_fence_value = 1;
};

} // namespace d3d12

#endif