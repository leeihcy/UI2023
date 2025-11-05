#ifndef _UI_GPU_SRC_D3D12_D3D12APP_H_
#define _UI_GPU_SRC_D3D12_D3D12APP_H_
#include "src/d3d12/inc.h"

namespace ui {

class D3D12Application {
public:
  static D3D12Application& GetInstance();
public:
  bool Startup();
  void Shutdown();

public:
  CComPtr<IDXGIFactory1> m_dxgi_factory;
  CComPtr<ID3D12Device> m_device;
};

}
#endif