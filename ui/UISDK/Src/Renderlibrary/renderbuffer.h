#pragma once


namespace ui
{
	class ImageData;

// gdi gdi+ ʹ�õ�˫����
class RenderBuffer
{
public:
	RenderBuffer();
	~RenderBuffer();

public:
    void  Resize(unsigned int width, unsigned int height);
    void  Clear(DWORD dwColor, RECT* prc);
	void  Destroy();
    void  Dump(const wchar_t*);
	void  GetImageData(ImageData*  pData);
    void  Upload2Gpu(IGpuLayer* p, LPRECT prcArray, int nCount);

    static bool IsTopDownBitmap();

public:
    GRAPHICS_RENDER_LIBRARY_TYPE  m_eGraphicsType;  // ͬʱ֧��gdi/gdi+
	HBITMAP  m_hBitmap;
    HDC      m_hDC;

    unsigned int  m_nWidth;
    unsigned int  m_nHeight;
};

}