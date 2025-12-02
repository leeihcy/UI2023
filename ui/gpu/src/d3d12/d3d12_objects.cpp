#include "src/d3d12/d3d12_objects.h"
#include "src/d3d12/d3d12_app.h"

namespace d3d12 {

static ID3D12Device *GetDevice() {
  return ui::D3D12Application::GetInstance().m_device;
}

UploadContext::UploadContext(IBridge &bridge) : m_bridge(bridge) {}
UploadContext::~UploadContext() {
  if (m_fence_event) {
    CloseHandle(m_fence_event);
    m_fence_event = nullptr;
  }
}

bool UploadContext ::Create() {
  GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                      IID_PPV_ARGS(&m_allocator));

  // Create the command list.
  GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_allocator,
                                 nullptr, IID_PPV_ARGS(&m_command_list));

  GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
  m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);

  return true;
}

// 上传资源并等待完成
void UploadContext::UploadResource(ID3D12Resource *resource, const void *data,
                                   UINT64 size,
                                   D3D12_RESOURCE_STATES current_state,
                                   D3D12_RESOURCE_STATES dest_state) {
  if (!m_command_list) {
    Create();
  }

  // m_command_list->Close();
  // m_allocator->Reset();
  // m_command_list->Reset(m_allocator, nullptr);

  CComPtr<ID3D12Resource> uploadBuffer = CreateUploadBuffer(size);

  void *map_data = nullptr;
  HRESULT hr = uploadBuffer->Map(0, nullptr, &map_data);
  memcpy(map_data, data, size);
  uploadBuffer->Unmap(0, nullptr);

  D3D12_RESOURCE_BARRIER barrier_desc;
  ZeroMemory(&barrier_desc, sizeof(barrier_desc));
  barrier_desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
  barrier_desc.Transition.pResource = resource;
  barrier_desc.Transition.Subresource = 0;

  if ((current_state & D3D12_RESOURCE_STATE_COPY_DEST) == 0) {
    barrier_desc.Transition.StateBefore = current_state;
    barrier_desc.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    m_command_list->ResourceBarrier(1, &barrier_desc);
    current_state = D3D12_RESOURCE_STATE_COPY_DEST;
  }

  m_command_list->CopyBufferRegion(resource, 0, uploadBuffer, 0, size);

  if (current_state != dest_state) {
    barrier_desc.Transition.StateBefore = current_state;
    barrier_desc.Transition.StateAfter = dest_state;
    m_command_list->ResourceBarrier(1, &barrier_desc);
  }

  FlushAndFinish();
}

void UploadContext::UploadTexure(ID3D12Resource *resource, const void *data, UINT64 size,
                      D3D12_RESOURCE_STATES current_state,
                      D3D12_RESOURCE_STATES dest_state) 
{

}                      

CComPtr<ID3D12Resource> UploadContext::CreateUploadBuffer(UINT64 size) {
  CComPtr<ID3D12Resource> uploadBuffer;
  CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
  auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);
  GetDevice()->CreateCommittedResource(
    &heapProps, D3D12_HEAP_FLAG_NONE, &desc,
    D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
    IID_PPV_ARGS(&uploadBuffer));
  return uploadBuffer;
}

void UploadContext::FlushAndFinish() {
  ID3D12CommandQueue *command_queue = m_bridge.GetCommandQueue();

  m_command_list->Close();
  ID3D12CommandList *lists[] = {m_command_list};
  command_queue->ExecuteCommandLists(1, lists);

  UINT64 fence_value = m_fence_value;
  command_queue->Signal(m_fence, fence_value);
  m_fence->SetEventOnCompletion(m_fence_value, m_fence_event);
  m_fence_value++;

  WaitForSingleObject(m_fence_event, INFINITE);

  m_allocator->Reset();
  m_command_list->Reset(m_allocator, nullptr);
}

} // namespace d3d12