#pragma once
#include <D3D10.h>

class RenderStates
{
public:
    static bool Init(ID3D10Device* device);
    static void Release();

    // 默认状态，开启透明 
    static ID3D10BlendState*  pBlendStateTransparent;

    // 不修改后台缓存。场景：只填充模板缓存。
    static ID3D10BlendState*  pBlendStateDisableWriteRenderTarget;
    
    // 禁用模板缓存
    static ID3D10DepthStencilState*  pStencilStateDisable;

    // 修改模板缓存。场景：设置/填充剪裁区域
    static ID3D10DepthStencilState*  pStencilStateCreateClip;			

    // 在模板缓存的基础上，绘制其它对象。场景：剪裁
    static ID3D10DepthStencilState*  pStencilStateClip;	

    static ID3D10RasterizerState*  pRasterizerState;
};

