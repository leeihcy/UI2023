#include "RenderStates.h"
#include "src/stdafx.h"
#define SAFE_RELEASE(ptr)   do { if(ptr) { (ptr)->Release(); (ptr) = NULL; } } while(false)


ID3D10BlendState* RenderStates::pBlendStateTransparent = nullptr;
ID3D10BlendState* RenderStates::pBlendStateDisableWriteRenderTarget = nullptr;
ID3D10DepthStencilState* RenderStates::pStencilStateDisable = nullptr;
ID3D10DepthStencilState* RenderStates::pStencilStateCreateClip = nullptr;
ID3D10DepthStencilState* RenderStates::pStencilStateClip = nullptr;
ID3D10RasterizerState*  RenderStates::pRasterizerState = nullptr;

//
// ?? 在fx文件中只去设置vs/ps，其它的state全由代码来设置 
//
bool RenderStates::Init(ID3D10Device* pDevice)
{
    // 开启裁剪、抗锯齿
    {
        D3D10_RASTERIZER_DESC drd = {
            D3D10_FILL_SOLID, //D3D10_FILL_MODE FillMode;
            D3D10_CULL_NONE,  //显示背面
            FALSE, //BOOL FrontCounterClockwise;
            0, //INT DepthBias;
            0.0f,//FLOAT DepthBiasClamp;
            0.0f,//FLOAT SlopeScaledDepthBias;
            FALSE,//BOOL DepthClipEnable;
            TRUE,//BOOL ScissorEnable;  // 是否允许剪裁
            TRUE,//BOOL MultisampleEnable;  // 开启抗锯齿
            FALSE//BOOL AntialiasedLineEnable;         
        };
        pDevice->CreateRasterizerState(&drd, &pRasterizerState);
        pDevice->RSSetState(pRasterizerState); 
    }

    // 默认支持透明 alhpa  通道
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

	//  不启用深度缓存
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

    //填充剪裁区域
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

    //绘制剪裁区域中物体
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