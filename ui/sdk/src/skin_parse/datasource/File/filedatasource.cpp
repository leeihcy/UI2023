#include "filedatasource.h"
// #include "Inc\Util\iimage.h"
// #include "src/Atl\image.h"
#include "include/util/path.h"
#include "sdk/include/macro/helper.h"
#include "src/skin_parse/datasource/File/filebufferreader.h"
#include "src/skin_parse/xml/xmlwrap.h"
#include "src/util/util.h"
#include <fstream>
#include <ios>
#include <vector>

namespace ui {

FileDataSource::FileDataSource() : m_ISkinDataSource(this) {}
FileDataSource *FileDataSource::Create() { return new FileDataSource(); }
void FileDataSource::Release() { delete this; }
ISkinDataSource *FileDataSource::GetISkinDataSource() {
  return &m_ISkinDataSource;
}

void FileDataSource::SetPath(const char *szPath) {
  if (szPath)
    m_strPath = szPath;
  else
    m_strPath.clear();

  int nLength = (int)m_strPath.length();
  if (nLength > 1) {
    if (m_strPath[nLength - 1] != Path::SEPARATOR) {
      m_strPath.push_back(Path::SEPARATOR);
    }
  }
}

const char *FileDataSource::GetPath() { return m_strPath.c_str(); }

void FileDataSource::SetData(byte *data, int size) { UIASSERT(0); }

SKIN_PACKET_TYPE FileDataSource::GetType() { return SKIN_PACKET_TYPE_DIR; }

bool FileDataSource::Load(const char* szPath, slot<void(const char*)>&& callback) {
 if (!szPath)
    return false;

  std::string strTemp = m_strPath;
  strTemp.append(szPath);
 
  if (!util::Path_FileExists(strTemp.c_str())) {
    return false;
  }

  std::fstream f(strTemp.c_str(), std::ios_base::in|std::ios_base::binary);
  if (!f) {
    return false;
  }

  f.seekg(0, std::ios::end);
  size_t size = f.tellg();

  std::vector<char> buf;
  buf.reserve(size);
  f.seekg(0, std::ios::beg);

  f.read(buf.data(), size);
  callback.emit(buf.data());
  return true;
}

bool FileDataSource::Load_UIDocument(UIDocument *pDocument,
                                     const char *szPath) {
  if (nullptr == pDocument || nullptr == szPath)
    return false;

  std::string strTemp = m_strPath;
  strTemp.append(szPath);

  return pDocument->LoadFile(strTemp.c_str());
}

bool FileDataSource::Load_RenderBitmap(IRenderBitmap *pBitmap,
                                       const char *szPath,
                                       RENDER_BITMAP_LOAD_FLAG e) {
  if (nullptr == pBitmap || nullptr == szPath)
    return false;

  std::string strTemp = m_strPath;
  strTemp.append(szPath);

  return pBitmap->LoadFromFile(strTemp.c_str(), e);
}

#if 0 // defined(OS_WIN)
bool  FileDataSource::Load_Image(const char* szPath, ImageWrap* pImage)
{
    if (nullptr == pImage || nullptr == szPath)
        return false;

    std::string strTemp = m_strPath;
    strTemp.append(szPath);

    pImage->GetImpl()->Load(strTemp.c_str());
    return pImage->IsNull()? false:true;
}

bool  FileDataSource::Load_GdiplusImage(const char* szPath, GdiplusBitmapLoadWrap* pImage)
{
    if (nullptr == pImage || nullptr == szPath)
        return false;

    std::string strTemp = m_strPath;
    strTemp.append(szPath);

    return pImage->LoadFromFile(strTemp.c_str());
}

bool  FileDataSource::Load_StreamBuffer(const char* szPath, IStreamBufferReader** pp)
{
    if (nullptr == pp || nullptr == szPath)
        return false;

    std::string strTemp = m_strPath;
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
#endif
bool FileDataSource::FileExist(const char *szPath) {
  std::string strTemp = m_strPath;
  strTemp.append(szPath);

  return ui::util::Path_FileExists(strTemp.c_str()) ? true : false;
}

//
// bool  FileDataSource::CalcFilePath(const char* szPath, IResource* pSkinRes,
// std::string& strLastPath)
// {
//     if (nullptr == szPath || nullptr == pSkinRes)
//         return false;
//
//     if (strlen(szPath) >= MAX_PATH)
//         return false;
//
//     // 1. 直接就是绝对路径
//     if (util::IsFullPath(szPath))
//     {
//         if (!::PathFileExists(szPath))
//             return false;
//
//         strLastPath = szPath;
//         return true;
//     }
//
//     char  szFullPath[MAX_PATH] = _T("");
//
//     // 相对于皮肤包路径
//     const char* szSkinDir = pSkinRes->GetPath();
//     if (util::CalcFullPathByRelative(szSkinDir, szPath, szFullPath))
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
//     char  szFileDir[MAX_PATH] = _T("");
//     if (false == util::GetPathDir(pDoc->GetPath(), szFileDir))
//     {
//         SAFE_RELEASE(pDoc);
//         return false;
//     }
//     SAFE_RELEASE(pDoc);
//
//     if (util::CalcFullPathByRelative(szFileDir, szPath, szFullPath))
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

} // namespace ui