#include "src/d3d12/d3d12_pipeline.h"
#include "src/d3d12/d3d12_app.h"
#include "src/d3d12/shader/shader_types.h"
#include "src/d3d12/d3d12_objects.h"
#include "src/util.h"

namespace d3d12 {

static ID3D12Device* GetDevice() {
  return ui::D3D12Application::GetInstance().m_device;
}

Pipeline::Pipeline(IBridge& bridge) : m_bridge(bridge) {

}
bool Pipeline::Create() {
  // Create an empty root signature.
  CComPtr<ID3D12Device> device = GetDevice();

  if (!createRootSignature(device)) {
    return false;
  }

  if (!createPipelineState(device)) {
    return false;
  }
  if (!createSampleHeap()) {
    return false;
  }

  return true;
}



// RootSignature 和 VkPipelineLayout 对应。
bool Pipeline::createRootSignature(ID3D12Device *device) {
  CD3DX12_ROOT_PARAMETER parameters[3];

  // 将索引为0的根参数，定义为绑定到寄存器 b0 的常量缓冲区视图 (CBV)
  // 第一个参数0：绑定到 b0 寄存器（b 表示 Constant Buffer）
  // 第二个参数0：空间索引为 space0
  // 第三个参数表示只对顶点着色器可见
  parameters[0].InitAsConstantBufferView(SHADER_REGISTER_INDEX_CONSTAT_BUFFER, 0, D3D12_SHADER_VISIBILITY_VERTEX);

  // 纹理数组（SRV）
  CD3DX12_DESCRIPTOR_RANGE texRange;
  texRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,  // 类型：Shader Resource View
                1,                                 // 数量：1个纹理
                0,                                 // 基础寄存器：t0
                0);                                // 空间：space0
  parameters[1].InitAsDescriptorTable(1, &texRange, D3D12_SHADER_VISIBILITY_PIXEL);

  // 采样器
  CD3DX12_DESCRIPTOR_RANGE samplerRange;
  samplerRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,  // 类型：采样器
                    1,                                     // 数量：1个采样器
                    0,                                     // 基础寄存器：s0
                    0);                                    // 空间：space0
  parameters[2].InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);

  CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
  rootSignatureDesc.Init(
      std::size(parameters), parameters, 0, nullptr,
      D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

  CComPtr<ID3DBlob> signature;
  CComPtr<ID3DBlob> error;
  D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                              &signature, &error);
  device->CreateRootSignature(0, signature->GetBufferPointer(),
                              signature->GetBufferSize(),
                              IID_PPV_ARGS(&m_root_signature));
  return true;
}

bool Pipeline::createPipelineState(ID3D12Device *device) {
  // Create the pipeline state, which includes compiling and loading shaders.

  CComPtr<ID3DBlob> vertexShader;
  CComPtr<ID3DBlob> pixelShader;
  CComPtr<ID3DBlob> errorMsgs;

#if defined(_DEBUG)
  // Enable better shader debugging with the graphics debugging tools.
  UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
  UINT compileFlags = 0;
#endif

  HRESULT hr1 =
      D3DCompileFromFile(L"shaders/d3d12.hlsl", nullptr, nullptr, "VSMain",
                         "vs_5_0", compileFlags, 0, &vertexShader, &errorMsgs);
  if (FAILED(hr1)) {
    if (errorMsgs != NULL) {
      ui::Log((char *)errorMsgs->GetBufferPointer());
    }
    assert(false);
    return false;
  }
  HRESULT hr2 =
      D3DCompileFromFile(L"shaders/d3d12.hlsl", nullptr, nullptr, "PSMain",
                         "ps_5_0", compileFlags, 0, &pixelShader, &errorMsgs);
  if (FAILED(hr2)) {
    if (errorMsgs != NULL) {
      ui::Log((char *)errorMsgs->GetBufferPointer());
    }
    assert(false);
    return false;
  }

  // Define the vertex input layout.
  D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 20,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
       
       // ------------------------------------------

       // 最后一个参数InstanceDataStepRate 设为 1（表示每实例更新一次）
      {"OFFSET", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0,
       D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},  //
  };

  // Describe and create the graphics pipeline state object (PSO).
  D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
  psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
  psoDesc.pRootSignature = m_root_signature;
  psoDesc.VS = {reinterpret_cast<UINT8 *>(vertexShader->GetBufferPointer()),
                vertexShader->GetBufferSize()};
  psoDesc.PS = {reinterpret_cast<UINT8 *>(pixelShader->GetBufferPointer()),
                pixelShader->GetBufferSize()};
  
  psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
  // psoDesc.RasterizerState.CullMode =D3D12_CULL_MODE_NONE;

  psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
  psoDesc.DepthStencilState.DepthEnable = FALSE;
  psoDesc.DepthStencilState.StencilEnable = FALSE;
  psoDesc.SampleMask = UINT_MAX;
  psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
  psoDesc.NumRenderTargets = 1;
  psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
  psoDesc.SampleDesc.Count = 1;
  device->CreateGraphicsPipelineState(&psoDesc,
                                      IID_PPV_ARGS(&m_pipeline_state));
  return true;
}

bool Pipeline::createSampleHeap() {
  // Describe and create a sampler descriptor heap.
  D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
  samplerHeapDesc.NumDescriptors = 1;
  samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
  samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  GetDevice()->CreateDescriptorHeap(&samplerHeapDesc,
                                    IID_PPV_ARGS(&m_samplerHeap));

  // Describe and create a sampler.
  D3D12_SAMPLER_DESC samplerDesc = {};
  samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
  samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
  samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 1;
  samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
  GetDevice()->CreateSampler(
      &samplerDesc, m_samplerHeap->GetCPUDescriptorHandleForHeapStart());

  return true;
}

} // namespace ui