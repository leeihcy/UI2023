#pragma once
#include "..\renderresourceimpl.h"
#include "include/interface/graphics.h"
#include "Src\Atl\image.h"
#include "Src\Util\DPI\dpihelper.h"

namespace ui
{

template<class T>
class GDIRenderBitmapImpl : public T
{
public:
    enum 
    {
        GDI_RENDERBITMAP_FLAG_ATTACH = 0x01,
        GDI_RENDERBITMAP_FLAG_ATTACH_DELETE = 0x02,
    };
    GDIRenderBitmapImpl()
    {
        m_nFlag = 0;
    }
	virtual ~GDIRenderBitmapImpl()
	{
        if ((m_nFlag & GDI_RENDERBITMAP_FLAG_ATTACH) &&
            (m_nFlag & GDI_RENDERBITMAP_FLAG_ATTACH_DELETE))
        {
            m_image.Detach();
        }
//		UI_LOG_DEBUG("GDIRenderBitmap Delete. ptr=0x%08X", this);
	}

	virtual eGraphicsLibraryType GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; }

	Image*  GetBitmap() { return &m_image; }

public:
	virtual bool  LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e)
	{
		if (!m_image.IsNull())
		{
			m_image.Destroy();
		}

		if (e & RENDER_BITMAP_LOAD_CREATE_ALPHA_CHANNEL)
		{
			m_image.ForceUseAlpha();
		}

        const TCHAR* szExt = szPath + (strlen(szPath)-4);
		if (0 == _tcsicmp(szExt, _T(".ico")))
		{
			const int ICON_SIZE = 16;
			HICON hIcon = (HICON)::LoadImage ( nullptr, szPath, IMAGE_ICON,ICON_SIZE,ICON_SIZE, LR_LOADFROMFILE );
			HDC hMemDC = Image::GetCacheDC();

			m_image.Create( ICON_SIZE, ICON_SIZE, 32, Image::createAlphaChannel );
			HBITMAP hOldBmp = (HBITMAP)::SelectObject( hMemDC, (HBITMAP)m_image );

			::DrawIconEx(hMemDC, 0,0, hIcon, ICON_SIZE, ICON_SIZE, 0, nullptr, DI_NORMAL );
			::SelectObject(hMemDC, hOldBmp);
			Image::ReleaseCacheDC(hMemDC);
			::DestroyIcon(hIcon);
		}
		else
		{
            // 利用Gdiplus来抗锯齿缩放
            if (e & RENDER_BITMAP_LOAD_DPI_ADAPT)
            {
				// 图片已经放大的倍数,如加载的是@2x图片
				int imageScaled = (e & 0xFF000000) >> 24;
				if (imageScaled == 0)
					imageScaled = 1;

                Gdiplus::Bitmap bmp(szPath);
                if (bmp.GetLastStatus() == Gdiplus::Ok )
                {
					if (imageScaled == 1 && DEFAULT_SCREEN_DPI == UI::GetDpi())
					{
						m_image.CreateFromGdiplusBitmap(bmp,true);
					}
					else
					{
						long w = bmp.GetWidth();
						long h = bmp.GetHeight();
						w = UI::ScaleByDpi(w / imageScaled);
						h = UI::ScaleByDpi(h / imageScaled);

						m_image.CreateSpecialSizeFromGdiplusBitmap(&bmp, w, h);
					}
				}
            }
            else
            {
			    m_image.Load(szPath);
            }
		}

		return m_image.IsNull() ? false: true;
	}
    virtual bool  LoadFromData(byte* pData, int nSize, RENDER_BITMAP_LOAD_FLAG e)
    {
        if (!m_image.IsNull())
        {
            m_image.Destroy();
        }

        if (e & RENDER_BITMAP_LOAD_CREATE_ALPHA_CHANNEL)
        {
            m_image.ForceUseAlpha();
        }

        if (e & RENDER_BITMAP_LOAD_DPI_ADAPT)
        {
            LPSTREAM pStream = nullptr;
            HGLOBAL  hNew    = ::GlobalAlloc(GHND, nSize);
            LPBYTE  lpByte    = (LPBYTE)::GlobalLock(hNew);
            ::memcpy(lpByte, pData, nSize);
            ::GlobalUnlock(hNew);

            // 从指定内存创建流对象
            HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
            if (ht != S_OK)
            {
                GlobalFree(hNew);
                return false;
            }

            Gdiplus::Bitmap* pBmp = Gdiplus::Bitmap::FromStream(pStream);
            if (pBmp)
            {
				this->virtualLoadImageFromBitmap(pBmp, e);
				delete pBmp;
            }
            
            pStream->Release();
        }
        else
        {
            m_image.LoadFromData(pData, nSize);
        }
        return m_image.IsNull() ? false: true;
    }
	
	// 统一入口，需要gdiplus介入的地方走这个，也便于dpi处理。
	virtual bool  virtualLoadImageFromBitmap(
		Gdiplus::Bitmap* pBmp, RENDER_BITMAP_LOAD_FLAG e)
	{
		long w = pBmp->GetWidth();
		long h = pBmp->GetHeight();
		w = UI::ScaleByDpi(w);
		h = UI::ScaleByDpi(h);

		m_image.CreateSpecialSizeFromGdiplusBitmap(pBmp, w, h);
		return true;
	}

	virtual bool  Create(int nWidth, int nHeight)
	{
		UIASSERT(nHeight > 0);   // 使用反向的(bottom-up DIB)
		if (!m_image.IsNull())
		{
			m_image.Destroy();
		}
		m_image.Create(nWidth, nHeight, 32, Image::createAlphaChannel);

		if (m_image.IsNull())
			return false;
		else
			return true;
	}

	virtual int   GetWidth()
	{
		return m_image.GetWidth();
	}
	virtual int   GetHeight()
	{
		return m_image.GetHeight();
	}

	virtual int   GetBPP()
	{
		return m_image.GetBPP();
	}

	virtual Color GetAverageColor() 
	{
		return m_image.GetAverageColor();
	}

	virtual BYTE* LockBits()
	{
		return (BYTE*)m_image.GetBits();
	}
	virtual void  UnlockBits()
	{
		// Nothing.
	}

	virtual bool  SaveBits(ImageData* pImageData)
	{
		if (nullptr == pImageData)
			return false;

		int nSize = 0;
		SAFE_ARRAY_DELETE(pImageData->m_ptr);
		m_image.SaveBits(pImageData, &nSize);
		pImageData->m_ptr = new BYTE[nSize];
		pImageData->m_bNeedDeletePtr = true;

		return m_image.SaveBits(pImageData, &nSize);
	}
	virtual bool  ChangeHSL( const ImageData* pOriginImageData, short h, short s, short l, int nFlag )
	{
		bool bRet = m_image.ChangeHSL(pOriginImageData, h, s, l, nFlag );
		return bRet;
	}

	virtual HBITMAP CopyRect(RECT *prc)
	{
		return m_image.CopyImageByRect(prc);
	}


// 	virtual void  Attach(HBITMAP hBitmap, bool bDelete) override
// 	{
// 		Destroy();
// 		if (!hBitmap)
// 			return;
// 
// 		if (bDelete)
// 		{
// 			m_image.Attach(hBitmap);
// 		}
// 		else
// 		{
// 			// 拷贝一张
// 			Image temp;
// 			temp.Attach(hBitmap);
// 
// 			m_image.Create(temp.GetWidth(), temp.GetHeight(), temp.GetBPP(), 
// 				temp.GetBPP()==32 ? Image::createAlphaChannel:0);
// 
// 			HDC hDC = m_image.GetDC();
// 			temp.BitBlt(hDC, 0, 0);
// 			m_image.ReleaseDC();
// 			temp.Detach();
// 		}
// 	}
    void  Attach(HBITMAP hBitmap, bool bDelete)
    {
        m_image.Attach(hBitmap);
        m_nFlag |= GDI_RENDERBITMAP_FLAG_ATTACH;
        if (bDelete)
        {
            m_nFlag |= GDI_RENDERBITMAP_FLAG_ATTACH_DELETE;
        }
    }
    virtual HBITMAP  Detach() 
    {
        m_nFlag &= ~(GDI_RENDERBITMAP_FLAG_ATTACH_DELETE|GDI_RENDERBITMAP_FLAG_ATTACH);
        return m_image.Detach(); 
    }
    virtual void  Destroy()
    {
        m_image.Destroy();
    }
protected:
	Image   m_image;
    int  m_nFlag;
};

class GDIRenderBitmap : public GDIRenderBitmapImpl<IRenderResourceImpl<IRenderBitmap> >
{
public:
	static  void CreateInstance(IRenderBitmap** ppOut);
    virtual eImageItemType  GetImageType() { return IMAGE_ITEM_TYPE_IMAGE; }
};
class GDIIconRenderBitmap : public GDIRenderBitmapImpl<IRenderResourceImpl<IImageIconRenderBitmap > >// : public GDIRenderBitmap
{
public:
	GDIIconRenderBitmap();

	static  void  CreateInstance(IRenderBitmap** pOut);
	virtual bool  LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e);
    virtual bool  LoadFromData(byte* pData, int nSize, RENDER_BITMAP_LOAD_FLAG e);
    
    virtual eImageItemType  GetImageType() { return IMAGE_ITEM_TYPE_ICON; }

    virtual SIZE  GetDrawSize();
    virtual void  SetDrawSize(SIZE* ps);
private:
	int    m_nIconWidth;
	int    m_nIconHeight;
};

class GDIImageListRenderBitmap : public GDIRenderBitmapImpl<IRenderResourceImpl<IImageListRenderBitmap> >
{
protected:
	GDIImageListRenderBitmap();

public:
	static  void CreateInstance(IRenderBitmap** pOut);

	virtual int  GetItemWidth();
	virtual int  GetItemHeight();
	virtual IMAGELIST_LAYOUT_TYPE GetLayoutType();
	virtual bool GetIndexPos(int nIndex, POINT* pPoint);
	virtual int  GetItemCount() { return m_nCount; }
    virtual void  SetItemCount(int);
    virtual void  SetLayoutType(IMAGELIST_LAYOUT_TYPE);

	virtual bool  LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e);
	virtual bool  virtualLoadImageFromBitmap(
		Gdiplus::Bitmap* pBmp, RENDER_BITMAP_LOAD_FLAG e) override;

    virtual eImageItemType  GetImageType() { return IMAGE_ITEM_TYPE_IMAGE_LIST; }
private:
	IMAGELIST_LAYOUT_TYPE   m_eLayout;
	int     m_nCount;
};
}

