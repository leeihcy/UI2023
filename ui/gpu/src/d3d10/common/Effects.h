#ifndef _UI_GPU_SRC_D3d10_COMMON_EFFECTS_H_
#define _UI_GPU_SRC_D3d10_COMMON_EFFECTS_H_

#include "src/d3d10/inc.h"

namespace ui {

class Effects {
public:
  bool Init(ID3D10Device *device);
  void Release();

  static Effects& GetInstance();

public:
  CComPtr<ID3D10Effect> m_pEffect;

  // Technique: RenderUI
  ID3D10EffectTechnique *m_pTechDrawTexture = nullptr;
  // Technique: RenderUI without texture
  ID3D10EffectTechnique *m_pTechDrawRect = nullptr;
  ID3D10EffectTechnique *m_pTechFillRect = nullptr;
  ID3D10EffectTechnique *m_pTechFillRectMatrix = nullptr;
  ID3D10EffectTechnique *m_pTechDrawTextureMatrix = nullptr;

  ID3D10EffectShaderResourceVariable *m_pFxTexture10 = nullptr;
  ID3D10EffectMatrixVariable *m_pFxMatrix = nullptr;
  ID3D10EffectVectorVariable *m_pFxVsDestPos = nullptr;
  ID3D10EffectMatrixVariable *m_pFxOrthMatrix = nullptr;
  ID3D10EffectScalarVariable *m_pFxAlpha = nullptr;
};
}
#endif