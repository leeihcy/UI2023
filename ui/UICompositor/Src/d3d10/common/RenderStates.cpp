#include "stdafx.h"
#include "RenderStates.h"

ID3D10BlendState* RenderStates::pBlendStateTransparent = nullptr;
ID3D10BlendState* RenderStates::pBlendStateDisableWriteRenderTarget = nullptr;
ID3D10DepthStencilState* RenderStates::pStencilStateDisable = nullptr;
ID3D10DepthStencilState* RenderStates::pStencilStateCreateClip = nullptr;
ID3D10DepthStencilState* RenderStates::pStencilStateClip = nullptr;
ID3D10RasterizerState*  RenderStates::pRasterizerState = nullptr;

//
// ?? ��fx�ļ���ֻȥ����vs/ps��������stateȫ�ɴ��������� 
//
bool RenderStates::Init(ID3D10Device* pDevice)
{
    // �����ü��������
    {
        D3D10_RASTERIZER_DESC drd = {
            D3D10_FILL_SOLID, //D3D10_FILL_MODE FillMode;
            D3D10_CULL_NONE,  //��ʾ����
            FALSE, //BOOL FrontCounterClockwise;
            0, //INT DepthBias;
            0.0f,//FLOAT DepthBiasClamp;
            0.0f,//FLOAT SlopeScaledDepthBias;
            FALSE,//BOOL DepthClipEnable;
            TRUE,//BOOL ScissorEnable;  // �Ƿ��������
            TRUE,//BOOL MultisampleEnable;  // ���������
            FALSE//BOOL AntialiasedLineEnable;         
        };
        pDevice->CreateRasterizerState(&drd, &pRasterizerState);
        pDevice->RSSetState(pRasterizerState); 
    }

    // Ĭ��֧��͸�� alhpa  ͨ��
    {
        D3D10_BLEND_DESC desc = {0};
        desc.AlphaToCoverageEnable = false;
        desc.BlendEnable[0] = true;
        desc.SrcBlend = D3D10_BLEND_ONE;
        desc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
        desc.BlendOp = D3D10_BLEND_OP_ADD;
        desc.SrcBlendAlpha = D3D10_BLEND_ONE;
        desc.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;
        desc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
        desc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
        if (FAILED(pDevice->CreateBlendState(&desc,&pBlendStateTransparent)))
        {
            assert(0);
        }
        else
        {
            pDevice->OMSetBlendState(pBlendStateTransparent, 0, 0xFFFFFFFF);
        }
    }

    {
        D3D10_BLEND_DESC blend_desc = {0};
        blend_desc.BlendEnable[0] = TRUE;
        blend_desc.RenderTargetWriteMask[0] = 0;
        blend_desc.AlphaToCoverageEnable = FALSE;    
        blend_desc.SrcBlend = D3D10_BLEND_ONE;    
        blend_desc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;    
        blend_desc.BlendOp = D3D10_BLEND_OP_ADD;    
        blend_desc.SrcBlendAlpha = D3D10_BLEND_ONE;    
        blend_desc.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;    
        blend_desc.BlendOpAlpha = D3D10_BLEND_OP_ADD;

        if (FAILED(pDevice->CreateBlendState(
                &blend_desc, &pBlendStateDisableWriteRenderTarget)))
        {
            assert(0);
        }
    }

	//  ��������Ȼ���
    {
        D3D10_DEPTH_STENCIL_DESC desc = {0};
        desc.DepthEnable = false;
        if (FAILED(pDevice->CreateDepthStencilState(
                &desc, &pStencilStateDisable)))
        {
            assert(0);
        }
        
        pDevice->OMSetDepthStencilState(pStencilStateDisable, 0);
    }

    //����������
    {
        D3D10_DEPTH_STENCIL_DESC desc = {0};

        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D10_COMPARISON_LESS;

        desc.StencilEnable = true;
        desc.StencilReadMask = D3D10_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D10_DEFAULT_STENCIL_WRITE_MASK;

        desc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
        desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_REPLACE;
        desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;

        desc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
        desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_REPLACE;
        desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
        desc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;

        if (FAILED(pDevice->CreateDepthStencilState(
                &desc, &pStencilStateCreateClip)))
        {
           assert(0);
        }
    }

    //���Ƽ�������������
    {
        D3D10_DEPTH_STENCIL_DESC desc = {0};

        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D10_COMPARISON_LESS;

        desc.StencilEnable = true;
        desc.StencilReadMask = D3D10_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D10_DEFAULT_STENCIL_WRITE_MASK;

        desc.FrontFace.StencilFunc = D3D10_COMPARISON_EQUAL;
        desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
        desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;

        desc.BackFace.StencilFunc = D3D10_COMPARISON_EQUAL;
        desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_REPLACE;
        desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
        desc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;

        if (FAILED(pDevice->CreateDepthStencilState(
                &desc, &pStencilStateClip)))
        {
            assert(0);
        }
    }

    return true;
}

void RenderStates::Release()
{
    SAFE_RELEASE(pBlendStateTransparent);
    SAFE_RELEASE(pBlendStateDisableWriteRenderTarget);
    SAFE_RELEASE(pStencilStateDisable);
    SAFE_RELEASE(pStencilStateCreateClip);
    SAFE_RELEASE(pStencilStateClip);
    SAFE_RELEASE(pRasterizerState);
}