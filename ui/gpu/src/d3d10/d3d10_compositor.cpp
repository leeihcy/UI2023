#include "src/d3d10/d3d10_compositor.h"
#include "gpu/include/api.h"
#include "src/d3d10/D3D10_app.h"
#include "src/d3d10/d3d10_gpu_layer.h"
#include <D3dx9math.h>

namespace ui {

D3D10Compositor::D3D10Compositor() {
  // m_pRootTexture = nullptr;
  m_sizeBackBuffer.cx = m_sizeBackBuffer.cy = 8;
}

D3D10Compositor::~D3D10Compositor() {
  if (D3D10Application::GetInstance().IsActiveSwapChain(m_hWnd)) {
    D3D10Application::GetInstance().m_device->OMSetRenderTargets(0, nullptr,
                                                                 nullptr);
    D3D10Application::GetInstance().SetActiveSwapChain(nullptr);
  }

  m_pDepthStencilBuffer.Release();
  m_pDepthStencilView.Release();
  m_pRenderTargetView.Release();
  m_pSwapChain.Release();
}

bool D3D10Compositor::Initialize(IGpuCompositorWindow *w) {
  assert(w->GetType() == GpuCompositorWindowType::WindowsHWND);
  m_hWnd = (HWND)((IGpuCompositorWindowHWND *)w)->GetHWND();
  CreateSwapChain();
  return true;
}

static void ReleaseGpuLayer(IGpuLayer *p) {
  if (p) {
    static_cast<D3D10GpuLayer *>(p)->Release();
  }
}
std::shared_ptr<IGpuLayer> D3D10Compositor::CreateLayerTexture() {
  auto *layer = new D3D10GpuLayer();
  layer->SetGpuCompositor(this);
  auto p = std::shared_ptr<IGpuLayer>(layer, ReleaseGpuLayer);
  return p;
}

// void D3D10Compositor::SetRootLayerTexture(IGpuLayer *p) {
//   m_pRootTexture = static_cast<GpuLayer *>(p);
// }
// GpuLayer *D3D10Compositor::GetRootLayerTexture() { return m_pRootTexture; }
void D3D10Compositor::CreateSwapChain() {
  if (!m_hWnd)
    return;
  if (!D3D10Application::GetInstance().m_dxgi_factory)
    return;

  RECT rc;
  GetClientRect(m_hWnd, &rc);
  if (rc.right == rc.left || rc.bottom == rc.top)
    return;

  DXGI_SWAP_CHAIN_DESC swapDesc;
  ::ZeroMemory(&swapDesc, sizeof(swapDesc));

  m_sizeBackBuffer.cx = rc.right - rc.left;
  m_sizeBackBuffer.cy = rc.bottom - rc.top;

  swapDesc.BufferDesc.Width = m_sizeBackBuffer.cx;
  swapDesc.BufferDesc.Height = m_sizeBackBuffer.cy;
  swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapDesc.BufferDesc.RefreshRate.Numerator = 60;
  swapDesc.BufferDesc.RefreshRate.Denominator = 1;
  swapDesc.SampleDesc.Count = 4;

  UINT quality = D3D10Application::GetInstance().GetDeviceMultisampleQuality();
  if (quality <= 0) {
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;
  } else {
    swapDesc.SampleDesc.Quality = quality - 1;
  }

  swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapDesc.BufferCount = 1;
  swapDesc.OutputWindow = m_hWnd;
  swapDesc.Windowed = TRUE;

  /*HRESULT hr = */ D3D10Application::GetInstance()
      .m_dxgi_factory->CreateSwapChain(D3D10Application::GetInstance().m_device,
                                       &swapDesc, &m_pSwapChain);

  if (!m_pSwapChain)
    return;

  ReCreateRenderTargetView();
  ReCreateStencilView();

  D3D10_VIEWPORT vp = {
      0, 0, (UINT)m_sizeBackBuffer.cx, (UINT)m_sizeBackBuffer.cy, 0, 1};
  D3D10Application::GetInstance().m_device->RSSetViewports(1, &vp);
}

void D3D10Compositor::ReCreateRenderTargetView() {
  if (m_pRenderTargetView) {
    D3D10Application::GetInstance().m_device->OMSetRenderTargets(0, nullptr,
                                                                 nullptr);
    m_pRenderTargetView.Release();
  }

  ID3D10Texture2D *pBuffer = nullptr;
  HRESULT hr =
      m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (VOID **)&pBuffer);
  if (SUCCEEDED(hr)) {
    // We need to create a render target view because
    // we would like to bind the back buffer
    // of our swap chain as a render target, so that
    // Direct3D 10 can render onto it.
    hr = D3D10Application::GetInstance().m_device->CreateRenderTargetView(
        pBuffer, nullptr, &m_pRenderTargetView);
  }
  pBuffer->Release();
}

void D3D10Compositor::ReCreateStencilView() {
  m_pDepthStencilView.Release();
  m_pDepthStencilBuffer.Release();

  D3D10_TEXTURE2D_DESC dsDesc;
  dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  dsDesc.Width = m_sizeBackBuffer.cx;
  dsDesc.Height = m_sizeBackBuffer.cy;
  dsDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
  dsDesc.MipLevels = 1;
  dsDesc.ArraySize = 1;
  dsDesc.CPUAccessFlags = 0;
  dsDesc.SampleDesc.Count = 4;
  UINT quality = D3D10Application::GetInstance().GetDeviceMultisampleQuality();
  if (quality <= 0) {
    dsDesc.SampleDesc.Count = 1;
    dsDesc.SampleDesc.Quality = 0;
  } else {
    dsDesc.SampleDesc.Quality = quality - 1;
  }

  dsDesc.MiscFlags = 0;
  dsDesc.Usage = D3D10_USAGE_DEFAULT;
  HRESULT hr = D3D10Application::GetInstance().m_device->CreateTexture2D(
      &dsDesc, 0, &m_pDepthStencilBuffer);
  assert(SUCCEEDED(hr));

  D3D10Application::GetInstance().m_device->CreateDepthStencilView(
      m_pDepthStencilBuffer, 0, &m_pDepthStencilView);
}

void D3D10Compositor::Resize(int nWidth, int nHeight) {
  if (!m_pSwapChain) {
    CreateSwapChain();
    if (!m_pSwapChain)
      return;
  }

  if (m_sizeBackBuffer.cx == nWidth && m_sizeBackBuffer.cy == nHeight)
    return;

  m_sizeBackBuffer.cx = nWidth;
  m_sizeBackBuffer.cy = nHeight;

  // 如果当前是选中的自己，则下次BeginCommit重新设置
  if (D3D10Application::GetInstance().IsActiveSwapChain(m_hWnd)) {
    D3D10Application::GetInstance().m_device->OMSetRenderTargets(0, nullptr,
                                                                 nullptr);
    D3D10Application::GetInstance().SetActiveSwapChain(nullptr);
  }

  // 光ResizeBuffers有问题，还得重新创建rendertargetview
  // Release all outstanding references to the swap chain's buffers.
  m_pRenderTargetView.Release();

  m_pSwapChain->ResizeBuffers(1, nWidth, nHeight, DXGI_FORMAT_B8G8R8A8_UNORM,
                              0);

  ReCreateRenderTargetView();
  ReCreateStencilView();
}

bool D3D10Compositor::BeginCommit(GpuLayerCommitContext *) {
  if (!m_pSwapChain)
    return false;

  if (!D3D10Application::GetInstance().IsActiveSwapChain(m_hWnd)) {
    D3D10_VIEWPORT vp = {
        0, 0, (UINT)m_sizeBackBuffer.cx, (UINT)m_sizeBackBuffer.cy, 0, 1};
    D3D10Application::GetInstance().m_device->RSSetViewports(1, &vp);

    const int size = 1;
    ID3D10RenderTargetView* array[size] = {
      m_pRenderTargetView
    };

    D3D10Application::GetInstance().m_device->OMSetRenderTargets(
        size, array, m_pDepthStencilView);

    D3DXMATRIX matrix;
    D3DXMatrixOrthoOffCenterLH(&matrix, 0, (float)m_sizeBackBuffer.cx,
                               (float)m_sizeBackBuffer.cy, 0, -2000.f, 2000.f);
    Effects::GetInstance().m_pFxOrthMatrix->SetMatrix((float *)&matrix);

    D3D10Application::GetInstance().SetActiveSwapChain(m_hWnd);
  }

  // TBD: 其实大部分情况都不需要CLEAR，但如果背景是透明的，则需要CLEAR，
  //      否则会导致背景重叠

  // ??? 使用{0,0,0,0}会导致渲染不正常
  FLOAT rgba[4] = {0, 0, 0, 1}; // {1, 0, 0, 1};
  D3D10Application::GetInstance().m_device->ClearRenderTargetView(
      m_pRenderTargetView, rgba);

  ClearStencil();

  // 还原剪裁区,防止默认全部被剪裁（后期优化），出现过一次由于注释了设置剪裁代码导致的全部不显示问题。
  D3D10_RECT rects;
  SetRect((LPRECT)&rects, 0, 0, m_sizeBackBuffer.cx, m_sizeBackBuffer.cy);
  D3D10Application::GetInstance().m_device->RSSetScissorRects(1, &rects);

  return true;
}

void D3D10Compositor::EndCommit(GpuLayerCommitContext *) {
  Font::GetInstance().DrawDebugFps();

  //     if (m_pRootTexture)
  //         m_pRootTexture->Commit();

  // TODO: 什么场景会触发Dx10 lost device?
  //  修改分辨率、颜色深度、锁屏都没出现什么问题
  HRESULT hr = m_pSwapChain->Present(1, 0);
  if (DXGI_STATUS_OCCLUDED == hr) {
    // There is a window covering our entire rendering area.
    // Don't render until we're visible again.
    // 或者将窗口的大小拉伸为0时，也会出现这个返回值
  } else if (DXGI_ERROR_DEVICE_RESET == hr) {
    // If a mode change happened, we must reset the device
  } else if (DXGI_ERROR_DEVICE_REMOVED == hr) {
  } else if (SUCCEEDED(hr)) {
  }
}

void D3D10Compositor::ClearStencil() {
  assert(m_pDepthStencilView);
  D3D10Application::GetInstance().m_device->ClearDepthStencilView(
      m_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
} 

} // namespace ui