// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#pragma once

#pragma warning(disable:4312)
#pragma warning(disable:4311)
#pragma warning(disable:4312)

#pragma warning( push, 3 )
// #pragma push_macro("new")
// #undef new
// #include <gdiplus.h>
// #pragma pop_macro("new")
// #include "3rd\gdiplus\gdiplusfix.h"
#include <gdiplus.h>
#pragma warning( pop )

#include <shlwapi.h>
#include <malloc.h>


// 在版本小于0x0500时，显示PNG图片不会透明
#ifndef WINVER				 
	#define WINVER 0x0500	 
#elif WINVER < 0x0500
		#undef  WINVER
		#define WINVER 0x0500
#endif

#define CLR_USE_COLOR_DIRECT    0xFFFFFFFE


#include <assert.h>
#include <string>
#include <vector>
using namespace std;


#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")
#if WINVER >= 0x0500
#pragma comment(lib, "msimg32.lib")
#endif  // WINVER >= 0x0500


namespace ui
{
	const int CIMAGE_DC_CACHE_SIZE = 4;

	class Image;

	// // 
	// //	图片内存数据拷贝，用于皮肤改变色调时保存其原始数据，并兼容gdi/gdi+图片
	// //
	// class ImageData
	// {
	// public:
	// 	ImageData()
	// 	{
	// 		m_bNeedDeletePtr = false;
	// 		m_ptr = m_pScan0 = 0;
	// 		m_nbpp = m_nStride = m_nWidth = m_nHeight = 0;
	// 	}
	// 	~ImageData()
	// 	{
	// 		if (m_ptr && m_bNeedDeletePtr)
	// 			delete[] m_ptr;
	// 		m_ptr = m_pScan0 = 0;
	// 		m_nbpp = m_nStride = m_nWidth = m_nHeight = 0;
	// 	}

	// 	BYTE*  m_ptr;         // 数据首地址，用于new/delete
	// 	BYTE*  m_pScan0;      // 图片第一行的数据地址
	// 	int    m_nbpp;        // bit per pixel
	// 	int    m_nStride;     // Offset, in bytes, between consecutive scan lines of the bitmap. If the stride is positive, the bitmap is top-down. If the stride is negative, the bitmap is bottom-up.
	// 	int    m_nWidth;      // 图片宽度
	// 	int    m_nHeight;     // 图片高度
	// 	bool  m_bNeedDeletePtr;  // 是否需要释放m_ptr
	// };

	class CImageDC
	{
	public:
		CImageDC( const Image& image ) throw( ... );
		~CImageDC() throw();

		operator HDC() const throw();

	private:
		const Image& m_image;
		HDC m_hDC;
	};

	class CDCCache
	{
	public:
		CDCCache() throw();
		~CDCCache() throw();

		HDC GetDC() throw();
		void ReleaseDC( HDC ) throw();

	private:
		HDC m_ahDCs[CIMAGE_DC_CACHE_SIZE];
	};


	class Image
	{
	private:
		class CInitGDIPlus
		{
		public:
			CInitGDIPlus();
			~CInitGDIPlus() throw();

			bool Init() throw();
			void ReleaseGDIPlus() throw();
			void IncreaseCImageCount() throw();
			void DecreaseCImageCount() throw();

		private:
			ULONG_PTR m_dwToken;
			CRITICAL_SECTION m_sect;
			LONG m_nCImageObjects;
		};

	public:
		static const DWORD createAlphaChannel = 0x01;

		static const DWORD excludeGIF = 0x01;
		static const DWORD excludeBMP = 0x02;
		static const DWORD excludeEMF = 0x04;
		static const DWORD excludeWMF = 0x08;
		static const DWORD excludeJPEG = 0x10;
		static const DWORD excludePNG = 0x20;
		static const DWORD excludeTIFF = 0x40;
		static const DWORD excludeIcon = 0x80;
		static const DWORD excludeOther = 0x80000000;
		static const DWORD excludeDefaultLoad = 0;
		static const DWORD excludeDefaultSave = excludeIcon|excludeEMF|excludeWMF;
		static const DWORD excludeValid = 0x800000ff;

		enum DIBOrientation
		{
			DIBOR_DEFAULT,
			DIBOR_TOPDOWN,
			DIBOR_BOTTOMUP
		};

        enum ImageAlphaEdge
        {
            ImageAlphaEdge_Left = 1,
            ImageAlphaEdge_Top  = 2,
            ImageAlphaEdge_Right = 4,
            ImageAlphaEdge_Bottom = 8,
        };
	public:
		Image() throw();
		virtual ~Image() throw();

		operator HBITMAP() const throw();
		HBITMAP GetHBITMAP() { return m_hBitmap; }
#if WINVER >= 0x0500
		BOOL  AlphaBlend( HDC hDestDC, int xDest, int yDest, BYTE bSrcAlpha = 0xff, BYTE bBlendOp = AC_SRC_OVER ) const throw();
		BOOL  AlphaBlend( HDC hDestDC, const POINT& pointDest, BYTE bSrcAlpha = 0xff, BYTE bBlendOp = AC_SRC_OVER ) const throw();
		BOOL  AlphaBlend( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, BYTE bSrcAlpha = 0xff, BYTE bBlendOp = AC_SRC_OVER ) const throw();
		BOOL  AlphaBlend( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, BYTE bSrcAlpha = 0xff, BYTE bBlendOp = AC_SRC_OVER ) const throw();
#endif  // WINVER >= 0x0500
		void  Attach( HBITMAP hBitmap, DIBOrientation eOrientation = DIBOR_DEFAULT ) throw();
		BOOL  BitBlt( HDC hDestDC, int xDest, int yDest, DWORD dwROP = SRCCOPY ) const throw();
		BOOL  BitBlt( HDC hDestDC, const POINT& pointDest, DWORD dwROP = SRCCOPY ) const throw();
		BOOL  BitBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, DWORD dwROP = SRCCOPY ) const throw();
		BOOL  BitBlt( HDC hDestDC, const RECT& rectDest, const POINT& pointSrc, DWORD dwROP = SRCCOPY ) const throw();
		BOOL  Create( int nWidth, int nHeight, int nBPP, DWORD dwFlags = 0 ) throw();
		BOOL  CreateEx( int nWidth, int nHeight, int nBPP, DWORD eCompression, const DWORD* pdwBitmasks = nullptr, DWORD dwFlags = 0 ) throw();
		void  Destroy() throw();
		HBITMAP Detach() throw();
		BOOL  Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, byte bAlpha=0xFF ) const throw();
		/* bo.li ADD 20101202 增加9宫绘制 */
		BOOL  Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, C9Region* pImage9Region, bool bDrawCenter=true, byte bAlpha=0xFF ) const throw();
		BOOL  Draw( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, byte bAlpha=0xFF ) const throw();
		BOOL  Draw( HDC hDestDC, int xDest, int yDest, byte bAlpha=0xFF ) const throw();
		BOOL  Draw( HDC hDestDC, const POINT& pointDest, byte bAlpha=0xFF ) const throw();
		BOOL  Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, byte bAlpha=0xFF ) const throw();
		BOOL  Draw( HDC hDestDC, const RECT& rectDest, byte bAlpha=0xFF ) const throw();
		const void* GetBits() const throw();
		void* GetBits() throw();
		int   GetBPP() const throw();
		void  GetColorTable( unsigned int iFirstColor, unsigned int nColors, RGBQUAD* prgbColors ) const throw();
		HDC   GetDC() const throw();
		static HRESULT GetExporterFilterString( String& strExporters, vector< GUID >& aguidFileTypes, const wchar_t* pszAllFilesDescription = nullptr, DWORD dwExclude = excludeDefaultSave, TCHAR chSeparator = _T( '|' ) );
		static HRESULT GetImporterFilterString( String& strImporters, vector< GUID >& aguidFileTypes, const wchar_t* pszAllFilesDescription = nullptr, DWORD dwExclude = excludeDefaultLoad, TCHAR chSeparator = _T( '|' ) );
		static HDC   GetCacheDC(){ return s_cache.GetDC(); }
		static void  ReleaseCacheDC(HDC hDC) { s_cache.ReleaseDC(hDC); }

		int  GetHeight() const throw();
		int  GetMaxColorTableEntries() const throw();
		int  GetPitch() const throw();
		const void* GetPixelAddress( int x, int y ) const throw();
		void* GetPixelAddress( int x, int y ) throw();
		COLORREF GetPixel( int x, int y ) const throw();
		LONG GetTransparentColor() const throw();
		int  GetWidth() const throw();
		bool IsDIBSection() const throw();
		bool IsIndexed() const throw();
		bool IsNull() const throw();
		HRESULT Load( const wchar_t* pszFileName ) throw();
		HRESULT Load( IStream* pStream ) throw();
		void LoadFromResource( HINSTANCE hInstance, const wchar_t* pszResourceName ) throw();
		void LoadFromResource( HINSTANCE hInstance, unsigned int nIDResource ) throw();
		void LoadFromResource( HINSTANCE hInstance, unsigned int nIDResource, TCHAR* szResourceType );
		bool LoadFromData( void* pImageData, int nSize );
		HRESULT CreateFromGdiplusBitmap( Gdiplus::Bitmap& bmSrc, bool bForceAlpha ) throw();
		void  CreateSpecialSizeFromGdiplusBitmap(Gdiplus::Bitmap* pBmp, long newWidth, long newHeight);

		BOOL MaskBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, HBITMAP hbmMask, int xMask, int yMask, DWORD dwROP = SRCCOPY ) const throw();
		BOOL MaskBlt( HDC hDestDC, const RECT& rectDest, const POINT& pointSrc, HBITMAP hbmMask, const POINT& pointMask, DWORD dwROP = SRCCOPY ) const throw();
		BOOL MaskBlt( HDC hDestDC, int xDest, int yDest, HBITMAP hbmMask, DWORD dwROP = SRCCOPY ) const throw();
		BOOL MaskBlt( HDC hDestDC, const POINT& pointDest, HBITMAP hbmMask, DWORD dwROP = SRCCOPY ) const throw();
		BOOL PlgBlt( HDC hDestDC, const POINT* pPoints, HBITMAP hbmMask = nullptr ) const throw();
		BOOL PlgBlt( HDC hDestDC, const POINT* pPoints, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, HBITMAP hbmMask = nullptr, int xMask = 0, int yMask = 0 ) const throw();
		/* ↓ 修改：将CPoint->Point,并去掉默认参数CPoint(0,0) */
		BOOL PlgBlt( HDC hDestDC, const POINT* pPoints, const RECT& rectSrc, const POINT& pointMask, HBITMAP hbmMask = nullptr ) const throw();
		void ReleaseDC() const throw();
		HRESULT Save( IStream* pStream, REFGUID guidFileType ) const throw();
		HRESULT Save( const wchar_t* pszFileName, REFGUID guidFileType = GUID_NULL ) const throw();
		void SetColorTable( unsigned int iFirstColor, unsigned int nColors, const RGBQUAD* prgbColors ) throw();
        void SetPixelBit32(uint x, uint y, long lValue);
		void SetPixel( int x, int y, COLORREF color ) throw();
		void SetPixelIndexed( int x, int y, int iIndex ) throw();
		void SetPixelRGB( int x, int y, BYTE r, BYTE g, BYTE b ) throw();
		LONG SetTransparentColor( LONG iTransparentColor ) throw();
		COLORREF SetTransparentColor( COLORREF colorTransparent ) throw();
		BOOL StretchBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, DWORD dwROP = SRCCOPY ) const throw();
		BOOL StretchBlt( HDC hDestDC, const RECT& rectDest, DWORD dwROP = SRCCOPY ) const throw();
		BOOL StretchBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwROP = SRCCOPY ) const throw();
		BOOL StretchBlt( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, DWORD dwROP = SRCCOPY ) const throw();
#if WINVER >= 0x0500
		BOOL TransparentBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, unsigned int crTransparent = CLR_INVALID ) const throw();
		BOOL TransparentBlt( HDC hDestDC, const RECT& rectDest, unsigned int crTransparent = CLR_INVALID ) const throw();
		BOOL TransparentBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, unsigned int crTransparent = CLR_INVALID ) const throw();
		BOOL TransparentBlt( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, unsigned int crTransparent = CLR_INVALID ) const throw();
#endif  // WINVER >= 0x0500

		static BOOL IsTransparencySupported() throw();
		void   ForceUseAlpha(){m_bHasAlphaChannel = true ;}   // libo add 20110710 在调用Create时，我需要创建一个带通道的位图，但没有提供给我接口，因此增加一个
		bool   HasAlphaChannel() const { return m_bHasAlphaChannel; }
		HBITMAP   CreateDDB();                     // libo add 20120318 TransparentBlt只能适应于DDB，因此提供一个接口将DIB转换成DDB
		bool   CopyGrayImageFrom( const Image* pImage );         // libo add 20120321 去色
		bool   DrawGray(HDC hDC, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc);  // libo add 20120927
		bool   ChangeHSL( const ImageData* pOriginImageData, short h, short s, short l, int nFlag );
		bool   SaveBits( ImageData* pImageData, int* pNeedSize );
		void   RestoreBits( ImageData* pImageData );
		bool   ImageList_Draw(HDC hDestDC, int x, int y, int col, int row, int cx, int cy );  // libo add 20120401 增加图像列表绘制方法
		HBITMAP  CopyImageByRect(RECT* prc);    // libo add 20121019 增加拷贝图片的一部分的方法
		DWORD  GetAverageColor();      // libo add 20121128 增加获取图片平均色的方法
        DWORD  GetAverageColor(LPCRECT prc);
		bool   ModifyAlpha(ImageData* pOriginImageData, byte nAlphaPercent);
		bool   SetAlpha(byte nAlpha);
		bool   FixGDIAlphaChannel(int nAlphaIsZero=1);
        void   AlphaEdge(int nEdge, byte nSpeed=5);
        void   Clear();
		
		// void  PreMulti();
		void  UnPreMulti();

	private:
		HBITMAP m_hBitmap;
		void* m_pBits;
		int   m_nWidth;
		int   m_nHeight;
		int   m_nPitch;
		int   m_nBPP;               // bit per pixel
		bool  m_bIsDIBSection;
		bool  m_bHasAlphaChannel;
		LONG  m_iTransparentColor;    // -1 表示未采用, -2 表示直接使用m_colorTransparent
		COLORREF m_colorTransparent; // 当m_iTransparentColor为-2时有效


		static CInitGDIPlus s_initGDIPlus;

	public:  // modified by libo 2012-2-17 为了使其它外部类也能引用该对象来加减引用计数
		inline static void ReleaseGDIPlus()      { DecreaseCImageCount(); s_initGDIPlus.ReleaseGDIPlus(); }
		inline static bool InitGDIPlus() throw() { IncreaseCImageCount(); return  s_initGDIPlus.Init(); }
		inline static void IncreaseCImageCount() { s_initGDIPlus.IncreaseCImageCount(); }
		inline static void DecreaseCImageCount() { s_initGDIPlus.DecreaseCImageCount(); }

		// Implementation
	private:
		static CLSID FindCodecForExtension( const wchar_t* pszExtension, const Gdiplus::ImageCodecInfo* pCodecs, unsigned int nCodecs );
		static CLSID FindCodecForFileType( REFGUID guidFileType, const Gdiplus::ImageCodecInfo* pCodecs, unsigned int nCodecs );
		static void BuildCodecFilterString( const Gdiplus::ImageCodecInfo* pCodecs, unsigned int nCodecs, String& strFilter, vector< GUID >& aguidFileTypes, const wchar_t* pszAllFilesDescription, DWORD dwExclude, TCHAR chSeparator );
		static bool ShouldExcludeFormat( REFGUID guidFileType, DWORD dwExclude ) throw();
		void UpdateBitmapInfo( DIBOrientation eOrientation );


		static int ComputePitch( int nWidth, int nBPP )
		{
			return( (((nWidth*nBPP)+31)/32)*4 );
		}
		static void GenerateHalftonePalette( LPRGBQUAD prgbPalette );
	public:
		COLORREF GetTransparentRGB() const;

	private:
		mutable HDC m_hDC;
		mutable int m_nDCRefCount;
		mutable HBITMAP m_hOldBitmap;

		static CDCCache s_cache;
	};
	//CImage::CInitGDIPlus CImage::s_initGDIPlus;
	//CImage::CDCCache	CImage::s_cache;

	inline CImageDC::CImageDC( const Image& image ) throw( ... ) :
	m_image( image ),m_hDC( image.GetDC() )
	{
		if( m_hDC == nullptr )
		{
			throw( E_OUTOFMEMORY );
		}
	}

	inline CImageDC::~CImageDC() throw()
	{
		m_image.ReleaseDC();
	}

	inline CImageDC::operator HDC() const throw()
	{
		return( m_hDC );
	}

	inline Image::CInitGDIPlus::CInitGDIPlus():
	m_dwToken( 0 ), m_nCImageObjects( 0 )
	{
		__try
		{
			InitializeCriticalSection(&m_sect);
		}
		__except( STATUS_NO_MEMORY == GetExceptionCode() )
		{
			throw( E_OUTOFMEMORY );
		}
	}

	inline Image::CInitGDIPlus::~CInitGDIPlus() throw()
	{
		ReleaseGDIPlus();
		DeleteCriticalSection(&m_sect);
	}

    // Gdiplus::GdiplusStartup会创建一个线程：
    // 00 0106f3cc 739578a9 00000000 00000000 73957832 kernel32!CreateThreadStub (FPO: [Non-Fpo])
    // 01 0106f3fc 73957a6f 00000000 00000000 739578ca gdiplus!CreateThreadAndAddRefDll+0x4e (FPO: [Non-Fpo])
    // 02 0106f424 7396a4f6 fc6cc4e1 73a8375c 0106f570 gdiplus!BackgroundThreadStartup+0x4d (FPO: [0,0,4])
    // 03 0106f458 73935656 0106f570 0106f59c 0106f484 gdiplus!InternalGdiplusStartup+0x5cf (FPO: [Non-Fpo])
    // 04 0106f470 53ebe494 54089594 0106f570 0106f560 gdiplus!GdiplusStartup+0x56 (FPO: [Non-Fpo])    
	inline bool Image::CInitGDIPlus::Init() throw()
	{
		EnterCriticalSection(&m_sect);
		bool fRet = true;
		if( m_dwToken == 0 )
		{
			Gdiplus::GdiplusStartupInput input;
			Gdiplus::GdiplusStartupOutput output;
			Gdiplus::Status status = Gdiplus::GdiplusStartup( &m_dwToken, &input, &output );
			if( status != Gdiplus::Ok )
				fRet = false;
		}
		LeaveCriticalSection(&m_sect);
		return fRet;
	}

	inline void Image::CInitGDIPlus::ReleaseGDIPlus() throw()
	{
		EnterCriticalSection(&m_sect);
		if( m_dwToken != 0 )
		{
			Gdiplus::GdiplusShutdown( m_dwToken );
		}
		m_dwToken = 0;
		LeaveCriticalSection(&m_sect);
	}

	inline void Image::CInitGDIPlus::IncreaseCImageCount() throw()
	{
		EnterCriticalSection(&m_sect);
		m_nCImageObjects++;
		LeaveCriticalSection(&m_sect);
	}

	inline void Image::CInitGDIPlus::DecreaseCImageCount() throw()
	{
		EnterCriticalSection(&m_sect);
		if( --m_nCImageObjects == 0 )
			ReleaseGDIPlus();
		LeaveCriticalSection(&m_sect);
	}

	inline CDCCache::CDCCache() throw()
	{
		int iDC;

		for( iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
		{
			m_ahDCs[iDC] = nullptr;
		}
	}

	inline CDCCache::~CDCCache() throw()
	{
		int iDC;

		for( iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
		{
			if( m_ahDCs[iDC] != nullptr )
			{
				::DeleteDC( m_ahDCs[iDC] );
			}
		}
	}

	inline HDC CDCCache::GetDC() throw()
	{
		HDC hDC;

		for( int iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
		{
			hDC = static_cast< HDC >( InterlockedExchangePointer( reinterpret_cast< void** >(&m_ahDCs[iDC]), nullptr ) );
			if( hDC != nullptr )
			{
				return( hDC );
			}
		}

		hDC = ::CreateCompatibleDC( nullptr );

		return( hDC );
	}

	inline void CDCCache::ReleaseDC( HDC hDC ) throw()
	{
		for( int iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
		{
			HDC hOldDC;

			hOldDC = static_cast< HDC >( InterlockedExchangePointer( reinterpret_cast< void** >(&m_ahDCs[iDC]), hDC ) );
			if( hOldDC == nullptr )
			{
				return;
			}
			else
			{
				hDC = hOldDC;
			}
		}
		if( hDC != nullptr )
		{
			::DeleteDC( hDC );
		}
	}

	inline Image::Image() throw() :
	m_hBitmap( nullptr ),
		m_pBits( nullptr ),
		m_hDC( nullptr ),
		m_nDCRefCount( 0 ),
		m_hOldBitmap( nullptr ),
		m_nWidth( 0 ),
		m_nHeight( 0 ),
		m_nPitch( 0 ),
		m_nBPP( 0 ),
		m_iTransparentColor( -1 ),
		m_colorTransparent(RGB(255,0,255)),
		m_bHasAlphaChannel( false ),
		m_bIsDIBSection( false )
	{
		s_initGDIPlus.IncreaseCImageCount();
	}

	inline Image::~Image() throw()
	{
		Destroy();
		s_initGDIPlus.DecreaseCImageCount();
	}

	inline Image::operator HBITMAP() const throw()
	{
		return( m_hBitmap );
	}

#if WINVER >= 0x0500
	inline BOOL Image::AlphaBlend( HDC hDestDC, int xDest, int yDest, 
		BYTE bSrcAlpha, BYTE bBlendOp ) const throw()
	{
		return( AlphaBlend( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, 
			m_nWidth, m_nHeight, bSrcAlpha, bBlendOp ) );
	}

	inline BOOL Image::AlphaBlend( HDC hDestDC, const POINT& pointDest, 
		BYTE bSrcAlpha, BYTE bBlendOp ) const throw()
	{
		return( AlphaBlend( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 
			0, 0, m_nWidth, m_nHeight, bSrcAlpha, bBlendOp ) );
	}

	inline BOOL Image::AlphaBlend( HDC hDestDC, int xDest, int yDest, 
		int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, 
		int nSrcHeight, BYTE bSrcAlpha, BYTE bBlendOp ) const throw()
	{
		BLENDFUNCTION blend;
		BOOL bResult;

		blend.SourceConstantAlpha = bSrcAlpha;
		blend.BlendOp = bBlendOp;
		blend.BlendFlags = 0;
		if( m_bHasAlphaChannel )
		{
			blend.AlphaFormat = AC_SRC_ALPHA;
		}
		else
		{
			blend.AlphaFormat = 0;
		}

		GetDC();

		bResult = ::AlphaBlend( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC, 
			xSrc, ySrc, nSrcWidth, nSrcHeight, blend );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL Image::AlphaBlend( HDC hDestDC, const RECT& rectDest, 
		const RECT& rectSrc, BYTE bSrcAlpha, BYTE bBlendOp ) const throw()
	{
		return( AlphaBlend( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top, 
			rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, bSrcAlpha, 
			bBlendOp ) );
	}
#endif  // WINVER >= 0x0500

	inline void Image::Attach( HBITMAP hBitmap, DIBOrientation eOrientation ) throw()
	{
		assert( m_hBitmap == nullptr );
		assert( hBitmap != nullptr );

		m_hBitmap = hBitmap;

		UpdateBitmapInfo( eOrientation );
	}

	inline BOOL Image::BitBlt( HDC hDestDC, int xDest, int yDest, DWORD dwROP ) const throw()
	{
		return( BitBlt( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, dwROP ) );
	}

	inline BOOL Image::BitBlt( HDC hDestDC, const POINT& pointDest, DWORD dwROP ) const throw()
	{
		return( BitBlt( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight,
			0, 0, dwROP ) );
	}

	inline BOOL Image::BitBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
		int nDestHeight, int xSrc, int ySrc, DWORD dwROP ) const throw()
	{
		BOOL bResult;

		assert( m_hBitmap != nullptr );
		assert( hDestDC != nullptr );

		GetDC();

		bResult = ::BitBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC, 
			xSrc, ySrc, dwROP );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL Image::BitBlt( HDC hDestDC, const RECT& rectDest, 
		const POINT& pointSrc, DWORD dwROP ) const throw()
	{
		return( BitBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, pointSrc.x, pointSrc.y, 
			dwROP ) );
	}

	// Remarks:
	//	如果nHeight为负值，表示要创建一个bottom-up类型的DIB，否则为一个top-down类型的
	// 
	inline BOOL Image::Create( int nWidth, int nHeight, int nBPP, DWORD dwFlags ) throw()
	{
		return( CreateEx( nWidth, nHeight, nBPP, BI_RGB, nullptr, dwFlags ) );
	}

	inline BOOL Image::CreateEx( int nWidth, int nHeight, int nBPP, DWORD eCompression, 
		const DWORD* pdwBitfields, DWORD dwFlags ) throw()
	{
		//USES_ATL_SAFE_ALLOCA;
		LPBITMAPINFO pbmi;
		HBITMAP hBitmap;

		assert( (eCompression == BI_RGB) || (eCompression == BI_BITFIELDS) );
		if( dwFlags&createAlphaChannel )
		{
			assert( (nBPP == 32) && (eCompression == BI_RGB) );
		}

		pbmi = (LPBITMAPINFO)_alloca(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256);
		if( pbmi == nullptr )
			return FALSE;

		memset( &pbmi->bmiHeader, 0, sizeof( pbmi->bmiHeader ) );
		pbmi->bmiHeader.biSize = sizeof( pbmi->bmiHeader );
		pbmi->bmiHeader.biWidth = nWidth;
		pbmi->bmiHeader.biHeight = nHeight;
		pbmi->bmiHeader.biPlanes = 1;
		pbmi->bmiHeader.biBitCount = USHORT( nBPP );
		pbmi->bmiHeader.biCompression = eCompression;
		if( nBPP <= 8 )
		{
			assert( eCompression == BI_RGB );
#pragma warning(push)
#pragma warning(disable:4068) //Disable unknown pragma warning that prefast pragma causes.
#pragma prefast(push)
#pragma prefast(disable:203, "no buffer overrun here, buffer was alocated properly")	
			memset( pbmi->bmiColors, 0, 256*sizeof( RGBQUAD ) );
#pragma prefast(pop)
#pragma warning(pop)
		}

		else 
		{
			if( eCompression == BI_BITFIELDS )
			{
				assert( pdwBitfields != nullptr );
				/* < BEGIN 20100623 libo MODIFY，　在VS2005以下，没有memcpy_s函数 */
				/*Checked::*///memcpy_s(pbmi->bmiColors, 3*sizeof( DWORD ), pdwBitfields, 3*sizeof( DWORD ));
				memcpy( pbmi->bmiColors,  pdwBitfields, 3*sizeof( DWORD ) );
				/* 20100623 libo MODIFY END > */
			}
		}

		hBitmap = ::CreateDIBSection( nullptr, pbmi, DIB_RGB_COLORS, &m_pBits, nullptr, 0 );
		if (hBitmap == nullptr)
		{
			return( FALSE );
		}

		Attach( hBitmap, (nHeight < 0) ? DIBOR_TOPDOWN : DIBOR_BOTTOMUP );

		if (dwFlags&createAlphaChannel)
		{
			m_bHasAlphaChannel = true;
		}

		return( TRUE );
	}

	inline void Image::Destroy() throw()
	{
		HBITMAP hBitmap;

		if( m_hBitmap != nullptr )
		{
			hBitmap = Detach();
			::DeleteObject( hBitmap );
		}
		m_bHasAlphaChannel = false;
	}

	inline HBITMAP Image::Detach() throw()
	{
		HBITMAP hBitmap;

		assert( m_hBitmap != nullptr );
		assert( m_hDC == nullptr );

		hBitmap = m_hBitmap;
		m_hBitmap = nullptr;
		m_pBits = nullptr;
		m_nWidth = 0;
		m_nHeight = 0;
		m_nBPP = 0;
		m_nPitch = 0;
		m_iTransparentColor = -1;
		m_bHasAlphaChannel = false;
		m_bIsDIBSection = false;

		return( hBitmap );
	}

	inline BOOL Image::Draw( HDC hDestDC, const RECT& rectDest, byte bAlpha ) const throw()
	{
		return( Draw( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, 0, 0, m_nWidth, 
			m_nHeight, bAlpha ) );
	}

	inline BOOL Image::Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, byte bAlpha ) const throw()
	{
		return( Draw( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, m_nWidth, m_nHeight, bAlpha ) );
	}

	inline BOOL Image::Draw( HDC hDestDC, const POINT& pointDest, byte bAlpha ) const throw()
	{
		return( Draw( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight, bAlpha ) );
	}

	inline BOOL Image::Draw( HDC hDestDC, int xDest, int yDest, byte bAlpha ) const throw()
	{
		return( Draw( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight, bAlpha ) );
	}

	inline BOOL Image::Draw( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, byte bAlpha ) const throw()
	{
		return( Draw( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top, 
			rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, bAlpha ) );
	}

	inline const void* Image::GetBits() const throw()
	{
		assert( m_hBitmap != nullptr );
		assert( IsDIBSection() );

		return( m_pBits );
	}

	inline void* Image::GetBits() throw()
	{
		assert( m_hBitmap != nullptr );
		assert( IsDIBSection() );

		return( m_pBits );
	}

	inline int Image::GetBPP() const throw()
	{
		assert( m_hBitmap != nullptr );

		return( m_nBPP );
	}

	inline void Image::GetColorTable( unsigned int iFirstColor, unsigned int nColors, 
		RGBQUAD* prgbColors ) const throw()
	{
		assert( m_hBitmap != nullptr );
		assert( m_pBits != nullptr );
		assert( IsIndexed() );

		GetDC();

		::GetDIBColorTable( m_hDC, iFirstColor, nColors, prgbColors );

		ReleaseDC();
	}

	inline HDC Image::GetDC() const throw()
	{
		assert( m_hBitmap != nullptr );

		m_nDCRefCount++;
		if( m_hDC == nullptr )
		{
			m_hDC = s_cache.GetDC();
			m_hOldBitmap = HBITMAP( ::SelectObject( m_hDC, m_hBitmap ) );
		}

		return( m_hDC );
	}

	inline bool Image::ShouldExcludeFormat( REFGUID guidFileType, DWORD dwExclude ) throw()
	{
		static const GUID* apguidFormats[] =
		{
			&Gdiplus::ImageFormatGIF,
			&Gdiplus::ImageFormatBMP,
			&Gdiplus::ImageFormatEMF,
			&Gdiplus::ImageFormatWMF,
			&Gdiplus::ImageFormatJPEG,
			&Gdiplus::ImageFormatPNG,
			&Gdiplus::ImageFormatTIFF,
			&Gdiplus::ImageFormatIcon,
			nullptr
		};

		assert( (dwExclude|excludeValid) == excludeValid );
		for( int iFormat = 0; apguidFormats[iFormat] != nullptr; iFormat++ )
		{
			if( guidFileType == *apguidFormats[iFormat] )
			{
				return( (dwExclude&(1<<iFormat)) != 0 );
			}
		}

		return( (dwExclude&excludeOther) != 0 );
	}

	inline void Image::BuildCodecFilterString( const Gdiplus::ImageCodecInfo* pCodecs, unsigned int nCodecs,
		String& strFilter, vector< GUID >& aguidFileTypes, const wchar_t* pszAllFilesDescription, 
		DWORD dwExclude, TCHAR chSeparator )
	{
#ifdef _UNICODE // libo 20101117 remove 使用于多字节编码
		if( pszAllFilesDescription != nullptr )
		{
			aguidFileTypes.push_back( GUID_NULL );
		}

		String strAllExtensions;
		String strTempFilter;
		for( unsigned int iCodec = 0; iCodec < nCodecs; iCodec++ )
		{
			const Gdiplus::ImageCodecInfo* pCodec = &pCodecs[iCodec];

			if( !ShouldExcludeFormat( pCodec->FormatID, dwExclude ) )
			{
				LPCWSTR pwszFilenameExtension( pCodec->FilenameExtension );
				strTempFilter +=  pCodec->FormatDescription ;
				strTempFilter += _T( " (" );
				strTempFilter += pwszFilenameExtension;
				strTempFilter += _T( ")" );
				strTempFilter += chSeparator;
				strTempFilter += pwszFilenameExtension;
				strTempFilter += chSeparator;

				aguidFileTypes.push_back( pCodec->FormatID );

				if( strAllExtensions != _T("") )
				{
					strAllExtensions += _T( ";" );
				}
				strAllExtensions += pwszFilenameExtension;
			}
		}

		if( pszAllFilesDescription != nullptr )
		{
			strFilter += pszAllFilesDescription;
			strFilter += chSeparator;
			strFilter += strAllExtensions;
			strFilter += chSeparator;
		}
		strFilter += strTempFilter;

		strFilter += chSeparator;
		if( aguidFileTypes.size() == 0 )
		{
			strFilter += chSeparator;
		}
#endif
	}

	inline HRESULT Image::GetImporterFilterString( String& strImporters, 
		vector< GUID >& aguidFileTypes, const wchar_t* pszAllFilesDescription /* = nullptr */,
		DWORD dwExclude /* = excludeDefaultLoad */, TCHAR chSeparator /* = '|' */ )
	{
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		unsigned int nCodecs;
		unsigned int nSize;
		Gdiplus::Status status;
		Gdiplus::ImageCodecInfo* pCodecs;

		status = Gdiplus::GetImageDecodersSize( &nCodecs, &nSize );
		//USES_ATL_SAFE_ALLOCA;
		pCodecs = static_cast< Gdiplus::ImageCodecInfo* >( _alloca(nSize) );

		if( pCodecs == nullptr )
			return E_OUTOFMEMORY;

		status = Gdiplus::GetImageDecoders( nCodecs, nSize, pCodecs );
		BuildCodecFilterString( pCodecs, nCodecs, strImporters, aguidFileTypes, pszAllFilesDescription, dwExclude, chSeparator );

		return( S_OK );
	}

	inline HRESULT Image::GetExporterFilterString( String& strExporters, 
		vector< GUID >& aguidFileTypes, const wchar_t* pszAllFilesDescription /* = nullptr */,
		DWORD dwExclude /* = excludeDefaultSave */, TCHAR chSeparator /* = '|' */ )
	{
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		unsigned int nCodecs;
		unsigned int nSize;
		Gdiplus::Status status;
		Gdiplus::ImageCodecInfo* pCodecs;

		status = Gdiplus::GetImageDecodersSize( &nCodecs, &nSize );
		//USES_ATL_SAFE_ALLOCA;
		pCodecs = static_cast< Gdiplus::ImageCodecInfo* >( _alloca(nSize) );

		if( pCodecs == nullptr )
			return E_OUTOFMEMORY;

		status = Gdiplus::GetImageDecoders( nCodecs, nSize, pCodecs );
		BuildCodecFilterString( pCodecs, nCodecs, strExporters, aguidFileTypes, pszAllFilesDescription, dwExclude, chSeparator );

		return( S_OK );
	}

	inline int Image::GetHeight() const throw()
	{
		assert( m_hBitmap != nullptr );

		return( m_nHeight );
	}

	inline int Image::GetMaxColorTableEntries() const throw()
	{
		assert( m_hBitmap != nullptr );
		assert( IsDIBSection() );

		if( IsIndexed() )
		{
			return( 1<<m_nBPP );
		}
		else
		{
			return( 0 );
		}
	}

	inline int Image::GetPitch() const throw()
	{
		assert( m_hBitmap != nullptr );
		assert( IsDIBSection() );

		return( m_nPitch );
	}

	inline COLORREF Image::GetPixel( int x, int y ) const throw()
	{
		assert( m_hBitmap != nullptr );
		assert( (x >= 0) && (x < m_nWidth) );
		assert( (y >= 0) && (y < m_nHeight) );

		GetDC();

		COLORREF clr = ::GetPixel( m_hDC, x, y );

		ReleaseDC();

		return( clr );
	}

	inline const void* Image::GetPixelAddress( int x, int y ) const throw()
	{
		assert( m_hBitmap != nullptr );
		assert( IsDIBSection() );
		assert( (x >= 0) && (x < m_nWidth) );
		assert( (y >= 0) && (y < m_nHeight) );

		return( LPBYTE( m_pBits )+(y*m_nPitch)+((x*m_nBPP)/8) );
	}

	inline void* Image::GetPixelAddress( int x, int y ) throw()
	{
		assert( m_hBitmap != nullptr );
		assert( IsDIBSection() );
		assert( (x >= 0) && (x < m_nWidth) );
		assert( (y >= 0) && (y < m_nHeight) );

		return( LPBYTE( m_pBits )+(y*m_nPitch)+((x*m_nBPP)/8) );
	}

	inline LONG Image::GetTransparentColor() const throw()
	{
		assert( m_hBitmap != nullptr );
		assert( (m_nBPP == 4) || (m_nBPP == 8) );

		return( m_iTransparentColor );
	}

	inline int Image::GetWidth() const throw()
	{
		assert( m_hBitmap != nullptr );

		return( m_nWidth );
	}

	inline bool Image::IsDIBSection() const throw()
	{
		return( m_bIsDIBSection );
	}

	inline bool Image::IsIndexed() const throw()
	{
		assert( m_hBitmap != nullptr );
		assert( IsDIBSection() );

		return( m_nBPP <= 8 );
	}

	inline bool Image::IsNull() const throw()
	{
		return( m_hBitmap == nullptr );
	}

	inline HRESULT Image::Load( IStream* pStream ) throw()
	{
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		Gdiplus::Bitmap bmSrc( pStream );
		if( bmSrc.GetLastStatus() != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		return( CreateFromGdiplusBitmap( bmSrc, m_bHasAlphaChannel ) );
	}

	inline HRESULT Image::Load( const wchar_t* pszFileName ) throw()
	{
#ifdef _UNICODE // libo 20101117 remove 只适用用于多字节编码
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		Gdiplus::Bitmap bmSrc( pszFileName );
		if( bmSrc.GetLastStatus() != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		return( CreateFromGdiplusBitmap( bmSrc, m_bHasAlphaChannel ) );
#else
		return -1;
#endif
	}

	// 2012.11.28 增加强制创建一个32位带alpha通道的功能，使得24位格式图片也能在分层窗口上显示
	inline HRESULT Image::CreateFromGdiplusBitmap(Gdiplus::Bitmap& bmSrc, bool bForceAlpha) throw()
	{
		m_bHasAlphaChannel = bForceAlpha;

		unsigned int  nBPP = 32;
		DWORD dwFlags = 0;

		Gdiplus::PixelFormat eSrcPixelFormat = bmSrc.GetPixelFormat();
		Gdiplus::PixelFormat eDestPixelFormat = PixelFormat32bppPARGB;
		if( eSrcPixelFormat&PixelFormatGDI )
		{
			nBPP = Gdiplus::GetPixelFormatSize( eSrcPixelFormat );
			eDestPixelFormat = eSrcPixelFormat;
		}
		if( m_bHasAlphaChannel || Gdiplus::IsAlphaPixelFormat( eSrcPixelFormat ) )  // Add by libo 如果需要强制创建一个带alpha channel的位图
		{
			nBPP = 32;
			dwFlags |= createAlphaChannel;
			//eDestPixelFormat = PixelFormat32bppARGB;  // 2015/3/16 AlphaBlend需要预乘数据
			eDestPixelFormat = PixelFormat32bppPARGB;
		}

		BOOL bSuccess = Create( bmSrc.GetWidth(), bmSrc.GetHeight(), nBPP, dwFlags );
		if( !bSuccess )
		{
			return( E_FAIL );
		}
		//USES_ATL_SAFE_ALLOCA;
		Gdiplus::ColorPalette* pPalette = nullptr;
		if (false == m_bHasAlphaChannel && Gdiplus::IsIndexedPixelFormat(eSrcPixelFormat))  // 有可能外部强制指定了创建alpha通道的32位图片，这时候没有color table
		{
			unsigned int nPaletteSize = bmSrc.GetPaletteSize();
			pPalette = static_cast< Gdiplus::ColorPalette* >( _alloca(nPaletteSize) );

			if( pPalette == nullptr )
				return E_OUTOFMEMORY;

			bmSrc.GetPalette( pPalette, nPaletteSize );

			RGBQUAD argbPalette[256];
			assert( (pPalette->Count > 0) && (pPalette->Count <= 256) );
			for( unsigned int iColor = 0; iColor < pPalette->Count; iColor++ )
			{
				Gdiplus::ARGB color = pPalette->Entries[iColor];
				argbPalette[iColor].rgbRed = (BYTE)( (color>>RED_SHIFT) & 0xff );
				argbPalette[iColor].rgbGreen = (BYTE)( (color>>GREEN_SHIFT) & 0xff );
				argbPalette[iColor].rgbBlue = (BYTE)( (color>>BLUE_SHIFT) & 0xff );
				argbPalette[iColor].rgbReserved = 0;
			}

			SetColorTable( 0, pPalette->Count, argbPalette );
		}

		if (eDestPixelFormat == eSrcPixelFormat)
		{
			// The pixel formats are identical, so just memcpy the rows.
			Gdiplus::BitmapData data;
			Gdiplus::Rect rect( 0, 0, GetWidth(), GetHeight() );
			if(bmSrc.LockBits( &rect, Gdiplus::ImageLockModeRead, eSrcPixelFormat, &data )!=Gdiplus::Ok)
			{
				return E_OUTOFMEMORY;
			}

			size_t nBytesPerRow = ( (nBPP*GetWidth()+8-1)&~(8 -1) )/ 8;//AtlAlignUp( nBPP*GetWidth(), 8 )/8;
			
			BYTE* pbDestRow = static_cast< BYTE* >( GetBits() );
			BYTE* pbSrcRow = static_cast< BYTE* >( data.Scan0 );
	
			for( int y = 0; y < GetHeight(); y++ )
			{
				//////////////////////////////////////////////////////////////////////////
				//
				//                         实现预乘。（2015/3/16 指定PixelFormat32bppPARGB格式即代表创建预乘的格式）
				//
				// First you need to create a pre-multiplied 32-bits-per-pixel (bpp) bitmap using a
				// blue-green-red-alpha (BGRA) color channel byte order. Pre-multiplied just means
				// that the color channel values have already been multiplied by the alpha value.
				// This tends to provide better performance for alpha blending images, but it means
				// you need to reverse the process by dividing the color values by the alpha value
				// to get their true color values.
				//
				//////////////////////////////////////////////////////////////////////////
// 				if (m_bHasAlphaChannel)
// 				{
// 					for (int i = 0; i < (int)nBytesPerRow; i+=4)
// 					{
// 						byte nAlpha = pbSrcRow[i+3];
// 						pbDestRow[i]   = pbSrcRow[i]  *nAlpha/255;
// 						pbDestRow[i+1] = pbSrcRow[i+1]*nAlpha/255;
// 						pbDestRow[i+2] = pbSrcRow[i+2]*nAlpha/255;
// 						pbDestRow[i+3] = nAlpha;
// 					}
// 				}
// 				else 
				{
					/* < BEGIN 20100623 libo MODIFY，　在VS2005以下，没有memcpy_s函数 */
					//memcpy_s(pbDestRow, nBytesPerRow, pbSrcRow, nBytesPerRow);
					memcpy( pbDestRow,  pbSrcRow, nBytesPerRow );
					/* 20100623 libo MODIFY END > */
				}
				pbDestRow += GetPitch();
				pbSrcRow += data.Stride;
			}

			bmSrc.UnlockBits( &data );
		}
		else
		{
			// Let GDI+ work its magic
			Gdiplus::Bitmap bmDest( GetWidth(), GetHeight(), GetPitch(), eDestPixelFormat, static_cast< BYTE* >( GetBits() ) );
			Gdiplus::Graphics gDest( &bmDest );
            //gDest.DrawImage( &bmSrc, 0,0); //-- libo 2014.3.19 直接这么绘制调用会导致图片被拉伸（如24位的bmp图片）,DPI不一致

            //bmSrc.SetResolution(96,96);
			gDest.DrawImage( &bmSrc, 0,0,GetWidth(), GetHeight()); 
		}

		return( S_OK );
	}

	// 反预乘
	// 场景：调用Save时，需要反预乘之后，再保存。否则再用gdiplus加载出来的图片显示不正确
	//       或者使用PixelFormat32bppPArgb来创建Gdiplus::Bitmap
	inline void  Image::UnPreMulti()
	{
		if (m_nBPP != 32)
			return;

		byte* pbits = (byte*)m_pBits;
		int nBytesPerRow = abs(m_nPitch);
		for (int y = 0; y < m_nHeight; y++)
		{
			for (int x = 0; x < nBytesPerRow; x+=4)
			{
				byte a = pbits[x+3];
				if (a)
				{
					pbits[x]   = (pbits[x]   * 255)/a;
					pbits[x+1] = (pbits[x+1] * 255)/a;
					pbits[x+2] = (pbits[x+2] * 255)/a;
				}
			}
			pbits += m_nPitch;
		}
	}

#if 0
	// 指定创建32位格式的Image。为了避免24位的图片有没有alpha值，导致在分层窗口上显示不出来，在这里将alpha填充为255
	inline HRESULT Image::Create32BPPFromGdiplusBitmap( Gdiplus::Bitmap& bmSrc ) throw()
	{
		Gdiplus::PixelFormat eSrcPixelFormat = bmSrc.GetPixelFormat();
		unsigned int nBPP = 32;
		DWORD dwFlags = 0;
		Gdiplus::PixelFormat eDestPixelFormat = PixelFormat32bppRGB;
		if( eSrcPixelFormat&PixelFormatGDI )
		{
			nBPP = Gdiplus::GetPixelFormatSize( eSrcPixelFormat );
			eDestPixelFormat = eSrcPixelFormat;
		}
		if( Gdiplus::IsAlphaPixelFormat( eSrcPixelFormat ) )
		{
			nBPP = 32;
			dwFlags |= createAlphaChannel;
			eDestPixelFormat = PixelFormat32bppARGB;
		}

		BOOL bSuccess = Create( bmSrc.GetWidth(), bmSrc.GetHeight(), nBPP, dwFlags );
		if( !bSuccess )
		{
			return( E_FAIL );
		}
		//USES_ATL_SAFE_ALLOCA;
		Gdiplus::ColorPalette* pPalette = nullptr;
		if( Gdiplus::IsIndexedPixelFormat( eSrcPixelFormat ) )
		{
			unsigned int nPaletteSize = bmSrc.GetPaletteSize();
			pPalette = static_cast< Gdiplus::ColorPalette* >( _alloca(nPaletteSize) );

			if( pPalette == nullptr )
				return E_OUTOFMEMORY;

			bmSrc.GetPalette( pPalette, nPaletteSize );

			RGBQUAD argbPalette[256];
			assert( (pPalette->Count > 0) && (pPalette->Count <= 256) );
			for( unsigned int iColor = 0; iColor < pPalette->Count; iColor++ )
			{
				Gdiplus::ARGB color = pPalette->Entries[iColor];
				argbPalette[iColor].rgbRed = (BYTE)( (color>>RED_SHIFT) & 0xff );
				argbPalette[iColor].rgbGreen = (BYTE)( (color>>GREEN_SHIFT) & 0xff );
				argbPalette[iColor].rgbBlue = (BYTE)( (color>>BLUE_SHIFT) & 0xff );
				argbPalette[iColor].rgbReserved = 0;
			}

			SetColorTable( 0, pPalette->Count, argbPalette );
		}

		if( eDestPixelFormat == eSrcPixelFormat  )
		{
			// The pixel formats are identical, so just memcpy the rows.
			Gdiplus::BitmapData data;
			Gdiplus::Rect rect( 0, 0, GetWidth(), GetHeight() );
			if(bmSrc.LockBits( &rect, Gdiplus::ImageLockModeRead, eSrcPixelFormat, &data )!=Gdiplus::Ok)
			{
				return E_OUTOFMEMORY;
			}

			size_t nBytesPerRow = ( (nBPP*GetWidth()+8-1)&~(8 -1) )/ 8;//AtlAlignUp( nBPP*GetWidth(), 8 )/8;

			BYTE* pbDestRow = static_cast< BYTE* >( GetBits() );
			BYTE* pbSrcRow = static_cast< BYTE* >( data.Scan0 );

			for( int y = 0; y < GetHeight(); y++ )
			{
				//////////////////////////////////////////////////////////////////////////
				//
				//                         实现预乘。
				//
				// First you need to create a pre-multiplied 32-bits-per-pixel (bpp) bitmap using a
				// blue-green-red-alpha (BGRA) color channel byte order. Pre-multiplied just means
				// that the color channel values have already been multiplied by the alpha value.
				// This tends to provide better performance for alpha blending images, but it means
				// you need to reverse the process by dividing the color values by the alpha value
				// to get their true color values.
				//
				//////////////////////////////////////////////////////////////////////////
				if (m_bHasAlphaChannel)
				{
					for (int i = 0; i < (int)nBytesPerRow; i+=4)
					{
						byte nAlpha = pbSrcRow[i+3];
						pbDestRow[i]   = pbSrcRow[i]  *nAlpha/255;
						pbDestRow[i+1] = pbSrcRow[i+1]*nAlpha/255;
						pbDestRow[i+2] = pbSrcRow[i+2]*nAlpha/255;
						pbDestRow[i+3] = nAlpha;
					}
				}
				else 
				{
					/* < BEGIN 20100623 libo MODIFY，　在VS2005以下，没有memcpy_s函数 */
					//memcpy_s(pbDestRow, nBytesPerRow, pbSrcRow, nBytesPerRow);
					memcpy( pbDestRow,  pbSrcRow, nBytesPerRow );
					/* 20100623 libo MODIFY END > */
				}
				pbDestRow += GetPitch();
				pbSrcRow += data.Stride;
			}

			bmSrc.UnlockBits( &data );
		}
		else
		{
			// Let GDI+ work its magic
			Gdiplus::Bitmap bmDest( GetWidth(), GetHeight(), GetPitch(), eDestPixelFormat, static_cast< BYTE* >( GetBits() ) );
			Gdiplus::Graphics gDest( &bmDest );

			gDest.DrawImage( &bmSrc, 0, 0 );
		}

		return( S_OK );
	}
#endif

	/* <BEGIN  ADD libo 20101116 添加从资源中加载png图片的方法 */
	inline void Image::LoadFromResource( HINSTANCE hInstance, unsigned int nIDResource, TCHAR* szResourceType ) throw()
	{
		// 如果没有指定资源类型，那么可能就是默认的bmp图片
		if( nullptr == szResourceType )
		{
			this->LoadFromResource( hInstance, MAKEINTRESOURCE(nIDResource) );
			return;
		}

		// 查找资源
		HRSRC hRsrc = ::FindResource( hInstance, MAKEINTRESOURCE(nIDResource), szResourceType);
		if (hRsrc == nullptr) 
			return;

		// 加载资源
		HGLOBAL hImgData = ::LoadResource( hInstance, hRsrc);
		if (hImgData == nullptr)
			return;

		// 锁定内存中的指定资源
		LPVOID   lpVoid  = ::LockResource(hImgData);
		DWORD    dwSize  = ::SizeofResource(hInstance, hRsrc);

		this->LoadFromData(lpVoid,dwSize);

		// 释放资源
		::FreeResource(hImgData);
	}

	//
	//	从文件数据中加载图片
	//		pImageData
	//			[in]	文件流数据
	//		nSize
	//			[in]	pImageData大小
	//
	inline bool Image::LoadFromData( void* pImageData, int nSize  )
	{
		LPSTREAM pStream = nullptr;
		HGLOBAL  hNew    = ::GlobalAlloc(GHND, nSize);

		LPBYTE lpByte    = (LPBYTE)::GlobalLock(hNew);
		::memcpy(lpByte, pImageData, nSize);
		::GlobalUnlock(hNew);

		// 从指定内存创建流对象
		HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
		if ( ht != S_OK )
		{
			GlobalFree(hNew);
			return false;
		}

        // 加载图片
        HRESULT hr = this->Load(pStream);
        pStream->Release();

		if( FAILED(hr) )
		    return false;
        return true;;
	}
	/* ADD END> */

	inline void Image::LoadFromResource( HINSTANCE hInstance, const wchar_t* pszResourceName ) throw()
	{
		HBITMAP hBitmap;

		hBitmap = HBITMAP( ::LoadImage( hInstance, pszResourceName, IMAGE_BITMAP, 0, 
			0, LR_CREATEDIBSECTION ) );

		Attach( hBitmap );
	}

	inline void Image::LoadFromResource( HINSTANCE hInstance, unsigned int nIDResource ) throw()
	{
		LoadFromResource( hInstance, MAKEINTRESOURCE( nIDResource ) );
	}

	inline BOOL Image::MaskBlt( HDC hDestDC, int xDest, int yDest, int nWidth, 
		int nHeight, int xSrc, int ySrc, HBITMAP hbmMask, int xMask, int yMask,
		DWORD dwROP ) const throw()
	{
		BOOL bResult;

		assert( m_hBitmap != nullptr );
		assert( hDestDC != nullptr );

		GetDC();

		bResult = ::MaskBlt( hDestDC, xDest, yDest, nWidth, nHeight, m_hDC, xSrc, 
			ySrc, hbmMask, xMask, yMask, dwROP );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL Image::MaskBlt( HDC hDestDC, const RECT& rectDest, 
		const POINT& pointSrc, HBITMAP hbmMask, const POINT& pointMask, 
		DWORD dwROP ) const throw()
	{
		return( MaskBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, pointSrc.x, pointSrc.y, 
			hbmMask, pointMask.x, pointMask.y, dwROP ) );
	}

	inline BOOL Image::MaskBlt( HDC hDestDC, int xDest, int yDest, HBITMAP hbmMask, 
		DWORD dwROP ) const throw()
	{
		return( MaskBlt( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, hbmMask, 
			0, 0, dwROP ) );
	}

	inline BOOL Image::MaskBlt( HDC hDestDC, const POINT& pointDest, HBITMAP hbmMask,
		DWORD dwROP ) const throw()
	{
		return( MaskBlt( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 0, 
			0, hbmMask, 0, 0, dwROP ) );
	}

	inline BOOL Image::PlgBlt( HDC hDestDC, const POINT* pPoints, int xSrc, 
		int ySrc, int nSrcWidth, int nSrcHeight, HBITMAP hbmMask, int xMask, 
		int yMask ) const throw()
	{
		BOOL bResult;

		assert( m_hBitmap != nullptr );
		assert( hDestDC != nullptr );

		GetDC();

		bResult = ::PlgBlt( hDestDC, pPoints, m_hDC, xSrc, ySrc, nSrcWidth, 
			nSrcHeight, hbmMask, xMask, yMask );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL Image::PlgBlt( HDC hDestDC, const POINT* pPoints, 
		const RECT& rectSrc, const POINT& pointMask, HBITMAP hbmMask  ) const throw()
	{
		return( PlgBlt( hDestDC, pPoints, rectSrc.left, rectSrc.top, rectSrc.right-
			rectSrc.left, rectSrc.bottom-rectSrc.top, hbmMask, pointMask.x, 
			pointMask.y ) );
	}

	inline BOOL Image::PlgBlt( HDC hDestDC, const POINT* pPoints, 
		HBITMAP hbmMask ) const throw()
	{
		return( PlgBlt( hDestDC, pPoints, 0, 0, m_nWidth, m_nHeight, hbmMask, 0, 
			0 ) );
	}

	inline void Image::ReleaseDC() const throw()
	{
		HBITMAP hBitmap;

		assert( m_hDC != nullptr );

		m_nDCRefCount--;
		if( m_nDCRefCount == 0 )
		{
			hBitmap = HBITMAP( ::SelectObject( m_hDC, m_hOldBitmap ) );
			assert( hBitmap == m_hBitmap );
			s_cache.ReleaseDC( m_hDC );
			m_hDC = nullptr;
		}
	}

	inline CLSID Image::FindCodecForExtension( const wchar_t* pszExtension, const Gdiplus::ImageCodecInfo* pCodecs, unsigned int nCodecs )
	{
#ifdef _UNICODE // libo 20101117 remove 只适用用于多字节编码
		LPCWSTR pszExtensionW( pszExtension );

		for( unsigned int iCodec = 0; iCodec < nCodecs; iCodec++ )
		{
			String strExtensions( pCodecs[iCodec].FilenameExtension );

			int iStart = 0;
			do
			{
				String strExtension;
				TCHAR* pszPlace = (TCHAR*)(strExtensions.c_str()+iStart);
				TCHAR* pszEnd = (TCHAR*)(strExtensions.c_str()+strExtensions.length());
				if( pszPlace < pszEnd )
				{
					int nIncluding = (int)wcsspn( pszPlace,	L";" );

					if( (pszPlace+nIncluding) < pszEnd )
					{
						pszPlace += nIncluding;
						int nExcluding = (int)wcsspn( pszPlace, L";" );

						int iFrom = iStart+nIncluding;
						int nUntil = nExcluding;
						iStart = iFrom+nUntil+1;

						strExtension = ( strExtensions.substr( iFrom, nUntil-iFrom ) );
					}
				}
				else
					iStart = -1;

				//String strExtension = ::PathFindExtensionW( strExtensions.Tokenize( L";", iStart ) );
				if( iStart != -1 )
				{
					if( strExtension.compare( pszExtensionW ) == 0 )
					{
						return( pCodecs[iCodec].Clsid );
					}
				}
			} while( iStart != -1 );
		}
#endif
		return( CLSID_NULL );

	}

	inline CLSID Image::FindCodecForFileType( REFGUID guidFileType, const Gdiplus::ImageCodecInfo* pCodecs, unsigned int nCodecs )
	{
		for( unsigned int iCodec = 0; iCodec < nCodecs; iCodec++ )
		{
			if( pCodecs[iCodec].FormatID == guidFileType )
			{
				return( pCodecs[iCodec].Clsid );
			}
		}

		return( CLSID_NULL );
	}

	inline HRESULT Image::Save( IStream* pStream, REFGUID guidFileType ) const throw()
	{
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		unsigned int nEncoders;
		unsigned int nBytes;
		Gdiplus::Status status;

		status = Gdiplus::GetImageEncodersSize( &nEncoders, &nBytes );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		//USES_ATL_SAFE_ALLOCA;
		Gdiplus::ImageCodecInfo* pEncoders = static_cast< Gdiplus::ImageCodecInfo* >( _alloca(nBytes) );

		if( pEncoders == nullptr )
			return E_OUTOFMEMORY;

		status = Gdiplus::GetImageEncoders( nEncoders, nBytes, pEncoders );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		CLSID clsidEncoder = FindCodecForFileType( guidFileType, pEncoders, nEncoders );
		if( clsidEncoder == CLSID_NULL )
		{
			return( E_FAIL );
		}

		if( m_bHasAlphaChannel )
		{
			assert( m_nBPP == 32 );
			Gdiplus::Bitmap bm( m_nWidth, m_nHeight, m_nPitch, PixelFormat32bppARGB, static_cast< BYTE* >( m_pBits ) );
			status = bm.Save( pStream, &clsidEncoder, nullptr );
			if( status != Gdiplus::Ok )
			{
				return( E_FAIL );
			}
		}
		else
		{
			Gdiplus::Bitmap bm( m_hBitmap, nullptr );
			status = bm.Save( pStream, &clsidEncoder, nullptr );
			if( status != Gdiplus::Ok )
			{
				return( E_FAIL );
			}
		}

		return( S_OK );
	}

	inline HRESULT Image::Save( const wchar_t* pszFileName, REFGUID guidFileType ) const throw()
	{
#ifdef _UNICODE // libo 20101117 remove 只适用用于多字节编码
		if( !InitGDIPlus() )
		{
			return( E_FAIL );
		}

		unsigned int nEncoders;
		unsigned int nBytes;
		Gdiplus::Status status;

		status = Gdiplus::GetImageEncodersSize( &nEncoders, &nBytes );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		//USES_CONVERSION_EX;
		Gdiplus::ImageCodecInfo* pEncoders = static_cast< Gdiplus::ImageCodecInfo* >( _alloca(nBytes) );

		if( pEncoders == nullptr )
			return E_OUTOFMEMORY;

		status = Gdiplus::GetImageEncoders( nEncoders, nBytes, pEncoders );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}

		CLSID clsidEncoder = CLSID_NULL;
		if( guidFileType == GUID_NULL )
		{
			// Determine clsid from extension
			clsidEncoder = FindCodecForExtension( ::PathFindExtension( pszFileName ), pEncoders, nEncoders );
		}
		else
		{
			// Determine clsid from file type
			clsidEncoder = FindCodecForFileType( guidFileType, pEncoders, nEncoders );
		}
		if( clsidEncoder == CLSID_NULL )
		{
			return( E_FAIL );
		}

		LPCWSTR pwszFileName =  pszFileName;
#ifndef _UNICODE
		if( pwszFileName == nullptr )
			return E_OUTOFMEMORY;
#endif // _UNICODE
		if (m_bHasAlphaChannel && m_pBits)
		{
			assert(m_nBPP == 32);
			// libo modified 20130121 PixelFormat32bppARGB->PixelFormat32bppPARGB（load时预乘过了，所以这里得加上P标识）
			// 使用PixelFormat32bppARGB保存时，alpha值不知道为什么总是和原始值不一样。
			Gdiplus::Bitmap bm( m_nWidth, m_nHeight, m_nPitch, PixelFormat32bppPARGB, static_cast< BYTE* >( m_pBits ) );
			status = bm.Save( pwszFileName, &clsidEncoder, nullptr );
			if (status != Gdiplus::Ok)
			{
				return( E_FAIL );
			}
		}
		else
		{
			Gdiplus::Bitmap bm( m_hBitmap, nullptr );
			status = bm.Save( pwszFileName, &clsidEncoder, nullptr );
			if (status != Gdiplus::Ok)
			{
				return( E_FAIL );
			}
		}

#endif
		return( S_OK );
	}

	inline void Image::SetColorTable( unsigned int iFirstColor, unsigned int nColors, 
		const RGBQUAD* prgbColors ) throw()
	{
		assert( m_hBitmap != nullptr );
		assert( IsDIBSection() );
		assert( IsIndexed() );

		GetDC();

		::SetDIBColorTable( m_hDC, iFirstColor, nColors, prgbColors );

		ReleaseDC();
	}

	inline void Image::SetPixel( int x, int y, COLORREF color ) throw()
	{
		assert( m_hBitmap != nullptr );
		assert( (x >= 0) && (x < m_nWidth) );
		assert( (y >= 0) && (y < m_nHeight) );

		GetDC();

		::SetPixel( m_hDC, x, y, color );

		ReleaseDC();
	}

	inline void Image::SetPixelIndexed( int x, int y, int iIndex ) throw()
	{
		SetPixel( x, y, PALETTEINDEX( iIndex ) );
	}

	inline void Image::SetPixelRGB( int x, int y, BYTE r, BYTE g, BYTE b ) throw()
	{
		SetPixel( x, y, RGB( r, g, b ) );
	}

	inline LONG Image::SetTransparentColor( LONG iTransparentColor ) throw()
	{
		LONG iOldTransparentColor;

		assert( m_hBitmap != nullptr );
		assert( (m_nBPP == 4) || (m_nBPP == 8) );
		assert( iTransparentColor < GetMaxColorTableEntries() );
		assert( iTransparentColor >= -1 );

		iOldTransparentColor = m_iTransparentColor;
		m_iTransparentColor = iTransparentColor;

		return( iOldTransparentColor );
	}

	inline COLORREF Image::SetTransparentColor( COLORREF colorTransparent ) throw()
	{
		COLORREF oldValue = m_colorTransparent;

		m_iTransparentColor = CLR_USE_COLOR_DIRECT;
		m_colorTransparent = colorTransparent;	

		return oldValue;
	}

	inline BOOL Image::StretchBlt( HDC hDestDC, int xDest, int yDest, 
		int nDestWidth, int nDestHeight, DWORD dwROP ) const throw()
	{
		return( StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, 
			m_nWidth, m_nHeight, dwROP ) );
	}

	inline BOOL Image::StretchBlt( HDC hDestDC, const RECT& rectDest, 
		DWORD dwROP ) const throw()
	{
		return( StretchBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, 0, 0, m_nWidth, m_nHeight, 
			dwROP ) );
	}

	inline BOOL Image::StretchBlt( HDC hDestDC, int xDest, int yDest, 
		int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, 
		int nSrcHeight, DWORD dwROP ) const throw()
	{
		BOOL bResult;

		assert( m_hBitmap != nullptr );
		assert( hDestDC != nullptr );

		GetDC();

		bResult = ::StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC,
			xSrc, ySrc, nSrcWidth, nSrcHeight, dwROP );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL Image::StretchBlt( HDC hDestDC, const RECT& rectDest, 
		const RECT& rectSrc, DWORD dwROP ) const throw()
	{
		return( StretchBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
			rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top, 
			rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, dwROP ) );
	}

#if WINVER >= 0x0500
	inline BOOL Image::TransparentBlt( HDC hDestDC, int xDest, int yDest, 
		int nDestWidth, int nDestHeight, unsigned int crTransparent ) const throw()
	{
		return( TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 
			0, m_nWidth, m_nHeight, crTransparent ) );
	}

	inline BOOL Image::TransparentBlt( HDC hDestDC, const RECT& rectDest, 
		unsigned int crTransparent ) const throw()
	{
		return( TransparentBlt( hDestDC, rectDest.left, rectDest.top, 
			rectDest.right-rectDest.left, rectDest.bottom-rectDest.top, 
			crTransparent ) );
	}

	inline BOOL Image::TransparentBlt( HDC hDestDC, int xDest, int yDest, 
		int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, 
		int nSrcHeight, unsigned int crTransparent ) const throw()
	{
		BOOL bResult;

		assert( m_hBitmap != nullptr );
		assert( hDestDC != nullptr );

		GetDC();

		if( crTransparent == CLR_INVALID )
		{
			crTransparent = GetTransparentRGB();
		}
		else if( crTransparent == CLR_USE_COLOR_DIRECT )
		{
			crTransparent = m_colorTransparent;
		}

		bResult = ::TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight,
			m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, crTransparent );

		ReleaseDC();

		return( bResult );
	}

	inline BOOL Image::TransparentBlt( HDC hDestDC, const RECT& rectDest, 
		const RECT& rectSrc, unsigned int crTransparent ) const throw()
	{
		return( TransparentBlt( hDestDC, rectDest.left, rectDest.top, 
			rectDest.right-rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, 
			rectSrc.top, rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, 
			crTransparent ) );
	}
#endif  // WINVER >= 0x0500

	inline BOOL Image::IsTransparencySupported() throw()
	{
#if WINVER >= 0x0500
		//return( _AtlBaseModule.m_bNT5orWin98 );
		return TRUE;
#else  // WINVER < 0x0500
		return( FALSE );
#endif  // WINVER >= 0x0500
	}

	inline void Image::UpdateBitmapInfo( DIBOrientation eOrientation )
	{
		DIBSECTION dibsection;
		int nBytes;

        // http://support.microsoft.com/kb/186586/zh-cn
        // When you use GetObject() to obtain information about a DIB section, GetObject() 
        // always returns a DIBSECTION containing a BITMAPINFOHEADER with a positive height 
        // value, regardless of the sign of the height it was created with. 
        // If you need to track the sign of the height in an application, you will need to
        // do this programmatically. 
        // 当使用 GetObject() 以获得有关 DIB 部分信息时，GetObject() 将始终返回包含正高度值，
        // 而不考虑与创建它的高度的符号 BITMAPINFOHEADER DIBSECTION。如果您需要跟踪应用程序
        // 中的高度的符号，您需要以编程方式执行此操作。

		nBytes = ::GetObject( m_hBitmap, sizeof( DIBSECTION ), &dibsection );
		if( nBytes == sizeof( DIBSECTION ) )
		{
			m_bIsDIBSection = true;
			m_nWidth = dibsection.dsBmih.biWidth;
			m_nHeight = abs( dibsection.dsBmih.biHeight );
			m_nBPP = dibsection.dsBmih.biBitCount;
			m_nPitch = ComputePitch( m_nWidth, m_nBPP );
			m_pBits = dibsection.dsBm.bmBits;
			if( eOrientation == DIBOR_DEFAULT )
			{
				eOrientation = (dibsection.dsBmih.biHeight > 0) ? DIBOR_BOTTOMUP : DIBOR_TOPDOWN;
			}
			if( eOrientation == DIBOR_BOTTOMUP )
			{
				m_pBits = LPBYTE( m_pBits )+((m_nHeight-1)*m_nPitch);
				m_nPitch = -m_nPitch;
			}
		}
		else
		{
			// Non-DIBSection 
			// TODO: 从DDB Attach到的图片绘制有问题，先不管了，直接转成DIB
#if 0
			assert( nBytes == sizeof( BITMAP ) );
			m_bIsDIBSection = false;
			m_nWidth = dibsection.dsBm.bmWidth;
			m_nHeight = dibsection.dsBm.bmHeight;
			m_nBPP = dibsection.dsBm.bmBitsPixel;
			m_nPitch = 0;
			m_pBits = 0;
#else

			HBITMAP hBitmap = m_hBitmap;
			m_hBitmap = nullptr;

			Create(dibsection.dsBm.bmWidth, dibsection.dsBm.bmHeight, 32, createAlphaChannel);

			HDC hDC = GetDC();
			HDC hCompDC = CreateCompatibleDC(nullptr);
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompDC, hBitmap);

			BOOL bRet = ::BitBlt(hDC, 0, 0, dibsection.dsBm.bmWidth, dibsection.dsBm.bmHeight, hCompDC, 0, 0, SRCCOPY);
			
			SelectObject(hCompDC, hOldBitmap);
			DeleteDC(hCompDC);
			ReleaseDC();

			FixGDIAlphaChannel();
#endif

		}
		m_iTransparentColor = -1;
		m_bHasAlphaChannel = (m_nBPP==32);
	}

	inline void Image::GenerateHalftonePalette( LPRGBQUAD prgbPalette )
	{
		int r;
		int g;
		int b;
		int gray;
		LPRGBQUAD prgbEntry;

		prgbEntry = prgbPalette;
		for( r = 0; r < 6; r++ )
		{
			for( g = 0; g < 6; g++ )
			{
				for( b = 0; b < 6; b++ )
				{
					prgbEntry->rgbBlue = BYTE( b*255/5 );
					prgbEntry->rgbGreen = BYTE( g*255/5 );
					prgbEntry->rgbRed = BYTE( r*255/5 );
					prgbEntry->rgbReserved = 0;

					prgbEntry++;
				}
			}
		}

		for( gray = 0; gray < 20; gray++ )
		{
			prgbEntry->rgbBlue = BYTE( gray*255/20 );
			prgbEntry->rgbGreen = BYTE( gray*255/20 );
			prgbEntry->rgbRed = BYTE( gray*255/20 );
			prgbEntry->rgbReserved = 0;

			prgbEntry++;
		}
	}

	inline COLORREF Image::GetTransparentRGB() const
	{
		if( m_iTransparentColor == CLR_USE_COLOR_DIRECT )
			return m_colorTransparent;

		RGBQUAD rgb;

		bool bHaveDC = (m_hDC != nullptr);   // libo add 用于外部调用获取颜色值
		if( !bHaveDC )
			GetDC();

		assert( m_hDC != nullptr );  // Must have a DC
		assert( m_iTransparentColor != -1 );

		::GetDIBColorTable( m_hDC, m_iTransparentColor, 1, &rgb );

		if( !bHaveDC )
			ReleaseDC();

		return( RGB( rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue ) );
	}

	// libo 20120321 add
	//	创建pImage的一张去色图片
	//
	// modify 20120927 将浮点运算修改为整数运算 
	//	参考:http://blog.csdn.net/housisong/article/details/3884368 图形图像处理－之－彩色转化到灰度的速度优化. 
	//	HouSisong@GMail.com  2009.02.08
	// 
	inline bool Image::CopyGrayImageFrom( const Image* pImage )
	{
		if( nullptr == pImage || (pImage->GetBPP()!= 32 && pImage->GetBPP()!= 24))  // 暂时不支持其它格式的图片，不了解其格式
			return false;
	
		this->Destroy();
		this->Create(pImage->GetWidth(), pImage->GetHeight(), pImage->GetBPP(), (pImage->HasAlphaChannel()==true)?createAlphaChannel:0 );
		if (this->IsNull())
			return false;

		// Gray = R*0.299 + G*0.587  + B*0.114;
		//灰度转换系数   
		const double gray_r_coeff = 0.299;  
		const double gray_g_coeff = 0.587;  
//		const double gray_b_coeff = 0.114;

		const long bit=16;  
		const int gray_r_coeff_int = (int)( gray_r_coeff*(1<<bit)+0.4999999 );  
		const int gray_g_coeff_int = (int)( gray_g_coeff*(1<<bit)+0.4999999 );  
		const int gray_b_coeff_int = (1<<bit)-gray_r_coeff_int-gray_g_coeff_int; 

		const BYTE* pImageBits = (const BYTE*)pImage->GetBits();
		BYTE* pNewImageBits = (BYTE*)m_pBits;
		int   bytesperpx    = m_nBPP>>3;
		int   bytesperrow   = bytesperpx*m_nWidth;  /*abs(m_nPitch);*/;   // 注：由于位于一行要求是4的位置，可能导致当宽度为奇数数，后面会补充无用的位。因此这里不能直接用m_nPitch

		for (int row = 0; row < m_nHeight; row ++ )
		{
			for (int i = 0; i < bytesperrow; i += bytesperpx)
			{
				//int rgb = (int)(pImageBits[i]*0.11 + pImageBits[i+1]*0.59 + pImageBits[i+2]*0.3);  // 去色算法，可优化非浮点运算
				int rgb = (pImageBits[i]*gray_b_coeff_int   + 
					       pImageBits[i+1]*gray_g_coeff_int + 
						   pImageBits[i+2]*gray_r_coeff_int) >> bit;  
				
				pNewImageBits[i] = pNewImageBits[i+1] = pNewImageBits[i+2] = (byte)rgb;

				if (m_bHasAlphaChannel)
					pNewImageBits[i+3] = pImageBits[i+3];
			}

			pNewImageBits += m_nPitch;
			pImageBits += m_nPitch;
		}

		return true;
	}
	inline bool Image::DrawGray(HDC hDC, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc)
	{
		Image  grayImage;
		if (false == this->CopyGrayImageFrom(&grayImage))
			return false;

		BOOL bRet = grayImage.Draw(hDC, xDest, yDest, wDest, hDest, xSrc, ySrc, wDest, hDest);
		grayImage.Destroy();
		return bRet?true:false;
	}

    bool ChangeColorHLS(BYTE& R, BYTE& G, BYTE& B, short h, short l , short s, int nFlag);
    bool ChangeColorHue(BYTE& R, BYTE& G, BYTE& B, short h, bool bOffsetOrReplace);
    bool ChangeColorSaturation(BYTE& R, BYTE& G, BYTE& B, short s, float dS);
    void ChangeColorLuminance(BYTE& R, BYTE& G, BYTE& B, short l, float dL);
    bool ChangeColorHueAndSaturation(BYTE& R, BYTE& G, BYTE& B, short h, bool bOffsetOrReplace, short s, float dS);

};  // namespace 


