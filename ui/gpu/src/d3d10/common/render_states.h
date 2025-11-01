#ifndef _UI_GPU_SRC_D3D10_COMMON_RENDERSTATES_H_
#define _UI_GPU_SRC_D3D10_COMMON_RENDERSTATES_H_

#include "src/d3d10/inc.h"

namespace ui {

class RenderStates {
public:
  static RenderStates& GetInstance();

public:
  bool Init(ID3D10Device *device);
  void Release();

public:
  // 默认状态，开启透明
  CComPtr<ID3D10BlendState> pBlendStateTransparent;

  // 不修改后台缓存。场景：只填充模板缓存。
  CComPtr<ID3D10BlendState> pBlendStateDisableWriteRenderTarget;

  // 禁用模板缓存
  CComPtr<ID3D10DepthStencilState> pStencilStateDisable;

  // 修改模板缓存。场景：设置/填充剪裁区域
  CComPtr<ID3D10DepthStencilState> pStencilStateCreateClip;

  // 在模板缓存的基础上，绘制其它对象。场景：剪裁
  CComPtr<ID3D10DepthStencilState> pStencilStateClip;

  CComPtr<ID3D10RasterizerState> pRasterizerState;
};

}
#endif