#include "Effects.h"
#include "resource.h"
#include "stdafx.h"

ID3D10Effect *Effects::m_pEffect = nullptr;

ID3D10EffectTechnique *Effects::m_pTechDrawTexture = nullptr;
ID3D10EffectTechnique *Effects::m_pTechDrawRect = nullptr;
ID3D10EffectTechnique *Effects::m_pTechFillRect = nullptr;
ID3D10EffectTechnique *Effects::m_pTechFillRectMatrix = nullptr;
ID3D10EffectTechnique *Effects::m_pTechDrawTextureMatrix = nullptr;

ID3D10EffectShaderResourceVariable *Effects::m_pFxTexture10 = nullptr;
ID3D10EffectMatrixVariable *Effects::m_pFxMatrix = nullptr;
ID3D10EffectMatrixVariable *Effects::m_pFxOrthMatrix = nullptr;
ID3D10EffectVectorVariable *Effects::m_pFxVsDestPos = nullptr;
ID3D10EffectScalarVariable *Effects::m_pFxAlpha = nullptr;

bool Effects::Init(ID3D10Device *pDevice) {
  // Create Effect
  DWORD dwShaderFlags = /*D3D10_SHADER_ENABLE_STRICTNESS |*/
      D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
#ifdef _DEBUG
  dwShaderFlags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

  HRESULT hCompileResult = 0;
  ID3D10Blob *pCompileBlob = nullptr;

#ifdef _DEBUG
  TCHAR szPath[MAX_PATH] = {0};
  GetModuleFileName(g_hInstance, szPath, MAX_PATH);
  TCHAR *p = _tcsrchr(szPath, TEXT('\\'));
  if (p) {
    *(p + 1) = 0;
  }

  _tcscat(szPath, TEXT("..\\..\\ui\\UICompositor\\Src\\d3d10\\d3d10.fx"));

  HRESULT hr = D3DX10CreateEffectFromFile(
      szPath, nullptr, nullptr, "fx_4_0", dwShaderFlags, 0, pDevice, nullptr,
      nullptr, &m_pEffect, &pCompileBlob, &hCompileResult);
#else
  // 必须放在 RCDATA 资源当中
  HRESULT hr = D3DX10CreateEffectFromResource(
      g_hInstance, MAKEINTRESOURCE(fx_4_0), 0, nullptr, nullptr, "fx_4_0",
      dwShaderFlags, 0, pDevice, nullptr, nullptr, &m_pEffect, &pCompileBlob,
      &hCompileResult);
#endif
  UIASSERT(SUCCEEDED(hr));
  if (pCompileBlob) {
    char *pErrorDesc = (char *)pCompileBlob->GetBufferPointer();
    MessageBoxA(nullptr, pErrorDesc, "load fx failed.", MB_OK | MB_ICONWARNING);
    SAFE_RELEASE(pCompileBlob);
  }

  if (FAILED(hr)) {
    return false;
  }

  m_pTechDrawTexture = m_pEffect->GetTechniqueByName("DrawTexture");
  m_pTechFillRect = m_pEffect->GetTechniqueByName("FillRect");
  m_pTechDrawRect = m_pEffect->GetTechniqueByName("DrawRect");
  m_pTechFillRectMatrix = m_pEffect->GetTechniqueByName("FillRectMatrix");
  m_pTechDrawTextureMatrix = m_pEffect->GetTechniqueByName("DrawTextureMatrix");
  m_pFxTexture10 =
      m_pEffect->GetVariableByName("g_Texture")->AsShaderResource();
  m_pFxMatrix = m_pEffect->GetVariableByName("g_Matrix")->AsMatrix();
  m_pFxOrthMatrix = m_pEffect->GetVariableByName("g_orthMatrix")->AsMatrix();
  m_pFxVsDestPos = m_pEffect->GetVariableByName("g_vsDestPos")->AsVector();
  m_pFxAlpha = m_pEffect->GetVariableByName("g_alpha")->AsScalar();

  return true;
}

void Effects::Release() {
  m_pTechDrawTexture = nullptr;
  m_pTechDrawRect = nullptr;
  m_pTechFillRect = nullptr;
  m_pFxTexture10 = nullptr;
  m_pTechFillRectMatrix = nullptr;
  m_pTechDrawTextureMatrix = nullptr;
  m_pFxOrthMatrix = nullptr;
  m_pFxMatrix = nullptr;
  m_pFxVsDestPos = nullptr;

  SAFE_RELEASE(m_pEffect);
}