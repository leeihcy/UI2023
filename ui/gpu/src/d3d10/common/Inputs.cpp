#include "Inputs.h"
#include "Effects.h"

// ID3D10Buffer*  Inputs::m_pVertexBuffer = nullptr;
ID3D10InputLayout*  Inputs::m_pInputLayout = nullptr;

bool  Inputs::Init(ID3D10Device* pDevice)
{
    // Create input layout
    const D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };

    D3D10_PASS_DESC PassDesc = {0};
    ID3D10EffectPass*  pPass = Effects::m_pTechDrawRect->GetPassByIndex( 0 );
    HRESULT hr = pPass->GetDesc( &PassDesc );
    pDevice->CreateInputLayout( layout, 3, PassDesc.pIAInputSignature,
        PassDesc.IAInputSignatureSize, &m_pInputLayout );

    D3D10_BUFFER_DESC BufDesc;
    BufDesc.ByteWidth = sizeof( DXUT_SCREEN_VERTEX_10 ) * 4;
    BufDesc.Usage = D3D10_USAGE_DYNAMIC;
    BufDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    BufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    BufDesc.MiscFlags = 0;

//     hr = pDevice->CreateBuffer(&BufDesc, 
//         nullptr, 
//         &m_pVertexBuffer);
//     if (FAILED(hr))
//         return false;

    pDevice->IASetInputLayout(m_pInputLayout);
    pDevice->IASetPrimitiveTopology( 
        D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

//     UINT stride = sizeof( DXUT_SCREEN_VERTEX_10 );
//     UINT offset = 0;
//     pDevice->IASetVertexBuffers(0, 1, 
//         &m_pVertexBuffer,
//         &stride, &offset);

   
    return true;
}

void  Inputs::Rlease()
{
    if (m_pInputLayout) {
     m_pInputLayout->Release();
     m_pInputLayout = nullptr;   
    }
//    SAFE_RELEASE(m_pVertexBuffer);
}

// void Inputs::SetVertex4(DXUT_SCREEN_VERTEX_10 vertices[4])
// {
//     DXUT_SCREEN_VERTEX_10* pVB;
//     if (SUCCEEDED(m_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (LPVOID*)&pVB)))
//     {
//         CopyMemory(pVB, vertices, sizeof( DXUT_SCREEN_VERTEX_10 )*4);
//         m_pVertexBuffer->Unmap();
//     }
// }