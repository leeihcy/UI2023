#pragma once
#include "Src\atl\image.h"

namespace UI
{
template<class T>
class GdiplusRenderBitmapImpl : public T
{
protected:
	GdiplusRenderBitmapImpl()
	{
		m_pBitmap = NULL;
		m_pBitmapData = NULL;
	}
public:
	virtual ~GdiplusRenderBitmapImpl()
	{
		SAFE_DELETE(m_pBitmapData);
		SAFE_DELETE(m_pBitmap);

		UI_LOG_DEBUG(_T("GdiplusRenderBitmap Delete. ptr=0x%08X"), this);
	}

	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS; }
	Gdiplus::Bitmap*  GetBitmap() { return m_pBitmap; }

public:

	//
	//	Remark:
	//		使用Gdiplus加载出来的ICON会丢失透明阴影，在这里对ICON进行了特殊处理
	//		因此在这里先使用GDI DrawIcon获取一次完整的数据
	//		TODO: 这里只默认支持16*16大小的，ico中其它大小的图标暂时没有增加接口分别加载
	//
	virtual bool  LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e)
	{
		SAFE_DELETE(m_pBitmap);
        SAFE_DELETE(m_pBitmapData);
		m_pBitmap = Gdiplus::Bitmap::FromFile(szPath);

		if (NULL == m_pBitmap || m_pBitmap->GetLastStatus() != Gdiplus::Ok)
		{
			SAFE_DELETE(m_pBitmap);
			return false;
		}
		else
			return true;
	}
    virtual bool  LoadFromData(byte* pData, int nSize, RENDER_BITMAP_LOAD_FLAG e)
    {
        SAFE_DELETE(m_pBitmap);
        SAFE_DELETE(m_pBitmapData);

        HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, nSize);
        byte* pGlobalData = (byte*)GlobalLock(hGlobal);
        memcpy(pGlobalData, pData, nSize);
        GlobalUnlock(hGlobal);
        IStream* pStream = NULL;
        CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);

        m_pBitmap = Gdiplus::Bitmap::FromStream(pStream);
        SAFE_RELEASE(pStream);

        if (NULL == m_pBitmap || m_pBitmap->GetLastStatus() != Gdiplus::Ok)
        {
            SAFE_DELETE(m_pBitmap);
            return false;
        }
        else
            return true;
    }

	virtual bool  Create( int nWidth, int nHeight )
	{
		SAFE_DELETE(m_pBitmap);
		SAFE_DELETE(m_pBitmapData);

		// The default value is PixelFormat32bppARGB
		m_pBitmap = new Gdiplus::Bitmap(nWidth, nHeight);

		if (NULL == m_pBitmap)
			return false;
		else
			return true;
	}

    virtual void  Destroy()
    {
        SAFE_DELETE(m_pBitmap);
        SAFE_DELETE(m_pBitmapData);
    }
	virtual int   GetWidth()
	{
		if (NULL == m_pBitmap)
			return 0;

		return m_pBitmap->GetWidth();
	}
	virtual int   GetHeight()
	{
		if (NULL == m_pBitmap)
			return 0;

		return m_pBitmap->GetHeight();
	}

	//
	// WARNING: 不支持多线程使用,不支持嵌套使用
	//
	virtual BYTE* LockBits()
	{
		if( NULL == m_pBitmap )
		{
			return NULL;
		}

		if( NULL != m_pBitmapData )
		{
			return NULL;
		}

		m_pBitmapData = new Gdiplus::BitmapData;
		Gdiplus::Rect rect(0,0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());

		m_pBitmap->LockBits(
			&rect,
			Gdiplus::ImageLockModeWrite|Gdiplus::ImageLockModeRead,
			PixelFormat32bppARGB,
			m_pBitmapData);

		return (BYTE*)m_pBitmapData->Scan0;
	}
	virtual void  UnlockBits()
	{
		if( NULL != m_pBitmapData )
		{
			m_pBitmap->UnlockBits(m_pBitmapData);
			delete m_pBitmapData;
			m_pBitmapData = NULL;
		}
	}


	// 将图片数据清0。用于带透明的图片重绘前先刷新上一次的数据，避免alpha叠加
	// 废弃，有直接的Graphic->Clear方法
	// 	void    Clear()
	// 	{
	// 		Gdiplus::BitmapData data;;
	// 		Gdiplus::Rect rect(0,0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());
	// 		m_pBitmap->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &data);
	// 		
	// 		byte* pTemp = (byte*)data.Scan0;
	// 		int   bytesperline = abs(data.Stride);
	// 		for (int row = 0; row < (int)data.Height; row ++ )
	// 		{
	// 			memset(pTemp, 0, bytesperline);
	// 			pTemp += data.Stride;
	// 		}
	// 		
	// 		m_pBitmap->UnlockBits(&data);
	// 	}

	virtual bool  SaveBits( ImageData* pImageData )
	{
		if( NULL == pImageData || NULL == m_pBitmap )
			return false;

		Gdiplus::BitmapData* pBitmapData = new Gdiplus::BitmapData;
		Gdiplus::Rect rect(0,0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());

		m_pBitmap->LockBits(
			&rect,
			Gdiplus::ImageLockModeWrite|Gdiplus::ImageLockModeRead,
			PixelFormat32bppARGB,
			pBitmapData);

		//////////////////////////////////////////////////////////////////////////

		BYTE* pThisBits = (BYTE*)pBitmapData->Scan0;
		int   bytesperline = abs(pBitmapData->Stride);

		pImageData->m_nWidth = pBitmapData->Width;
		pImageData->m_nHeight = pBitmapData->Height;
		pImageData->m_nStride = pBitmapData->Stride;
		pImageData->m_nbpp = 32;
		pImageData->m_bNeedDeletePtr = true;

		// 创建内存
		int nSize = bytesperline*pBitmapData->Height;
		pImageData->m_ptr = new BYTE[nSize];
		pImageData->m_pScan0 = pImageData->m_ptr;

		if( pBitmapData->Stride < 0 )
			pImageData->m_pScan0 += ((pBitmapData->Height-1)*bytesperline);

		// 内存拷贝
		BYTE* pTemp = pImageData->m_pScan0;
		for (int row = 0; row < (int)pBitmapData->Height; row ++ )
		{
			memcpy(pTemp, pThisBits, bytesperline);
			pThisBits += pBitmapData->Stride;
			pTemp += pBitmapData->Stride;
		}


		//////////////////////////////////////////////////////////////////////////

		m_pBitmap->UnlockBits(pBitmapData);
		SAFE_DELETE(pBitmapData);

		return true;
	}
	virtual bool  ChangeHSL( const ImageData* pOriginImageData, short h, short s, short l, int nFlag )
	{
		if( NULL == pOriginImageData || NULL == m_pBitmap )
			return false;

		Gdiplus::BitmapData* pBitmapData = new Gdiplus::BitmapData;
		Gdiplus::Rect rect(0,0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());

		int nPixelFormat = PixelFormat32bppARGB;
		if( 24 == pOriginImageData->m_nbpp )
		{
			nPixelFormat = PixelFormat24bppRGB;
		}
		else if( 32 == pOriginImageData->m_nbpp )
		{
			nPixelFormat = PixelFormat32bppARGB;
		}
		else
		{
			UI_LOG_WARN( _T("%s image pixel format not support :%d"), _T(__FUNCTION__), pOriginImageData->m_nbpp );
			return false;
		}
		m_pBitmap->LockBits(
			&rect,
			Gdiplus::ImageLockModeWrite|Gdiplus::ImageLockModeRead,
			nPixelFormat,
			pBitmapData);

		//////////////////////////////////////////////////////////////////////////

		BYTE* pTemp = pOriginImageData->m_pScan0;
		if( NULL == pTemp )
			return false;

		bool bChangeH = nFlag & CHANGE_SKIN_HLS_FLAG_H ? true:false;
		bool bChangeL = nFlag & CHANGE_SKIN_HLS_FLAG_L ? true:false;
		bool bChangeS = nFlag & CHANGE_SKIN_HLS_FLAG_S ? true:false;
		bool bSetHueMode = nFlag & CHANGE_SKIN_HLS_FALG_REPLACE_MODE ? false:true;

		if (l == 0)
			bChangeL = false;
		if (s == 0)
			bChangeS = false;

		if(false == bChangeH && false == bChangeL && false == bChangeS)
			return false;

		BYTE* pNewImageBits = (BYTE*)pBitmapData->Scan0;
		int   bytesperline  = abs(pBitmapData->Stride);
		int   bytesperpx    = pOriginImageData->m_nbpp/8;

		float dL = 0, ds = 0;
		if (bChangeL)
			dL = (float)(l/100.0);   // 避免在循环中重复计算该值
		if (bChangeS)
			ds = (float)(s/100.0);

		for (int row = 0; row < (int)pBitmapData->Height; row ++ )
		{
			for( int i = 0; i < bytesperline; i += bytesperpx )
			{
				BYTE B = pTemp[i];
				BYTE G = pTemp[i+1];
				BYTE R = pTemp[i+2];

				if (nPixelFormat == PixelFormat32bppARGB)
					pNewImageBits[i+3] = pTemp[i+3];

				if (bChangeL)
					ChangeColorLuminance(R,G,B,l,dL);

				if (bChangeH && bChangeS)
				{
					ChangeColorHueAndSaturation(R,G,B,h,bSetHueMode,s,ds);
				}
				else
				{
					if (bChangeH)
						ChangeColorHue(R,G,B,h,bSetHueMode);
					if (bChangeS)
						ChangeColorSaturation(R,G,B,s,ds);
				}

				pNewImageBits[i]   = B;
				pNewImageBits[i+1] = G;
				pNewImageBits[i+2] = R;
			}

			pNewImageBits += pBitmapData->Stride;
			pTemp += pOriginImageData->m_nStride;
		}

		//////////////////////////////////////////////////////////////////////////

		m_pBitmap->UnlockBits(pBitmapData);
		SAFE_DELETE(pBitmapData);

		return true;
	}

	HBITMAP CopyRect(RECT* prc)
	{
		if (NULL == prc)
			return NULL;

		int nWidth = prc->right - prc->left;
		int nHight = prc->bottom - prc->top;

		Gdiplus::BitmapData data;
		Gdiplus::Rect rect(prc->left ,prc->top, nWidth, nHight);
		m_pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data);

		HBITMAP hBitmap = NULL;
		Gdiplus::Color c(0,0,0,0);
		Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(nWidth, nHight, data.Stride, PixelFormat32bppARGB, (BYTE*)data.Scan0);
		pBitmap->GetHBITMAP(c, &hBitmap);
		delete pBitmap;
		m_pBitmap->UnlockBits(&data);

		return hBitmap;
	}

	virtual int GetBPP()
	{
		if (NULL == m_pBitmap)
			return 0;

		Gdiplus::PixelFormat pf = m_pBitmap->GetPixelFormat();
		return Gdiplus::GetPixelFormatSize(pf);
	}

protected:
	Gdiplus::Bitmap*     m_pBitmap;
	Gdiplus::BitmapData* m_pBitmapData;        // LockBits 过程中使用到的临时变量
};


class GdiplusRenderBitmap : public GdiplusRenderBitmapImpl<IRenderResourceImpl<IRenderBitmap> >
{
protected:
	GdiplusRenderBitmap(){}
public:
	static  void CreateInstance( IRenderBitmap** pOutRef );
    virtual IMAGE_ITEM_TYPE  GetImageType() { return IMAGE_ITEM_TYPE_IMAGE; }
};
class GdiplusIconRenderBitmap : public GdiplusRenderBitmapImpl<IRenderResourceImpl<IImageIconRenderBitmap > > // : public GdiplusRenderBitmap
{
protected:
	GdiplusIconRenderBitmap();
public:
	~GdiplusIconRenderBitmap();
	static  void  CreateInstance( IRenderBitmap** pOutRef );
	virtual bool  LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e);
    virtual IMAGE_ITEM_TYPE  GetImageType() { return IMAGE_ITEM_TYPE_ICON; }

    virtual SIZE  GetDrawSize();
    virtual void  SetDrawSize(SIZE* ps);

protected:
	int    m_nIconWidth;
	int    m_nIconHeight;

	HBITMAP  m_hBitmapToFixIcon;   // 解决Gdiplus加载ICON时丢失alpha的问题
};

class GdiplusImageListRenderBitmap : public GdiplusRenderBitmapImpl<IRenderResourceImpl<IImageListRenderBitmap> >
{
protected:
	GdiplusImageListRenderBitmap();
    ~GdiplusImageListRenderBitmap();

public:
	static  void CreateInstance(IRenderBitmap** pOutRef );

	virtual int  GetItemWidth();
	virtual int  GetItemHeight();
	virtual IMAGELIST_LAYOUT_TYPE GetLayoutType();
	virtual bool GetIndexPos(int nIndex, POINT* pPoint);
	virtual int  GetItemCount();
    virtual IMAGE_ITEM_TYPE  GetImageType() { return IMAGE_ITEM_TYPE_IMAGE_LIST; }
    virtual void  SetItemCount(int);
    virtual void  SetLayoutType(IMAGELIST_LAYOUT_TYPE);

private:
	IMAGELIST_LAYOUT_TYPE   m_eLayout;
	int     m_nCount;
};
}