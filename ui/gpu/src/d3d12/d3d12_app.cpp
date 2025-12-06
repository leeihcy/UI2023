#include "src/d3d12/d3d12_app.h"

namespace ui {

D3D12Application &D3D12Application::GetInstance() {
  static D3D12Application s_app;
  return s_app;
}

bool D3D12Application::Startup() {
  UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
  // Enable the debug layer (requires the Graphics Tools "optional feature").
  // NOTE: Enabling the debug layer after device creation will invalidate the
  // active device.
  {
    CComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
      debugController->EnableDebugLayer();

      // Enable additional debug layers.
      dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
  }
#endif

  HRESULT hr = S_OK;

  hr = CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(IDXGIFactory1),
                          (void **)&m_dxgi_factory);
  if (FAILED(hr)) {
    return false;
  }

  int highest_score = 0;
  CComPtr<IDXGIAdapter1> highest_score_adapter;

  UINT i = 0;
  CComPtr<IDXGIAdapter1> adapter;
  while (m_dxgi_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND) {
    int score = 0;

    DXGI_ADAPTER_DESC1 desc;
    adapter->GetDesc1(&desc);

#ifdef _DEBUG
    wchar_t text[256] = { 0 };
    wsprintf(text, L"Device found: %s\n", desc.Description);
    ::OutputDebugStringW(text);
#endif
    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
    } else {
      // 优先选择独显
      score += 1000;
    }

    if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_2,
                                    _uuidof(ID3D12Device), nullptr))) {
      // 是否支持D3D12
      score += 10000;
    }

    if (score > highest_score) {
      highest_score = score;
      highest_score_adapter = adapter;
    }
    adapter.Release();
    ++i;
  }

  if (!highest_score_adapter) {
    return false;
  }
  hr = D3D12CreateDevice(highest_score_adapter, D3D_FEATURE_LEVEL_12_2,
                         IID_PPV_ARGS(&m_device));
  if (FAILED(hr)) {
    return false;
  }

  return true;
}
void D3D12Application::Shutdown() {
  m_dxgi_factory.Release();

#if defined(_DEBUG)
  ID3D12DebugDevice* debugDevice = nullptr;
  m_device->QueryInterface(&debugDevice);
  m_device.Release();

  if (debugDevice) {
      debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
      debugDevice->Release();
  }
#else
  m_device.Release();
#endif
}


} // namespace ui