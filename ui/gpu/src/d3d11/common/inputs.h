#ifndef _UI_GPU_SRC_D3D11_COMMON_INPUTS_H_
#define _UI_GPU_SRC_D3D11_COMMON_INPUTS_H_
#include "src/d3d11/inc.h"

namespace ui {

// 对应fx文件中的VS()参数
struct DXUT_SCREEN_VERTEX_10 {
  float x, y, z;
  ui::D3DCOLORVALUE color;
  float tu, tv;
};

class Inputs {
public:
  static Inputs& GetInstance();

public:
  bool Init(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContent);
  void Rlease();

  // static void SetVertex4(DXUT_SCREEN_VERTEX_10 vertices[4]);

  CComPtr<ID3D11InputLayout> m_pInputLayout;
  // static ID3D11Buffer*  m_pVertexBuffer;
};

}
#endif