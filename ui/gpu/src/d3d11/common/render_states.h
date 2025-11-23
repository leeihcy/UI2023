#ifndef _UI_GPU_SRC_D3D11_COMMON_RENDERSTATES_H_
#define _UI_GPU_SRC_D3D11_COMMON_RENDERSTATES_H_

#include "src/d3d11/inc.h"

namespace d3d11 {

class RenderStates {
public:
  static RenderStates& GetInstance();

public:
  bool Init(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContent);
  void Release();

public:
  // 默认状态，开启透明
  CComPtr<ID3D11BlendState> pBlendStateTransparent;

  // 不修改后台缓存。场景：只填充模板缓存。
  CComPtr<ID3D11BlendState> pBlendStateDisableWriteRenderTarget;

  // 禁用模板缓存
  CComPtr<ID3D11DepthStencilState> pStencilStateDisable;

  // 修改模板缓存。场景：设置/填充剪裁区域
  CComPtr<ID3D11DepthStencilState> pStencilStateCreateClip;

  // 在模板缓存的基础上，绘制其它对象。场景：剪裁
  CComPtr<ID3D11DepthStencilState> pStencilStateClip;

  CComPtr<ID3D11RasterizerState> pRasterizerState;
};

}
#endif