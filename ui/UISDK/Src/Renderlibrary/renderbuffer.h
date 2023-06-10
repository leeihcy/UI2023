#pragma once


namespace UI
{
	class ImageData;

// gdi gdi+ 使用的双缓存
class RenderBuffer
{
public:
	RenderBuffer();
	~RenderBuffer();

public:
    void  Resize(unsigned int width, unsigned int height);
    void  Clear(DWORD dwColor, RECT* prc);
	void  Destroy();
    void  Dump(LPCTSTR);
	void  GetImageData(ImageData*  pData);
    void  Upload2Gpu(IGpuRenderLayer* p, LPRECT prcArray, int nCount);

    static bool IsTopDownBitmap();

public:
    GRAPHICS_RENDER_LIBRARY_TYPE  m_eGraphicsType;  // 同时支持gdi/gdi+
	HBITMAP  m_hBitmap;
    HDC      m_hDC;

    unsigned int  m_nWidth;
    unsigned int  m_nHeight;
};

}