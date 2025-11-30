#include "src/d3d12/d3d12_pipeline.h"
#include "src/d3d12/d3d12_app.h"
#include "src/d3d12/d3d12_objects.h"

namespace d3d12 {

static ID3D12Device* GetDevice() {
  return ui::D3D12Application::GetInstance().m_device;
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
  return true;
}

// RootSignature 和 VkPipelineLayout 对应。
bool Pipeline::createRootSignature(ID3D12Device *device) {
  CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
  rootSignatureDesc.Init(
      0, nullptr, 0, nullptr,
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

// bool Pipeline::createDescriptorHeap() {
//   D3D12_DESCRIPTOR_HEAP_DESC desc = {
//     .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
//     .NumDescriptors = 2,
//     .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
//     .NodeMask = 0
//   };

//   HRESULT hr =
//   D3D12Application::GetInstance().m_device->CreateDescriptorHeap(
//       &desc, __uuidof(ID3D12DescriptorHeap), (void
//       **)&m_rtv_descriptor_heap);
//   if (FAILED(hr)) {
//     return false;
//   }
//   return true;
// }

bool Pipeline::createPipelineState(ID3D12Device *device) {
  // Create the pipeline state, which includes compiling and loading shaders.

  CComPtr<ID3DBlob> vertexShader;
  CComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
  // Enable better shader debugging with the graphics debugging tools.
  UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
  UINT compileFlags = 0;
#endif

  D3DCompileFromFile(L"shaders/shaders.hlsl", nullptr, nullptr, "VSMain",
                     "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
  D3DCompileFromFile(L"shaders/shaders.hlsl", nullptr, nullptr, "PSMain",
                     "ps_5_0", compileFlags, 0, &pixelShader, nullptr);

  // Define the vertex input layout.
  D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

  // Describe and create the graphics pipeline state object (PSO).
  D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
  psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
  psoDesc.pRootSignature = m_root_signature;
  psoDesc.VS = {reinterpret_cast<UINT8 *>(vertexShader->GetBufferPointer()),
                vertexShader->GetBufferSize()};
  psoDesc.PS = {reinterpret_cast<UINT8 *>(pixelShader->GetBufferPointer()),
                pixelShader->GetBufferSize()};
  psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
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

} // namespace ui