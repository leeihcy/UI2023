#pragma once

class Font
{
public:
	Font();
	~Font();
	static void Init(ID3D10Device* pDevice);
	static void Release();
	static void DrawDebugFps();

	void  DrawText(
		LPCTSTR text, int len, LPCRECT prc, COLORREF color, DWORD dwFlags);

private:
	void  beginDraw();
	void  endDraw();
	ID3DX10Font* getFont();

	void  destroy();

private:
	static ID3DX10Sprite*   s_pSprite10;
	static ID3D10BlendState*  s_pFontBlendState10;

	// 代码不要直接引用这个变量，应该使用getFont()
	ID3DX10Font* __m_pFont10__ = nullptr;  

	struct FontInfo
	{
		INT    Height;
		UINT   Width;
		UINT   Weight;
		UINT   MipLevels;
		BOOL   Italic;
		UINT   CharSet;
		UINT   OutputPrecision;
		UINT   Quality;
		UINT   PitchAndFamily;
		LPCWSTR  pFaceName[32];
	};
	FontInfo  m_fontInfo = { 
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