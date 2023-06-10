#include "stdafx.h"
#include "skindatasource.h"
#include "Zip/zipdatasource.h"
#include "File/filedatasource.h"
#include "Zip/bytebufferreader.h"
#include "File/filebufferreader.h"
#include <gdiplus.h>

namespace UI
{

void  CreateDataSourceInstance(SKIN_PACKET_TYPE e, SkinDataSource** pp)
{
    if (nullptr == pp)
        return;

    SkinDataSource* p = nullptr;
    switch (e)
    {
    case SKIN_PACKET_TYPE_DIR:
        {
			p = FileDataSource::Create();
        }
        break;;

    case SKIN_PACKET_TYPE_ZIP:
	case SKIN_PACKET_TYPE_RESZIP:
        {
			p = ZipDataSource::Create();
        }
        break;
    }
    
    *pp = p;
}

void  CreateStreamBuffer(STREAM_TYPE e, IStreamBufferReader** pp)
{
    if (nullptr == pp)
        return;

    IStreamBufferReader* p = nullptr;
    switch (e)
    {
    case STREAM_TYPE_FILE:
        {
			p = FileBufferReader::Create();
        }
        break;;

    case STREAM_TYPE_BYTE:
        {
			p = ByteBufferReader::Create();
        }
        break;
    }

    *pp = p;
}

ISkinDataSource::ISkinDataSource(SkinDataSource* p)
{
	m_pImpl = p;
}

LPCTSTR  ISkinDataSource::GetPath()
{
	return m_pImpl->GetPath();
}
SKIN_PACKET_TYPE  ISkinDataSource::GetType()
{
	return m_pImpl->GetType();
}
bool  ISkinDataSource::Load_Image(LPCTSTR szPath, ImageWrap* pImage)
{
	return m_pImpl->Load_Image(szPath, pImage);
}
bool  ISkinDataSource::Load_GdiplusImage(LPCTSTR szPath, GdiplusBitmapLoadWrap* pImage)
{
	return m_pImpl->Load_GdiplusImage(szPath, pImage);
}

bool  ISkinDataSource::Load_StreamBuffer(LPCTSTR szPath, IStreamBufferReader** pp)
{
	return m_pImpl->Load_StreamBuffer(szPath, pp);
}

GdiplusBitmapLoadWrap::GdiplusBitmapLoadWrap()
{
    m_pBitmap = nullptr;
}
GdiplusBitmapLoadWrap::~GdiplusBitmapLoadWrap()
{
    Destroy();
}
Gdiplus::Bitmap*  GdiplusBitmapLoadWrap::GetBitmap()
{
    return m_pBitmap;
}

void  GdiplusBitmapLoadWrap::Destroy()
{
    SAFE_DELETE(m_pBitmap);
}
bool  GdiplusBitmapLoadWrap::LoadFromFile(LPCTSTR szPath)
{
    SAFE_DELETE(m_pBitmap);
    m_pBitmap = Gdiplus::Bitmap::FromFile(szPath);

    if (m_pBitmap->GetLastStatus() != Gdiplus::Ok)
    {
        SAFE_DELETE(m_pBitmap);
        return false;
    }
    return true;
}
bool  GdiplusBitmapLoadWrap::LoadFromByte(LPBYTE pByte, int nSize)
{
    SAFE_DELETE(m_pBitmap);

    IStream* pStream = nullptr;
    HGLOBAL  hNew    = ::GlobalAlloc(GHND, nSize);
    LPBYTE   lpByte  = (LPBYTE)::GlobalLock(hNew);
    ::memcpy(lpByte, pByte, nSize);
    ::GlobalUnlock(hNew);

    HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
    if (ht != S_OK)
    {
        GlobalFree(hNew);
        return false;
    }
    m_pBitmap = Gdiplus::Bitmap::FromStream(pStream);
    pStream->Release();  
    hNew = nullptr;

    if (m_pBitmap->GetLastStatus() != Gdiplus::Ok)
    {
        SAFE_DELETE(m_pBitmap);
        return false;
    }
    return true;
}

}