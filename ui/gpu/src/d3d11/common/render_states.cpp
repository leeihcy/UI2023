#include "render_states.h"
#include "src/d3d11/D3D11_app.h"
#include <d3d11.h>

namespace d3d11 {

RenderStates &RenderStates::GetInstance() {
  return ui::D3D11Application::GetInstance().m_render_states;
}

//
// ?? 在fx文件中只去设置vs/ps，其它的state全由代码来设置
//
bool RenderStates::Init(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContent) {
  // 开启裁剪、抗锯齿
  {
    D3D11_RASTERIZER_DESC drd = {
        D3D11_FILL_SOLID, // D3D11_FILL_MODE FillMode;
        D3D11_CULL_NONE,  // 显示背面
        FALSE,            // BOOL FrontCounterClockwise;
        0,                // INT DepthBias;
        0.0f,             // FLOAT DepthBiasClamp;
        0.0f,             // FLOAT SlopeScaledDepthBias;
        FALSE,            // BOOL DepthClipEnable;
        TRUE,             // BOOL ScissorEnable;  // 是否允许剪裁
        TRUE,             // BOOL MultisampleEnable;  // 开启抗锯齿
        FALSE             // BOOL AntialiasedLineEnable;
    };
    pDevice->CreateRasterizerState(&drd, &pRasterizerState);
    pDeviceContent->RSSetState(pRasterizerState);
  }

  // 默认支持透明 alhpa  通道
  {
    D3D11_BLEND_DESC desc = {0};
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;

    desc.RenderTarget[0].BlendEnable = true;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    if (FAILED(pDevice->CreateBlendState(&desc, &pBlendStateTransparent))) {
      assert(0);
    } else {
      pDeviceContent->OMSetBlendState(pBlendStateTransparent, 0, 0xFFFFFFFF);
    }
  }

  {
    D3D11_BLEND_DESC blend_desc = {0};
    blend_desc.AlphaToCoverageEnable = FALSE;
    blend_desc.IndependentBlendEnable = FALSE;

    blend_desc.RenderTarget[0].BlendEnable = true;
    blend_desc.RenderTarget[0].RenderTargetWriteMask = 0;
    blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    if (FAILED(pDevice->CreateBlendState(
            &blend_desc, &pBlendStateDisableWriteRenderTarget))) {
      assert(0);
    }
  }

  //  不启用深度缓存
  {
    D3D11_DEPTH_STENCIL_DESC desc = {0};
    desc.DepthEnable = false;
    if (FAILED(
            pDevice->CreateDepthStencilState(&desc, &pStencilStateDisable))) {
      assert(0);
    }

    pDeviceContent->OMSetDepthStencilState(pStencilStateDisable, 0);
  }

  // 填充剪裁区域
  {
    D3D11_DEPTH_STENCIL_DESC desc = {0};

    desc.DepthEnable = false;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    desc.DepthFunc = D3D11_COMPARISON_LESS;

    desc.StencilEnable = true;
    desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

    desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

    if (FAILED(pDevice->CreateDepthStencilState(&desc,
                                                &pStencilStateCreateClip))) {
      assert(0);
    }
  }

  // 绘制剪裁区域中物体
  {
    D3D11_DEPTH_STENCIL_DESC desc = {0};

    desc.DepthEnable = false;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc.DepthFunc = D3D11_COMPARISON_LESS;

    desc.StencilEnable = true;
    desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

    desc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

    if (FAILED(pDevice->CreateDepthStencilState(&desc, &pStencilStateClip))) {
      assert(0);
    }
  }

  return true;
}

void RenderStates::Release() {
  pBlendStateTransparent.Release();
  pBlendStateDisableWriteRenderTarget.Release();
  pStencilStateDisable.Release();
  pStencilStateCreateClip.Release();
  pStencilStateClip.Release();
  pRasterizerState.Release();
}

} // namespace ui