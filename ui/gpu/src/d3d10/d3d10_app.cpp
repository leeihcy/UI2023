#include "inc.h"
#include <libloaderapi.h>
#include <minwindef.h>
#pragma warning(disable : 4005) // 宏重定义

#include "src/d3d10/inc.h"
#include <dxgidebug.h>
#include <windows.h>

#include "D3D10_App.h"
#include "common/effects.h"
#include "common/Font.h"
#include "common/render_states.h"
#include <assert.h>

namespace ui {

static D3D10Application s_app;

// static
D3D10Application &D3D10Application::GetInstance() { return s_app; }

bool D3D10Application::Startup() {
  if (m_device) {
    assert(false);
    return true;
  }

  // 若要使用 Direct2D，必须使用 D3D10_CREATE_DEVICE_BGRA_SUPPORT
  // 标志创建提供 IDXGISurface 的 Direct3D 设备。
  UINT nCreateDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
  nCreateDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

  HRESULT hr = 0;
  hr = createDevice(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nCreateDeviceFlags,
                    &m_device);
  if (!m_device) {
    return false;
  }

  if (FAILED(hr)) {
    hr = D3D10CreateDevice(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr,
                           nCreateDeviceFlags, D3D10_SDK_VERSION, &m_device);
  }

  CComPtr<IDXGIDevice> pDXGIDevice;
  CComPtr<IDXGIAdapter> pAdapter;
  hr = m_device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
  hr = pDXGIDevice->GetAdapter(&pAdapter);
  hr = pAdapter->GetParent(IID_PPV_ARGS(&m_dxgi_factory));

  hr = m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4,
                                                &m_multi_sample_quality);

  m_effects.Init(m_device);
  m_inputs.Init(m_device);
  m_render_states.Init(m_device);
  m_font.Init(m_device);

  // Create an orthographic projection matrix for 2D rendering.
  // D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight,
  // screenNear, screenDepth);

  return true;
}

HRESULT D3D10Application::createDevice(IDXGIAdapter *pAdapter,
                                       D3D10_DRIVER_TYPE driverType, UINT flags,
                                       ID3D10Device **ppDevice) {
  HRESULT hr = 0;

  static const D3D10_FEATURE_LEVEL1 levelAttempts[] = {
      D3D10_FEATURE_LEVEL_10_0,
      D3D10_FEATURE_LEVEL_9_3,
      D3D10_FEATURE_LEVEL_9_2,
      D3D10_FEATURE_LEVEL_9_1,
  };

  for (UINT level = 0; level < ARRAYSIZE(levelAttempts); level++) {
    CComPtr<ID3D10Device1> pDevice;
    hr =
        D3D10CreateDevice1(pAdapter, driverType, nullptr, flags,
                           levelAttempts[level], D3D10_1_SDK_VERSION, &pDevice);

    /* 应用程序请求的操作依赖于已缺失或不匹配的 SDK 组件。

     Windows 8 users: If you upgrade to Windows 8.1, remember that all
     attempts to use D3Dxx_CREATE_DEVICE_DEBUG will fail until you
     upgrade the Developer Runtime. You can do this by installing VS
     2013, the standalone Windows 8.1 SDK, or by installing the VS 2013
     Remote Debugging Tools. If you are missing the updated SDK Debug
     Layers, you should see the following message in your debug output:

     D3D11CreateDevice: Flags (0x2) were specified which require the
     D3D11 SDK Layers for Windows 10, but they are not present on
     the system.
     These flags must be removed, or the Windows 10 SDK must be
     installed.
     Flags include: D3D11_CREATE_DEVICE_DEBUG
    */
    if (hr == 0x887a002d && flags & D3D10_CREATE_DEVICE_DEBUG) {
      flags &= ~D3D10_CREATE_DEVICE_DEBUG;
      return createDevice(pAdapter, driverType, false, ppDevice);
    }

    if (SUCCEEDED(hr)) {
      *ppDevice = pDevice.Detach();
      break;
    }
  }

  return hr;
}

void D3D10Application::Shutdown() {
  if (!m_device) {
    assert(false);
    return;
  }

  m_device->ClearState();

  // 释放所有元素的static变量

  m_render_states.Release();
  m_effects.Release();
  m_inputs.Rlease();
  m_font.Release();

  // 	ID3D11Debug* pD3dDebug;
  // 	HRESULT hr = m_device->QueryInterface(__uuidof(ID3D11Debug),
  // reinterpret_cast<void**>(&pD3dDebug)); 	if (SUCCEEDED(hr))
  // 	{
  // 		hr = pD3dDebug->ReportLiveDeviceObjects(D3D10_RLDO_DETAIL);
  // 	}

  m_device.Release();
  m_dxgi_factory.Release();
 
  reportLiveObjects();
}

// 退出时，对象未释放导致的泄露检测。
void D3D10Application::reportLiveObjects() {
  typedef HRESULT (*pfnDXGIGetDebugInterface)(REFIID riid, void **ppDebug);
  HMODULE dxgidebug = LoadLibraryA("dxgidebug.dll");
  if (!dxgidebug) {
    return;
  }

  pfnDXGIGetDebugInterface f = (pfnDXGIGetDebugInterface)GetProcAddress(
      dxgidebug, "DXGIGetDebugInterface");
  if (!f) {
    return;
  }

  CComPtr<IDXGIDebug> debugDev;
  f(IID_PPV_ARGS(&debugDev));
  if (!debugDev) {
    return;
  }

  const GUID DXGI_DEBUG_ALL = {0xe48ae283, 0xda80, 0x490b, 0x87, 0xe6, 0x43,
                               0xe9,       0xa9,   0xcf,   0xda, 0x8};
  debugDev->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
}

void D3D10Application::draw(ID3D10EffectTechnique *pTech,
                     DXUT_SCREEN_VERTEX_10 vertices[4]) {
  // TODO:    Inputs::SetVertex4(vertices);

  D3D10_TECHNIQUE_DESC techDesc;
  pTech->GetDesc(&techDesc);
  for (UINT p = 0; p < techDesc.Passes; ++p) {
    pTech->GetPassByIndex(p)->Apply(0);
    m_device->Draw(4, 0);
  }
}

void D3D10Application::ApplyTechnique(ID3D10EffectTechnique *pTech,
                                      ui::RECTF *prcDraw,
                                      ui::D3DCOLORVALUE color) {
  DXUT_SCREEN_VERTEX_10 vertices[4] = {
      {prcDraw->left, prcDraw->top, 0, color, 0.0f, 0.0f},
      {prcDraw->right, prcDraw->top, 0, color, 1.0f, 0.0f},
      {prcDraw->left, prcDraw->bottom, 0, color, 0.0f, 1.0f},
      {prcDraw->right, prcDraw->bottom, 0, color, 1.0f, 1.0f},
  };
  draw(pTech, vertices);
}
void D3D10Application::ApplyTechnique(ID3D10EffectTechnique *pTech,
                                      ui::RECTF *prcDraw, ui::RECTF *prcTexture,
                                      float fAlpha) {
  // {1,1,1, fAlpha},然后预乘后的结果
  ui::D3DCOLORVALUE color = {fAlpha, fAlpha, fAlpha, fAlpha};

  DXUT_SCREEN_VERTEX_10 vertices[4] = {
      {prcDraw->left, prcDraw->top, 0.f, color, prcTexture->left,
       prcTexture->top},
      {prcDraw->right, prcDraw->top, 0.f, color, prcTexture->right,
       prcTexture->top},
      {prcDraw->left, prcDraw->bottom, 0.f, color, prcTexture->left,
       prcTexture->bottom},
      {prcDraw->right, prcDraw->bottom, 0.f, color, prcTexture->right,
       prcTexture->bottom},
  };
  draw(pTech, vertices);
}

bool D3D10Application::IsActiveSwapChain(HWND hWnd) {
  return m_hActiveWnd == hWnd ? true : false;
}
void D3D10Application::SetActiveSwapChain(HWND hWnd) { m_hActiveWnd = hWnd; }

} // namespace ui