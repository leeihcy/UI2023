#ifndef _UI_GPU_SRC_D3D11_COMMON_FONT_H_
#define _UI_GPU_SRC_D3D11_COMMON_FONT_H_

#include "src/d3d11/inc.h"

namespace d3d11 {

class Font {
public:
  static Font& GetInstance();

public:
  ~Font();

  void Init(ID3D11Device *pDevice);
  void Release();
  void DrawDebugFps();

  void DrawText(LPCTSTR text, int len, LPCRECT prc, COLORREF color,
                DWORD dwFlags);

private:
  void beginDraw();
  void endDraw();
  void destroy();

#if 0  
  ID3DX11Font *getFont();

private:
  CComPtr<ID3DX11Sprite> s_pSprite10;
  CComPtr<ID3D11BlendState> s_pFontBlendState10;

  // 代码不要直接引用这个变量，应该使用getFont()
  CComPtr<ID3DX11Font> __m_pFont10__;
#endif

  struct FontInfo {
    INT Height;
    UINT Width;
    UINT Weight;
    UINT MipLevels;
    BOOL Italic;
    UINT CharSet;
    UINT OutputPrecision;
    UINT Quality;
    UINT PitchAndFamily;
    LPCWSTR pFaceName[32];
  };
  FontInfo m_fontInfo = {
      15,
      0,
      FW_BOLD,
      1,
      FALSE,
      DEFAULT_CHARSET,
      OUT_DEFAULT_PRECIS,
      DEFAULT_QUALITY,
      DEFAULT_PITCH | FF_DONTCARE,
      L"宋体",
  };
};

} // namespace ui

#endif