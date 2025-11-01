#ifndef _UI_GPU_SRC_D3d10_COMMON_EFFECTS_H_
#define _UI_GPU_SRC_D3d10_COMMON_EFFECTS_H_

#include "src/d3d11/inc.h"
#include "src/d3d11/directx_sdk/Effects11/Inc/d3dx11effect.h"

namespace ui {

class Effects {
public:
  bool Init(ID3D11Device *device);
  void Release();

  static Effects& GetInstance();

public:
  CComPtr<ID3DX11Effect> m_pEffect;

  // Technique: RenderUI
  ID3DX11EffectTechnique *m_pTechDrawTexture = nullptr;
  // Technique: RenderUI without texture
  ID3DX11EffectTechnique *m_pTechDrawRect = nullptr;
  ID3DX11EffectTechnique *m_pTechFillRect = nullptr;
  ID3DX11EffectTechnique *m_pTechFillRectMatrix = nullptr;
  ID3DX11EffectTechnique *m_pTechDrawTextureMatrix = nullptr;

  ID3DX11EffectShaderResourceVariable *m_pFxTexture10 = nullptr;
  ID3DX11EffectMatrixVariable *m_pFxMatrix = nullptr;
  ID3DX11EffectVectorVariable *m_pFxVsDestPos = nullptr;
  ID3DX11EffectMatrixVariable *m_pFxOrthMatrix = nullptr;
  ID3DX11EffectScalarVariable *m_pFxAlpha = nullptr;
};
}
#endif