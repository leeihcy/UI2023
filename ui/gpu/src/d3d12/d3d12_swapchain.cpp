#include "src/d3d12/d3d12_swapchain.h"
#include "src/d3d12/d3d12_app.h"

namespace ui {

bool D3D12SwapChain::Create(HWND hwnd) {
  CComPtr<IDXGIFactory2> factory2;
  HRESULT hr = D3D12Application::GetInstance().m_dxgi_factory->QueryInterface(
      __uuidof(IDXGIFactory2), (void**)&factory2);
  if (FAILED(hr)) {
    return false;
  }

  RECT rc;
  ::GetClientRect(hwnd, &rc);

  static const UINT FrameCount = 2;

  DXGI_SWAP_CHAIN_DESC1 desc = {
      .Width = (UINT)(rc.right - rc.left),
      .Height = (UINT)(rc.bottom - rc.top),
      .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
      .Stereo = FALSE,
      .SampleDesc = {.Count = 1, .Quality = 0},
      .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
      .BufferCount = FrameCount,
      .Scaling = DXGI_SCALING_STRETCH,
      .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
      .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
      .Flags = 0,
  };

  CComQIPtr<IDXGISwapChain1> swapchain;
  hr = factory2->CreateSwapChainForHwnd(
      D3D12Application::GetInstance().m_device, hwnd, &desc, nullptr, nullptr,
      &swapchain);
  if (FAILED(hr)) {
    return false;
  }
  hr = swapchain.QueryInterface(&m_swapchain);
  if (FAILED(hr)) {
    return false;
  }

  factory2->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
  m_frame_index = m_swapchain->GetCurrentBackBufferIndex();

  return true;
}

} // namespace ui