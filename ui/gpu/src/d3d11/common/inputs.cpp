#include "Inputs.h"
#include "effects.h"
#include "src/d3d11/d3d11_app.h"

namespace d3d11 {

//static 
Inputs& Inputs::GetInstance() {
  return ui::D3D11Application::GetInstance().m_inputs;
}

bool Inputs::Init(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContent) {
  // Create input layout
  const D3D11_INPUT_ELEMENT_DESC layout[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
       D3D11_INPUT_PER_VERTEX_DATA, 0}, // 3*sizeof(float) = 12
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
       D3D11_INPUT_PER_VERTEX_DATA, 0}, // 4*sizeof(float) = 16
      {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28,
       D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  D3DX11_PASS_DESC PassDesc = {0};
  ID3DX11EffectPass *pPass = Effects::GetInstance().m_pTechDrawRect->GetPassByIndex(0);
  HRESULT hr = pPass->GetDesc(&PassDesc);
  pDevice->CreateInputLayout(layout, 3, PassDesc.pIAInputSignature,
                             PassDesc.IAInputSignatureSize, &m_pInputLayout);

  D3D11_BUFFER_DESC BufDesc;
  BufDesc.ByteWidth = sizeof(d3d11::DXUT_SCREEN_VERTEX_11) * 4;
  BufDesc.Usage = D3D11_USAGE_DYNAMIC;
  BufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  BufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  BufDesc.MiscFlags = 0;

  //     hr = pDevice->CreateBuffer(&BufDesc,
  //         nullptr,
  //         &m_pVertexBuffer);
  //     if (FAILED(hr))
  //         return false;

  pDeviceContent->IASetInputLayout(m_pInputLayout);
  pDeviceContent->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  //     UINT stride = sizeof( DXUT_SCREEN_VERTEX_10 );
  //     UINT offset = 0;
  //     pDevice->IASetVertexBuffers(0, 1,
  //         &m_pVertexBuffer,
  //         &stride, &offset);

  return true;
}

void Inputs::Rlease() {
  m_pInputLayout.Release();
  //    SAFE_RELEASE(m_pVertexBuffer);
}

// void Inputs::SetVertex4(DXUT_SCREEN_VERTEX_10 vertices[4])
// {
//     DXUT_SCREEN_VERTEX_10* pVB;
//     if (SUCCEEDED(m_pVertexBuffer->Map(D3D11_MAP_WRITE_DISCARD, 0,
//     (LPVOID*)&pVB)))
//     {
//         CopyMemory(pVB, vertices, sizeof( DXUT_SCREEN_VERTEX_10 )*4);
//         m_pVertexBuffer->Unmap();
//     }
// }

}