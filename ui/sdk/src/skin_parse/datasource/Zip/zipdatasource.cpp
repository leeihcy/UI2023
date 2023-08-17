#include "include/inc.h"
#include "zipdatasource.h"
#include "Inc\Util\iimage.h"
#include "src/Atl\image.h"
#include "src/skin_parse/datasource/Zip\bytebufferreader.h"
#include "src/SkinParse\xml\xmlwrap.h"

namespace ui
{

ZipDataSource::ZipDataSource() : m_ISkinDataSource(this)
{
    m_hZip = nullptr;
}
ZipDataSource::~ZipDataSource()
{
    if (m_hZip)
    {
        CloseZip(m_hZip);
        m_hZip = nullptr;
    }
}

ZipDataSource*  ZipDataSource::Create()
{
	return new ZipDataSource();
}
void  ZipDataSource::Release()
{
	delete this;
}
ISkinDataSource*  ZipDataSource::GetISkinDataSource()
{
	return &m_ISkinDataSource;
}

SKIN_PACKET_TYPE  ZipDataSource::GetType()
{
    return SKIN_PACKET_TYPE_ZIP;
}

void  ZipDataSource::SetPath(const wchar_t* szPath)
{
	if (szPath)
		m_strPath = szPath;
	else
		m_strPath.clear();
}
const wchar_t*  ZipDataSource::GetPath()
{
	return m_strPath.c_str();
}

void  ZipDataSource::SetData(byte* data, int size)
{
	UIASSERT(!m_hZip);

	m_hZip = OpenZip(data, size, nullptr);
	if (nullptr == m_hZip)
	{
		UI_LOG_ERROR(_T("OpenZip Failed. file=%s"), m_strPath.c_str());
		return;
	}

	return;
}

bool  ZipDataSource::Init()
{
    if (nullptr == m_hZip)
    {
        m_hZip = OpenZip(m_strPath.c_str(), nullptr);
        if (nullptr == m_hZip)
        {
            UI_LOG_ERROR(_T("OpenZip Failed. file=%s"), m_strPath.c_str());
			return false;
        }
    }
	return true;
}

bool  ZipDataSource::Load_UIDocument(UIDocument* pDocument, const wchar_t* szPath)
{
    if (!m_hZip && !Init())
	{
		return false;
	}

    ZIPENTRY ze;
    int index = 0; 

    wchar_t szPath2[MAX_PATH];
    TranslatePath(szPath, szPath2);

    FindZipItem(m_hZip, szPath2, true, &index, &ze);
    if (-1 == index)
    {
        UI_LOG_ERROR(_T("FindZipItem Failed. path=%s"), szPath);
        return false;
    }

	int nSize = ze.unc_size;
    byte* szbuf = new byte[nSize];  // 最后一个用0填充，标明字符串结束
    memset(szbuf, 0, nSize);
    UnzipItem(m_hZip, index, szbuf, ze.unc_size);

    bool bRet = pDocument->LoadData(szbuf, nSize);    
    SAFE_ARRAY_DELETE(szbuf);
    return bRet;
}

void  ZipDataSource::TranslatePath(const wchar_t* szOrignPath, wchar_t* szLastPath)
{
    const wchar_t* p = szOrignPath;
    wchar_t* p2 = szLastPath;

    // 跳过 .\xxx 表示的当前目录
    if (wcslen(szOrignPath) > 2 &&
        szOrignPath[0] == _T('.') && 
        (szOrignPath[1] == _T('/') || szOrignPath[1] == _T('\\')))
    {
        p += 2;
    }

    while (*p)
    {
        if (*p == '\\')
            *p2 = '/';
        else
            *p2 = *p;

        p++;
        p2++;
    }
    *p2 = _T('\0');
}

// 注：zip内部的路径符号是/，而不是\ 
bool  ZipDataSource::Load_RenderBitmap(IRenderBitmap* pBitmap, const wchar_t* szPath, RENDER_BITMAP_LOAD_FLAG e)
{
    if (!m_hZip || !Init())
        return false;

    if (nullptr == pBitmap || nullptr == szPath)
        return false;

    ZIPENTRY ze;
    int index = 0; 

    // 替换路径符号
    wchar_t szPath2[MAX_PATH] = _T("");
    TranslatePath(szPath, szPath2);
	
    FindZipItem(m_hZip, szPath2, true, &index, &ze);
    if (-1 == index)
    {
        UI_LOG_ERROR(_T("FindZipItem Failed. path=%s"), szPath);
        return false;
    }

    char* szbuf = new char[ze.unc_size];
    memset(szbuf, 0, ze.unc_size);
    UnzipItem(m_hZip, index, szbuf, ze.unc_size);

    bool bRet = pBitmap->LoadFromData((byte*)szbuf, ze.unc_size, e);
    delete[] szbuf;

    return bRet;
}

bool  ZipDataSource::Load_Image(const wchar_t* szPath, ImageWrap* pImage)
{
    if (!m_hZip || !Init())
        return false;

    if (nullptr == pImage || nullptr == szPath)
        return false;

    ZIPENTRY ze;
    int index = 0; 

    // 替换路径符号
    wchar_t szPath2[MAX_PATH] = _T("");
    TranslatePath(szPath, szPath2);

    FindZipItem(m_hZip, szPath2, true, &index, &ze);
    if (-1 == index)
    {
        UI_LOG_ERROR(_T("FindZipItem Failed. path=%s"), szPath);
        return false;
    }

    char* szbuf = new char[ze.unc_size];
    memset(szbuf, 0, ze.unc_size);
    UnzipItem(m_hZip, index, szbuf, ze.unc_size);

    pImage->GetImpl()->LoadFromData((byte*)szbuf, ze.unc_size);
    delete[] szbuf;

    return pImage->IsNull()? false:true;
}

bool  ZipDataSource::Load_GdiplusImage(const wchar_t* szPath, GdiplusBitmapLoadWrap* pImage)
{
    if (!m_hZip || !Init())
        return false;

    if (nullptr == pImage || nullptr == szPath)
        return false;

    ZIPENTRY ze;
    int index = 0; 

    // 替换路径符号
    wchar_t szPath2[MAX_PATH] = _T("");
    TranslatePath(szPath, szPath2);

    FindZipItem(m_hZip, szPath2, true, &index, &ze);
    if (-1 == index)
    {
        UI_LOG_ERROR(_T("FindZipItem Failed. path=%s"), szPath);
        return false;
    }

    char* szbuf = new char[ze.unc_size];
    memset(szbuf, 0, ze.unc_size);
    UnzipItem(m_hZip, index, szbuf, ze.unc_size);

    bool bRet = pImage->LoadFromByte((byte*)szbuf, ze.unc_size);
    delete[] szbuf;

    return bRet;
}

bool  ZipDataSource::Load_StreamBuffer(const wchar_t* szPath, IStreamBufferReader** pp)
{
    if (!m_hZip || !Init())
        return false;

    if (nullptr == szPath || nullptr == pp)
        return false;

    ZIPENTRY ze;
    int index = 0; 

    // 替换路径符号
    wchar_t szPath2[MAX_PATH] = _T("");
    TranslatePath(szPath, szPath2);

    FindZipItem(m_hZip, szPath2, true, &index, &ze);
    if (-1 == index)
    {
        UI_LOG_ERROR(_T("FindZipItem Failed. path=%s"), szPath);
        return false;
    }

    char* szbuf = new char[ze.unc_size];
    memset(szbuf, 0, ze.unc_size);
    UnzipItem(m_hZip, index, szbuf, ze.unc_size);

    ByteBufferReader* pBuffer = new ByteBufferReader;
    pBuffer->load((const byte*)szbuf, ze.unc_size, true);
    *pp = pBuffer;

    return true;
}

bool ZipDataSource::FileExist(const wchar_t* szPath)
{
	ZIPENTRY ze;
	int index = 0;

	// 替换路径符号
	wchar_t szPath2[MAX_PATH] = _T("");
	TranslatePath(szPath, szPath2);

	FindZipItem(m_hZip, szPath2, true, &index, &ze);

	return (-1 == index) ? false : true;
}

}
