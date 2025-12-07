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

class DescriptorHeapStack {
public:
  DescriptorHeapStack(ID3D12Device &device)  : m_device(device) {}

  void Create(UINT numDescriptors,
                      D3D12_DESCRIPTOR_HEAP_TYPE type, 
                      D3D12_DESCRIPTOR_HEAP_FLAGS flags,
                      UINT NodeMask)
     {
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = numDescriptors;
    desc.Type = type;
    desc.Flags = flags;
    desc.NodeMask = NodeMask;
    m_device.CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pDescriptorHeap));

    m_descriptorSize = m_device.GetDescriptorHandleIncrementSize(type);
    m_descriptorHeapCpuBase =
        m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
  }
  void Destroy() {
    m_pDescriptorHeap.Release();
    m_descriptorsAllocated = 0;
    m_descriptorSize = 0;
    m_descriptorHeapCpuBase.ptr = 0;
  }

  ID3D12DescriptorHeap &GetDescriptorHeap() { return *m_pDescriptorHeap; }

  void AllocateDescriptor(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE &cpuHandle,
                          _Out_ UINT &descriptorHeapIndex) {
    descriptorHeapIndex = m_descriptorsAllocated;
    cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_descriptorHeapCpuBase, descriptorHeapIndex, m_descriptorSize);
    m_descriptorsAllocated++;
  }

  UINT AllocateBufferSrv(_In_ ID3D12Resource &resource) {
    UINT descriptorHeapIndex;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    AllocateDescriptor(cpuHandle, descriptorHeapIndex);
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.Buffer.NumElements =
        (UINT)(resource.GetDesc().Width / sizeof(UINT32));
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
    m_device.CreateShaderResourceView(&resource, &srvDesc, cpuHandle);

    return descriptorHeapIndex;
  }

  UINT AllocateBufferUav(_In_ ID3D12Resource &resource) {
    UINT descriptorHeapIndex;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    AllocateDescriptor(cpuHandle, descriptorHeapIndex);
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.NumElements =
        (UINT)(resource.GetDesc().Width / sizeof(UINT32));
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
    uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;

    m_device.CreateUnorderedAccessView(&resource, nullptr, &uavDesc, cpuHandle);
    return descriptorHeapIndex;
  }
  
  D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT descriptorIndex) {
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        descriptorIndex, m_descriptorSize);
  }
  D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT descriptorIndex) {
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(
        m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart(),
        descriptorIndex, m_descriptorSize);
  }

  ID3D12DescriptorHeap* GetHeap() { return m_pDescriptorHeap; }
private:
  ID3D12Device &m_device;
  CComPtr<ID3D12DescriptorHeap> m_pDescriptorHeap;
  UINT m_descriptorsAllocated = 0;
  UINT m_descriptorSize;
  D3D12_CPU_DESCRIPTOR_HANDLE m_descriptorHeapCpuBase;
};

} // namespace d3d12

#endif