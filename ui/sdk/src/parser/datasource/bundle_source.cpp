#include "bundle_source.h"
#include "zip/zipdatasource.h"
#include "file/filedatasource.h"
#include "file/filebufferreader.h"
#include "include/interface/ibundlesource.h"
#include "sdk/include/macro/helper.h"
#include <memory>

namespace ui {

std::unique_ptr<BundleSource> CreateBundleSource(eBundleFormat e) {
  switch (e) {
  case eBundleFormat::Directory: {
    return std::make_unique<FileDataSource>();
  } break;

  case eBundleFormat::Zip:
  case eBundleFormat::ModuleResource: {
    return std::make_unique<ZipDataSource>();
  } break;
  }
}

void CreateStreamBuffer(eStreamType e, IStreamBufferReader **pp) {
  if (nullptr == pp)
    return;

  IStreamBufferReader *p = nullptr;
  switch (e) {
  case eStreamType::File: {
    p = FileBufferReader::Create();
  } break;

  case eStreamType::Byte: {
#if 0
			p = ByteBufferReader::Create();
#else
    UIASSERT(false);
#endif
  } break;
  }

  *pp = p;
}

IBundleSource::IBundleSource(BundleSource *p) { m_pImpl = p; }

const char *IBundleSource::GetPath() { return m_pImpl->GetPath(); }
eBundleFormat IBundleSource::GetType() { return m_pImpl->GetType(); }

bool IBundleSource::LoadBuffer(const char *szPath,
                           slot<void(const char *, unsigned int)> &&callback) {
  return m_pImpl->LoadBuffer(szPath, std::move(callback));
}

#if 0 // defined(OS_WIN)
bool  IBundleSource::Load_Image(const char* szPath, ImageWrap* pImage)
{
	return m_pImpl->Load_Image(szPath, pImage);
}
bool  IBundleSource::Load_GdiplusImage(const char* szPath, GdiplusBitmapLoadWrap* pImage)
{
	return m_pImpl->Load_GdiplusImage(szPath, pImage);
}

bool  IBundleSource::Load_StreamBuffer(const char* szPath, IStreamBufferReader** pp)
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
bool  GdiplusBitmapLoadWrap::LoadFromFile(const char* szPath)
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

    long ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
    if (ht != 0)
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
#endif
} // namespace ui