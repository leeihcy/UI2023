#include "stdafx.h"
#include "filedatasource.h"
#include "Inc\Util\iimage.h"
#include "Src\Atl\image.h"
#include "Src\SkinParse\datasource\File\filebufferreader.h"
#include "Src\SkinParse\xml\xmlwrap.h"

namespace UI
{

FileDataSource::FileDataSource():m_ISkinDataSource(this)
{

}
FileDataSource*  FileDataSource::Create()
{
	return new FileDataSource();
}
void  FileDataSource::Release()
{
	delete this;
}
ISkinDataSource*  FileDataSource::GetISkinDataSource()
{
	return &m_ISkinDataSource;
}

void  FileDataSource::SetPath(LPCTSTR szPath)
{
    if (szPath)
        m_strPath = szPath;
    else
        m_strPath.clear();

	int nLength = (int)m_strPath.length();
	if (nLength > 1)
	{
		if (m_strPath[nLength-1] != _T('\\'))
		{
			m_strPath.append(_T("\\"));
		}
	}
}

LPCTSTR  FileDataSource::GetPath()
{
    return m_strPath.c_str();
}

void FileDataSource::SetData(byte* data, int size)
{
	UIASSERT(0);
}

SKIN_PACKET_TYPE  FileDataSource::GetType()
{
    return SKIN_PACKET_TYPE_DIR;
}
bool FileDataSource::Load_UIDocument(UIDocument* pDocument, LPCTSTR szPath)
{
    if (nullptr == pDocument || nullptr == szPath)
        return false;

    String strTemp = m_strPath;
    strTemp.append(szPath);

    return pDocument->LoadFile(strTemp.c_str());
}

bool  FileDataSource::Load_RenderBitmap(IRenderBitmap* pBitmap, LPCTSTR szPath, RENDER_BITMAP_LOAD_FLAG e)
{
    if (nullptr == pBitmap || nullptr == szPath)
        return false;

    String strTemp = m_strPath;
    strTemp.append(szPath);

    return pBitmap->LoadFromFile(strTemp.c_str(), e);
}

bool  FileDataSource::Load_Image(LPCTSTR szPath, ImageWrap* pImage)
{
    if (nullptr == pImage || nullptr == szPath)
        return false;

    String strTemp = m_strPath;
    strTemp.append(szPath);

    pImage->GetImpl()->Load(strTemp.c_str());
    return pImage->IsNull()? false:true;
}

bool  FileDataSource::Load_GdiplusImage(LPCTSTR szPath, GdiplusBitmapLoadWrap* pImage)
{
    if (nullptr == pImage || nullptr == szPath)
        return false;

    String strTemp = m_strPath;
    strTemp.append(szPath);

    return pImage->LoadFromFile(strTemp.c_str());
}

bool  FileDataSource::Load_StreamBuffer(LPCTSTR szPath, IStreamBufferReader** pp)
{
    if (nullptr == pp || nullptr == szPath)
        return false;

    String strTemp = m_strPath;
    strTemp.append(szPath);

    FileBufferReader*  pBuffer = new FileBufferReader;
    if (!pBuffer->load(strTemp.c_str()))
    {
        delete pBuffer;
        return false;
    }

    *pp = pBuffer;
    return true;
}

bool FileDataSource::FileExist(LPCTSTR szPath)
{
	String strTemp = m_strPath;
	strTemp.append(szPath);

	return PathFileExists(strTemp.c_str()) ? true : false;
}

// 
// bool  FileDataSource::CalcFilePath(LPCTSTR szPath, ISkinRes* pSkinRes, __out String& strLastPath)
// {
//     if (nullptr == szPath || nullptr == pSkinRes)
//         return false;
// 
//     if (_tcslen(szPath) >= MAX_PATH)
//         return false;
// 
//     // 1. 直接就是绝对路径
//     if (Util::IsFullPath(szPath))
//     {
//         if (!::PathFileExists(szPath))
//             return false;
// 
//         strLastPath = szPath;
//         return true;
//     }
// 
//     TCHAR  szFullPath[MAX_PATH] = _T("");
// 
//     // 相对于皮肤包路径
//     LPCTSTR szSkinDir = pSkinRes->GetPath();
//     if (Util::CalcFullPathByRelative(szSkinDir, szPath, szFullPath))
//     {
//         if (PathFileExists(szFullPath))
//         {
//             strLastPath = szFullPath;
//             return true;
//         }
//     }
// 
//     // 相对于配置文件路径
//     IUIDocument*  pDoc = nullptr;
//     pElement->GetDocument(&pDoc);
// 
//     if (nullptr == pDoc)
//         return false;
// 
//     TCHAR  szFileDir[MAX_PATH] = _T("");
//     if (FALSE == Util::GetPathDir(pDoc->GetPath(), szFileDir))
//     {
//         SAFE_RELEASE(pDoc);
//         return false;
//     }
//     SAFE_RELEASE(pDoc);
// 
//     if (Util::CalcFullPathByRelative(szFileDir, szPath, szFullPath))
//     {
//         if (PathFileExists(szFullPath))
//         {
//             strLastPath = szFullPath;
//             return true;
//         }
//     }
// 
//     return false;
// }

}