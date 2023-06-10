#pragma once

/*

// ����һ��Բ�ε�ͷ��
void  CreateCirclePortrait(__in Gdiplus::Bitmap* pBitmap, int nPortraitSize, __out CImage& image)
{
    image.Destroy();
    if (!pBitmap)
        return;

    image.Create(nPortraitSize, -nPortraitSize, 32, 
            CImage::createAlphaChannel);

    HDC hDC = image.GetDC();
    {
        Graphics g(hDC);
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(PixelOffsetModeHalf);   // <-- ���������������򻭳�����ͼƬ��Ե����ȷ

        RectF dst(0, 0, (REAL)nPortraitSize, (REAL)nPortraitSize);
        g.DrawImage(pBitmap, dst, 0, 0,
            (REAL)pBitmap->GetWidth(),
            (REAL)pBitmap->GetHeight(), UnitPixel);
    }
    image.ReleaseDC();

    g_Circle24_24RoundRectHandler.Set(nPortraitSize/2);
    g_Circle24_24RoundRectHandler.Apply2Bitmap(image, true, nullptr);
}
*/

namespace UI
{

class RoundRectHandler
{
public:
    RoundRectHandler();
    ~RoundRectHandler();

    void  Set(int n);
    void  Set(REGION4* r);
    void  Set(int nLeftTop, int nRightTop, int nLeftBottom, int nRightBottom);

    bool  Apply2Bitmap(
            HBITMAP hBitmap, 
            bool bTopDown, 
            RECT* prcRegion,
            RECT* prcClip);
    
protected:
    UINT  GetMinWidth();
    UINT  GetMinHeight();
    void  CreatePath(Gdiplus::GraphicsPath* pPath);
    void  CreateMaskBitmap();
    void  RequestBuffer(UINT nWidth, UINT nHeight);
    void  DestroyBuffer();

	struct CORNER_DATA
	{
		byte* pDstBits; int nDstPitch; 
		byte* pSrcBits; int nSrcPitch; 
		int xDst; int yDst;
		int xSrc; int ySrc;
		int width; int height;
		bool bClipMode;  // �ü�ģʽ���棬�޷����жԽ����Ż�����
	};

    // ע�������ĸ�Բ��ʱ����ʵֻ��Ҫ����һ�뼴�ɣ������ε�һ�룬һ�����ǿ���
    //     �����ѭ�������У�ͨ������xLoop��ʵ��ֻ����һ�������
    void  HandleLeftTopCorner(CORNER_DATA& data);
    void  HandleLeftBottomCorner(CORNER_DATA& data);
    void  HandleRightTopCorner(CORNER_DATA& data);
    void  HandleRightBottomCorner(CORNER_DATA& data);


//     void HandleXLine(byte* pDstBits, int xFrom, int xTo);
//     void HandleYLine(byte* pDstBits, int nDstPitch, int yFrom, int yTo);

private:
    // �ĸ�Բ�ǵİ뾶
    int  lefttop;
    int  righttop;
    int  leftbottom;
    int  rightbottom;

//    bool  m_bHandleBorder;  // �Ƿ�����������

    HBITMAP  m_hBitmap;
    UINT     m_nBitmapWidth;
    UINT     m_nBitmapHeigth;
    UINT     m_nBitmapAvailableWidth;   // ����ͼƬ��������ֻʹ����m_hBitmap�����Ͻǵ�һ����
    UINT     m_nBitmapAvailableHeight;
    UINT     m_nPitch;
    byte*    m_pBits;

    HDC      m_hDC;
};

bool  CreateRoundRectPath2(Gdiplus::GraphicsPath* pPath, RECT* prc);
bool  CreateRoundRectPath(Gdiplus::GraphicsPath* pPath, RECT* prc, int nRadius);
bool  CreateRoundRectPath3(Gdiplus::GraphicsPath* pPath, RECT* prc, 
                           int nRadiusLT, int nRadiusRT, int nRadiusRB, int nRadiusLB);
bool  CreateRoundRectPath4(Gdiplus::GraphicsPath* pPath, RECT* prc, REGION4* pRadius);

extern RoundRectHandler  g_CommonRoundRectHandler;       // ��ͨԲ�ǣ�����panelԲ��
extern RoundRectHandler  g_Circle24_24RoundRectHandler;  // Բ��ͷ��
}