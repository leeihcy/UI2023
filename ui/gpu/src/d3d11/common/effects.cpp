#include "effects.h"
#include "src/d3d11/resource.h"
#include "src/d3d11/d3d11_app.h"

namespace ui {

// static 
Effects& Effects::GetInstance() {
  return D3D11Application::GetInstance().m_effects;
}

bool Effects::Init(ID3D11Device *pDevice) {
  // Create Effect
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

  HRESULT hCompileResult = 0;
  ID3DBlob *pCompileBlob = nullptr;

  HINSTANCE hInstance = GetModuleHandle(L"uigpu.dll");

#ifdef _DEBUG
  TCHAR szPath[MAX_PATH] = {0};
  GetModuleFileName(hInstance, szPath, MAX_PATH);
  TCHAR *p = wcsrchr(szPath, L'\\');
  if (p) {
    *(p + 1) = 0;
  }

  wcscat(szPath, TEXT("..\\..\\..\\ui\\gpu\\src\\d3d11\\d3d11.fx"));
  CComPtr<ID3DBlob> pErrorBlob;
  HRESULT hr = D3DX11CompileFromFile(szPath, nullptr, nullptr, nullptr,
                                     "fx_5_0", dwShaderFlags, 0, NULL,
                                     &pCompileBlob, &pErrorBlob, 0);
  if (SUCCEEDED(hr)) {
    hr = D3DX11CreateEffectFromMemory(pCompileBlob->GetBufferPointer(),
                                      pCompileBlob->GetBufferSize(), 0, pDevice,
                                      &m_pEffect);
  }
#else
  // 必须放在 RCDATA 资源当中
  HRESULT hr = D3DX11CreateEffectFromResource(
      hInstance, MAKEINTRESOURCE(fx_4_0), 0, nullptr, nullptr, "fx_4_0",
      dwShaderFlags, 0, pDevice, nullptr, nullptr, &m_pEffect, &pCompileBlob,
      &hCompileResult);
#endif
  if (pErrorBlob != NULL) {
    OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
  }
  assert(SUCCEEDED(hr));
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

  m_pEffect.Release();
}

}