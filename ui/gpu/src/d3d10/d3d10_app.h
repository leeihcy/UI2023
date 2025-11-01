#ifndef UI_GPU_SRC_D3D10_D3D10_APP_H_
#define UI_GPU_SRC_D3D10_D3D10_APP_H_
#include "src/d3d10/inc.h"
#include "src/d3d10/common/inputs.h"
#include "src/d3d10/common/effects.h"
#include "src/d3d10/common/font.h"
#include "src/d3d10/common/render_states.h"

// TODO:
// 1. 多屏幕下的device要怎么处理？
// 2. debug字体渲染（帧率）

//
// IA-Input Assembler State
// SO-Stream Output State
// OM-Output Merger State
// VS-Vertex Shader
// PS-Pixel Shader
// GS-Geometry Shader
//

// 资源（纹理）不能被直接绑定到一个管线阶段，我们只能把与资源关联的资源视图绑定到不同的管线阶段。
// 无论以哪种方式使用纹理，Direct3D始终要求我们在初始化时为纹理创建相关的资源视图，这样有助于提高
// 运行效率，正如SDK文档所指出的那样：“运行时环境与驱动程序可以在视图创建时执行相应的验证与映射，
// 减少绑定时的类型检查”。
// Resource
// View是在DirectX10时代引入的概念，它实际上是连接resouce和pipeline绑定点之间的一种适配器对象。同一种资源可以对应多个不同的RV，允许它被绑定到不同的pipline阶段。
// 有四种不同类型的RV，分别对应着pipeline中的4个可绑定位置：
//   Render target view (ID3D11RenderTargetView)
//   Depth stencil view (ID3D11DepthStencilView)
//   Shader resource view (ID3D11ShaderResourceView)
//   Unordered access view (ID3D11UnorderedAccessView)

class Stage3D;
class Direct3DRenderTarget;
struct ID3D10EffectTechnique;
struct IDXGIFactory;

namespace ui {
class D3D10Application {
public:
  static D3D10Application& GetInstance();

public:
  bool Startup();
  void Shutdown();

public:
  unsigned int GetDeviceMultisampleQuality() { return m_multi_sample_quality; }

  bool IsActiveSwapChain(HWND hWnd);
  void SetActiveSwapChain(HWND hWnd);

  void ApplyTechnique(ID3D10EffectTechnique *pTech, ui::RECTF *prcDraw,
                      ui::D3DCOLORVALUE color);
  void ApplyTechnique(ID3D10EffectTechnique *pTech, ui::RECTF *prcDraw,
                      ui::RECTF *prcTexture, float fAlpha);

private:
  HRESULT createDevice(IDXGIAdapter *pAdapter, D3D10_DRIVER_TYPE driverType,
                       UINT flags, ID3D10Device **ppDevice);
  void draw(ID3D10EffectTechnique *pTech, DXUT_SCREEN_VERTEX_10 vertices[4]);
  
  void reportLiveObjects();
  
public:
  CComPtr<ID3D10Device> m_device;
  CComPtr<IDXGIFactory> m_dxgi_factory;

  Inputs m_inputs;
  Effects m_effects;
  RenderStates m_render_states;
  Font m_font;

  // 当前device中被选入的rendertarget所属窗口
  HWND m_hActiveWnd = nullptr;

private:
  UINT m_multi_sample_quality = 0;
};

}
#endif