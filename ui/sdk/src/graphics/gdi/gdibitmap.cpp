#include "stdafx.h"
#include "gdibitmap.h"

void GDIRenderBitmap::CreateInstance(IRenderBitmap** ppOut)
{
	UIASSERT(nullptr != ppOut);
	if (nullptr == ppOut)
		return;

	GDIRenderBitmap* p = new GDIRenderBitmap();
    p->AddRef();
	*ppOut = p;
}

GDIImageListRenderBitmap::GDIImageListRenderBitmap()
{
	m_nCount = 0;
	m_eLayout = IMAGELIST_LAYOUT_TYPE_H;
}
void GDIImageListRenderBitmap::CreateInstance(IRenderBitmap** ppOut)
{
	if (nullptr == ppOut)
		return;

	GDIImageListRenderBitmap* p = new GDIImageListRenderBitmap();
    p->AddRef();
	*ppOut = p;
}

int GDIImageListRenderBitmap::GetItemWidth()
{
	if (0 == m_nCount)
		return 0;

	switch(m_eLayout)
	{
	case IMAGELIST_LAYOUT_TYPE_V:
		return GetWidth();

	case IMAGELIST_LAYOUT_TYPE_H:
		return GetWidth()/m_nCount;
	}

	return 0;
}
int GDIImageListRenderBitmap::GetItemHeight()
{
	if (0 == m_nCount)
		return 0;

	switch(m_eLayout)
	{
	case IMAGELIST_LAYOUT_TYPE_H:
		return GetHeight();

	case IMAGELIST_LAYOUT_TYPE_V:
		return GetHeight()/m_nCount;
	}

	return 0;
}
void  GDIImageListRenderBitmap::SetItemCount(int n)
{
    m_nCount = n;
}
void  GDIImageListRenderBitmap::SetLayoutType(IMAGELIST_LAYOUT_TYPE e)
{
    m_eLayout = e;
}
IMAGELIST_LAYOUT_TYPE GDIImageListRenderBitmap::GetLayoutType()
{
	return m_eLayout;
}
bool GDIImageListRenderBitmap::GetIndexPos(int nIndex, POINT* pPoint)
{
	if (nullptr == pPoint)
		return false;

	pPoint->x = pPoint->y = 0;
	if (nIndex > m_nCount)
		return false;

	if (IMAGELIST_LAYOUT_TYPE_H == m_eLayout)
	{
		pPoint->x = nIndex*GetItemWidth();
		pPoint->y = 0;
	}
	else if (IMAGELIST_LAYOUT_TYPE_V == m_eLayout)
	{
		pPoint->x = 0;
		pPoint->y = nIndex*GetItemHeight();
	}
	else 
		return false;

	return true;
}


GDIIconRenderBitmap::GDIIconRenderBitmap()
{
	m_nIconWidth = m_nIconHeight = 16;
}
void GDIIconRenderBitmap::CreateInstance(IRenderBitmap** ppOut)
{
	UIASSERT(nullptr != ppOut);
	if( nullptr == ppOut )
		return;

	GDIIconRenderBitmap* p = new GDIIconRenderBitmap();
    p->AddRef();
	*ppOut = p;
}

//
// ע��1. Ϊ�˽����16λɫϵͳ�ֱ������棬ϵͳ�������32λ��ͼ������⣬
//     ���������icon��mask bitmap������ͼƬ�ķֱ��ʡ�
//     2. ���ͼ���в�����32λɫͼ����ʱ��Ҳ�ɸ���mask bitmap�����ɴ�
//     alphaͨ����λͼ
//     3. 32λɫͼ��color bitmap�Դ�alphaֵ����ֱ��ͨ��DrawIconEx���õ�32λλͼ
//
bool GDIIconRenderBitmap::LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e)
{
	if (!m_image.IsNull())
		m_image.Destroy();

	HICON hIcon = (HICON)::LoadImage(nullptr, szPath, IMAGE_ICON, m_nIconWidth, m_nIconHeight, LR_LOADFROMFILE);
	if (nullptr == hIcon)
		return false;

	ICONINFO iconinfo = {0};
	::GetIconInfo(hIcon, &iconinfo);

	// �жϸ�ͼ���Ƿ���32λɫ��ͼ��
	bool IsIcon32 = false;
	BITMAP bm = {0};
	bm.bmBitsPixel = 1; // Ĭ��1λ��λͼ
	if (iconinfo.hbmColor)
	{
		::GetObject(iconinfo.hbmColor, sizeof(BITMAP), &bm);
		if (bm.bmBitsPixel == 32)
		{
			IsIcon32 = true;
		}
	}

	m_image.Create(m_nIconHeight, m_nIconHeight, 32, Image::createAlphaChannel);
	if (IsIcon32)
	{
		HDC hDC = m_image.GetDC();
		::DrawIconEx(hDC, 0,0, hIcon, m_nIconWidth, m_nIconHeight, 0, nullptr, DI_NORMAL);
		m_image.ReleaseDC();
	}
	else
	{
		HDC hDC = m_image.GetDC();
		::DrawIconEx(hDC, 0,0, hIcon, m_nIconWidth, m_nIconHeight, 0, nullptr, DI_NORMAL);
		m_image.ReleaseDC();

		BITMAP bmMask = {0};
		GetObject(iconinfo.hbmMask, sizeof(BITMAP), &bmMask);

		int nSize = bmMask.bmWidthBytes*bmMask.bmHeight;
		BYTE* pMaskBitmapBits = new BYTE[nSize];
		::GetBitmapBits(iconinfo.hbmMask, nSize, (void**)pMaskBitmapBits);

		BYTE* pMaskBits = pMaskBitmapBits;
		BYTE* pDestBits = (BYTE*)m_image.GetBits();

		int nOffset = 0;
		for (int j = 0; j < bm.bmHeight; j++)   // ����Ĭ����mask bitmap��1λ�ĺڰ�ͼƬ��
		{
			for (int i = 0; i < bm.bmWidth; i++)
			{
				BYTE maskvalue = *pMaskBits;
				maskvalue = ((maskvalue << nOffset)&0x80);
				pDestBits[i*4+3] = maskvalue? 0:255;

				nOffset++;
				if (nOffset >= 8)
				{
					nOffset = 0;
					pMaskBits++;
				}

			}

			pDestBits += m_image.GetPitch();
		}
		SAFE_ARRAY_DELETE(pMaskBitmapBits);
	}

	::DestroyIcon(hIcon);

	if (m_image.IsNull())
		return false;
	else
		return true;
}

bool  GDIIconRenderBitmap::LoadFromData(byte* pData, int nSize, RENDER_BITMAP_LOAD_FLAG e)
{
    if (!m_image.IsNull())
        m_image.Destroy();

    HICON hIcon = (HICON)::CreateIconFromResource(pData, nSize, TRUE, 0x00030000);
    if (nullptr == hIcon)
        return false;

    ICONINFO iconinfo = {0};
    ::GetIconInfo(hIcon, &iconinfo);

    // �жϸ�ͼ���Ƿ���32λɫ��ͼ��
    bool IsIcon32 = false;
    BITMAP bm = {0};
    bm.bmBitsPixel = 1; // Ĭ��1λ��λͼ
    if (iconinfo.hbmColor)
    {
        ::GetObject(iconinfo.hbmColor, sizeof(BITMAP), &bm);
        if (bm.bmBitsPixel == 32)
        {
            IsIcon32 = true;
        }
    }

    m_image.Create(m_nIconHeight, m_nIconHeight, 32, Image::createAlphaChannel);
    if (IsIcon32)
    {
        HDC hDC = m_image.GetDC();
        ::DrawIconEx(hDC, 0,0, hIcon, m_nIconWidth, m_nIconHeight, 0, nullptr, DI_NORMAL);
        m_image.ReleaseDC();
    }
    else
    {
        HDC hDC = m_image.GetDC();
        ::DrawIconEx(hDC, 0,0, hIcon, m_nIconWidth, m_nIconHeight, 0, nullptr, DI_NORMAL);
        m_image.ReleaseDC();

        BITMAP bmMask = {0};
        GetObject(iconinfo.hbmMask, sizeof(BITMAP), &bmMask);

        int nSize = bmMask.bmWidthBytes*bmMask.bmHeight;
        BYTE* pMaskBitmapBits = new BYTE[nSize];
        ::GetBitmapBits(iconinfo.hbmMask, nSize, (void**)pMaskBitmapBits);

        BYTE* pMaskBits = pMaskBitmapBits;
        BYTE* pDestBits = (BYTE*)m_image.GetBits();

        int nOffset = 0;
        for (int j = 0; j < bm.bmHeight; j++)   // ����Ĭ����mask bitmap��1λ�ĺڰ�ͼƬ��
        {
            for (int i = 0; i < bm.bmWidth; i++)
            {
                BYTE maskvalue = *pMaskBits;
                maskvalue = ((maskvalue << nOffset)&0x80);
                pDestBits[i*4+3] = maskvalue? 0:255;

                nOffset++;
                if (nOffset >= 8)
                {
                    nOffset = 0;
                    pMaskBits++;
                }

            }

            pDestBits += m_image.GetPitch();
        }
        SAFE_ARRAY_DELETE(pMaskBitmapBits);
    }

    ::DestroyIcon(hIcon);

    if (m_image.IsNull())
        return false;
    else
        return true;
}


SIZE  GDIIconRenderBitmap::GetDrawSize()
{
    SIZE s = { m_nIconWidth, m_nIconHeight };
    return s;
}
void  GDIIconRenderBitmap::SetDrawSize(SIZE* ps)
{
    if (!ps)
        return;

    ps->cx = m_nIconWidth;
    ps->cy = m_nIconHeight;
}



bool UI::GDIImageListRenderBitmap::LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e)
{
	if (e & RENDER_BITMAP_LOAD_DPI_ADAPT)
	{
		Gdiplus::Bitmap bmp(szPath);
		if (bmp.GetLastStatus() != Gdiplus::Ok )
			return false;

		return virtualLoadImageFromBitmap(&bmp, e);
	}

	return __super::LoadFromFile(szPath, e);
}

bool UI::GDIImageListRenderBitmap::virtualLoadImageFromBitmap(
		Gdiplus::Bitmap* pBmp, RENDER_BITMAP_LOAD_FLAG e)
{
	Gdiplus::Bitmap& bmp = *pBmp;

	// ͼƬ�Ѿ��Ŵ�ı���,����ص���@2xͼƬ
	int imageScaled = (e & 0xFF000000) >> 24;
	if (imageScaled == 0)
		imageScaled = 1;

	if (imageScaled == 1 && DEFAULT_SCREEN_DPI == UI::GetDpi())
	{
		m_image.CreateFromGdiplusBitmap(bmp,true);
		return true;
	}

	if (m_nCount == 0)
		return false;

	int nItemWidth = 0;
	int nItemHeight = 0;
	int cx = 1;
	int cy = 1;
	if (m_eLayout == IMAGELIST_LAYOUT_TYPE_H)
	{
		nItemWidth = bmp.GetWidth() / m_nCount;
		nItemHeight = bmp.GetHeight();
		cx = m_nCount;
	}
	else if (m_eLayout == IMAGELIST_LAYOUT_TYPE_V)
	{
		nItemWidth = bmp.GetWidth();
		nItemHeight = bmp.GetHeight() / m_nCount;
		cy = m_nCount;
	}

	int nItemWidthDest = UI::ScaleByDpi(nItemWidth / imageScaled);
	int nItemHeightDest = UI::ScaleByDpi(nItemHeight / imageScaled);

	int nImageWidth = nItemWidthDest * cx;
	int nImageHeight = nItemHeightDest * cy;


	m_image.Create(nImageWidth, nImageHeight, 32, ImageWrap::createAlphaChannel);
	HDC hDC = m_image.GetDC();

	Gdiplus::Graphics g(hDC);
	g.SetInterpolationMode(Gdiplus::InterpolationModeBicubic); 

	Gdiplus::ImageAttributes  attr;
	attr.SetWrapMode(Gdiplus::WrapModeTileFlipXY);

	// �Ƚ�Դͼ����һ���̶���С���ڴ��У��ٰ�����ڴ��е�ͼ���쵽Ŀ��ͼ��
	// ��ֹ����������У�Դͼȡ��ֵ��ʱ��Խ�磬ȡ���Ա��Ǹ�item������
	Gdiplus::Bitmap memBmp(nItemWidth, nItemHeight, bmp.GetPixelFormat());
	Gdiplus::Graphics  gMem(&memBmp);
	gMem.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor); 

	for (int y = 0; y < cy; y++)
	{
		for (int x = 0; x < cx; x++)
		{
			gMem.Clear(Gdiplus::Color::Transparent);

			Gdiplus::RectF rcMemDest;
			rcMemDest.X = (Gdiplus::REAL)0;
			rcMemDest.Y = (Gdiplus::REAL)0;
			rcMemDest.Width = (Gdiplus::REAL)nItemWidth;
			rcMemDest.Height = (Gdiplus::REAL)nItemHeight;
			gMem.DrawImage(&bmp, rcMemDest, 
				(Gdiplus::REAL)nItemWidth * x, 
				(Gdiplus::REAL)nItemHeight * y,
				(Gdiplus::REAL)nItemWidth, 
				(Gdiplus::REAL)nItemHeight,
				Gdiplus::UnitPixel, &attr);


			Gdiplus::RectF rcDest;
			rcDest.X = (Gdiplus::REAL)nItemWidthDest * x;
			rcDest.Y = (Gdiplus::REAL)nItemHeightDest * y;
			rcDest.Width = (Gdiplus::REAL)nItemWidthDest;
			rcDest.Height = (Gdiplus::REAL)nItemHeightDest;

			g.DrawImage(&memBmp, rcDest, 
				(Gdiplus::REAL)0, 
				(Gdiplus::REAL)0,
				(Gdiplus::REAL)nItemWidth, 
				(Gdiplus::REAL)nItemHeight,
				Gdiplus::UnitPixel, &attr);
		}
	}

	m_image.ReleaseDC();

	return true;
}
