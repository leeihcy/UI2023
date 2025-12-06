#ifndef _UI_GPU_SRC_D3D12_D3D12_COMMAND_QUEUE_H_
#define _UI_GPU_SRC_D3D12_D3D12_COMMAND_QUEUE_H_

#include "src/d3d12/inc.h"
#include "src/d3d12/d3d12_bridge.h"

namespace d3d12 {

class Pipeline {
public:
  Pipeline(IBridge& bridge);
  bool Create();

private:
  bool createRootSignature(ID3D12Device* device);
  bool createPipelineState(ID3D12Device* device);
  void createFrameData();
  bool createSampleHeap();

public:
  IBridge& m_bridge;

  // render target view (RTV) descriptor heap
  CComPtr<ID3D12DescriptorHeap> m_rtv_descriptor_heap;

  CComPtr<ID3D12DescriptorHeap> m_samplerHeap;

  CComPtr<ID3D12RootSignature> m_root_signature;
  CComPtr<ID3D12PipelineState> m_pipeline_state;
};

}

#endif