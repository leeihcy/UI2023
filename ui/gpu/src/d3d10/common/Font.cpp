#include "font.h"
#include "src/d3d10/common_def.h"
#include "src/d3d10/d3d10_app.h"

namespace d3d10 {

static Font s_fontForDebug;

Font &Font::GetInstance() { return ui::D3D10Application::GetInstance().m_font; }

Font::~Font() { destroy(); }

void Font::destroy() { __m_pFont10__.Release(); }

void Font::Init(ID3D10Device *pDevice) {
  if (!pDevice)
    return;

  D3DX10CreateSprite(pDevice, 512, &s_pSprite10);

  D3D10_BLEND_DESC StateDesc;
  ZeroMemory(&StateDesc, sizeof(D3D10_BLEND_DESC));
  StateDesc.AlphaToCoverageEnable = FALSE;
  StateDesc.BlendEnable[0] = TRUE;
  StateDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
  StateDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
  StateDesc.BlendOp = D3D10_BLEND_OP_ADD;
  StateDesc.SrcBlendAlpha = D3D10_BLEND_ZERO;
  StateDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
  StateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
  StateDesc.RenderTargetWriteMask[0] = 0xf;
  pDevice->CreateBlendState(&StateDesc, &s_pFontBlendState10);

  s_fontForDebug.m_fontInfo.Height = 12;
}

void Font::Release() {
  s_pSprite10.Release();
  s_pFontBlendState10.Release();

  s_fontForDebug.destroy();
}

void Font::DrawDebugFps() {
  // static StopWatch watch;
  // static int frames = 0;
  // static __int64 lastCalcFpsTime = 0;
  // static float fps = 0;
  // frames++;

  // __int64 dwTickNow = watch.Now();
  // if (dwTickNow - lastCalcFpsTime > 1000)
  // {
  // 	fps = (float)frames / (float)(dwTickNow-lastCalcFpsTime) * 1000;
  // 	lastCalcFpsTime = dwTickNow;
  // 	frames = 0;
  // }

  // s_fontForDebug.beginDraw();

  // TCHAR buf[64];
  // wprintf(buf, TEXT("%0.2f fps"), fps);

  // RECT rc = { 0, 0, 0, 0 };
  // s_fontForDebug.DrawText(buf, -1, &rc, RGB(255, 0, 0), DT_NOCLIP);
  // s_fontForDebug.endDraw();
}

ID3DX10Font *Font::getFont() {
  if (__m_pFont10__) {
    return __m_pFont10__;
  }

  CComPtr<ID3D10Device> pDevice;
  s_pSprite10->GetDevice(&pDevice);
  assert(pDevice);
  if (!pDevice)
    return nullptr;

  D3DX10CreateFont(pDevice, m_fontInfo.Height, m_fontInfo.Width,
                   m_fontInfo.Weight, m_fontInfo.MipLevels, m_fontInfo.Italic,
                   m_fontInfo.CharSet, m_fontInfo.OutputPrecision,
                   m_fontInfo.Quality, m_fontInfo.PitchAndFamily,
                   (LPCWSTR)m_fontInfo.pFaceName, &__m_pFont10__);

  return __m_pFont10__;
}

void Font::DrawText(LPCTSTR text, int len, LPCRECT prc, COLORREF color,
                    DWORD dwFlags) {
  ID3DX10Font *font = getFont();
  if (!font)
    return;

  D3DXCOLOR dxcolor(D3DCOLOR_COLORREF(color));
  font->DrawText(s_pSprite10, text, len, (LPRECT)prc, dwFlags, dxcolor);
}

void Font::beginDraw() {
  HRESULT hr = s_pSprite10->Begin(D3DX10_SPRITE_SAVE_STATE);

#if 0
	D3D10_VIEWPORT VPs[D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	UINT cVPs = 1;
	ID3D10Device* pd3dDevice = NULL;
	s_pSprite10->GetDevice(&pd3dDevice);
	if (pd3dDevice)
	{
		// Set projection
		pd3dDevice->RSGetViewports(&cVPs, VPs);
		D3DXMATRIXA16 matProjection;
		D3DXMatrixOrthoOffCenterLH(&matProjection, (FLOAT)VPs[0].TopLeftX, (FLOAT)
			(VPs[0].TopLeftX + VPs[0].Width), (FLOAT)VPs[0].TopLeftY, (FLOAT)
			(VPs[0].TopLeftY + VPs[0].Height), 0.1f, 10);
		s_pSprite10->SetProjectionTransform(&matProjection);

		s_pSprite10->Begin(/*D3DX10_SPRITE_SORT_TEXTURE*/D3DX10_SPRITE_SORT_TEXTURE);
		SAFE_RELEASE(pd3dDevice);
	}
#endif
}

void Font::endDraw() {
  s_pSprite10->End();
#if 0
	FLOAT OriginalBlendFactor[4];
	UINT OriginalSampleMask = 0;
	ID3D10BlendState* pOriginalBlendState10 = NULL;
	ID3D10Device* pd3dDevice = NULL;

 	s_pSprite10->GetDevice(&pd3dDevice);
 	if (pd3dDevice)
 	{
 		// Get the old blend state and set the new one
 		pd3dDevice->OMGetBlendState(&pOriginalBlendState10, OriginalBlendFactor, &OriginalSampleMask);
 		if (s_pFontBlendState10)
 		{
 			FLOAT NewBlendFactor[4] = { 0,0,0,0 };
 			pd3dDevice->OMSetBlendState(s_pFontBlendState10, NewBlendFactor, 0xffffffff);
 		}
 	}
	//s_pSprite10->Flush();
	s_pSprite10->End();

	// Reset the original blend state
	if (pd3dDevice && pOriginalBlendState10)
	{
		pd3dDevice->OMSetBlendState(pOriginalBlendState10, OriginalBlendFactor, OriginalSampleMask);
	}
	SAFE_RELEASE(pOriginalBlendState10);
	SAFE_RELEASE(pd3dDevice);
#endif
}

} // namespace ui