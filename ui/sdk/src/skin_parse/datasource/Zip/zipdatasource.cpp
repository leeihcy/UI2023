#include "zipdatasource.h"
#include "include/inc.h"
#include "include/macro/helper.h"
#include "include/util/log.h"
#include "src/skin_parse/datasource/zip/bytebufferreader.h"
#include "src/skin_parse/xml/xmlwrap.h"
#include <vector>
#define MAX_PATH 256

namespace ui {

ZipDataSource::ZipDataSource()
    : m_ISkinDataSource(static_cast<SkinDataSource *>(this)) {
  m_unzip = 0;
}

ZipDataSource::~ZipDataSource() {
  if (m_unzip) {
    unzClose(m_unzip);
    m_unzip = nullptr;
  }
}

ZipDataSource *ZipDataSource::Create() { return new ZipDataSource(); }
void ZipDataSource::Release() { delete this; }
ISkinDataSource *ZipDataSource::GetISkinDataSource() {
  return &m_ISkinDataSource;
}

eResourceFormat ZipDataSource::GetType() { return eResourceFormat::Zip; }

void ZipDataSource::SetPath(const char *szPath) {
  if (szPath)
    m_strPath = szPath;
  else
    m_strPath.clear();
}
const char *ZipDataSource::GetPath() { return m_strPath.c_str(); }

void ZipDataSource::SetData(byte *data, int size) {
  UIASSERT(!m_unzip);

  UIASSERT(false && "TODO");

  // m_hZip = OpenZip(data, size, nullptr);
  // if (nullptr == m_hZip) {
  //   UI_LOG_ERROR("OpenZip Failed. file=%s", m_strPath.c_str());
  //   return;
  // }
}

bool ZipDataSource::Init() {
  if (nullptr == m_unzip) {
    m_unzip = unzOpen(m_strPath.c_str());
    if (nullptr == m_unzip) {
      UI_LOG_ERROR("OpenZip Failed. file=%s", m_strPath.c_str());
      return false;
    }
  }
  return true;
}

bool ZipDataSource::unzip_path(const char *path, std::vector<byte> &buffer) {
  if (!m_unzip) {
    return false;
  }

  char path2[MAX_PATH];
  TranslatePath(path, path2);

  if (unzLocateFile(m_unzip, path2, 0) != UNZ_OK) {
    UI_LOG_ERROR("File %s not found in zip", path);
    return false;
  }

  unz_file_info file_info;
  if (unzGetCurrentFileInfo(m_unzip, &file_info, NULL, 0, NULL, 0, NULL, 0) !=
      UNZ_OK) {
    UI_LOG_ERROR("Cannot get file info for %s\n", path);
    return false;
  }

  // 打开当前文件进行解压
  if (unzOpenCurrentFile(m_unzip) != UNZ_OK) {
    UI_LOG_ERROR("Cannot open file %s in zip\n", path);
    return false;
  }

  // 分配内存缓冲区
  unsigned int size = file_info.uncompressed_size;
  std::vector<byte> data(size);

  int bytes_read = unzReadCurrentFile(m_unzip, (void *)data.data(), size);
  unzCloseCurrentFile(m_unzip);

  if (bytes_read < 0 || (size_t)bytes_read != size) {
    UI_LOG_ERROR("Unzip file %s failed in zip\n", path);
    return false;
  }

  buffer.swap(data);
  return true;
}

bool ZipDataSource::Load_UIDocument(UIDocument *pDocument, const char *path) {
  if (!m_unzip && !Init()) {
    return false;
  }

  std::vector<byte> buffer;
  if (!unzip_path(path, buffer)) {
    return false;
  }

  return pDocument->LoadData(buffer.data(), buffer.size());
}

void ZipDataSource::TranslatePath(const char *szOrignPath, char *szLastPath) {
  const char *p = szOrignPath;
  char *p2 = szLastPath;

  // 跳过 .\xxx 表示的当前目录
  if (strlen(szOrignPath) > 2 && szOrignPath[0] == '.' &&
      (szOrignPath[1] == '/' || szOrignPath[1] == '\\')) {
    p += 2;
  }

  while (*p) {
    if (*p == '\\')
      *p2 = '/';
    else
      *p2 = *p;

    p++;
    p2++;
  }
  *p2 = '\0';
}

// 注：zip内部的路径符号是/
bool ZipDataSource::Load_RenderBitmap(IRenderBitmap *pBitmap, const char *path,
                                      RENDER_BITMAP_LOAD_FLAG e) {
  if (!m_unzip || !Init())
    return false;

  if (nullptr == pBitmap || nullptr == path)
    return false;

  std::vector<byte> buffer;
  if (!unzip_path(path, buffer)) {
    return false;
  }

  return pBitmap->LoadFromData(buffer.data(), buffer.size(), e);
}

bool ZipDataSource::Load_StreamBuffer(const char *path,
                                      IStreamBufferReader **pp) {
  if (!m_unzip || !Init())
    return false;

  if (nullptr == path || nullptr == pp)
    return false;

  std::vector<byte> buffer;
  if (!unzip_path(path, buffer)) {
    return false;
  }

  ByteBufferReader *pBuffer = new ByteBufferReader;
  pBuffer->load(buffer.data(), buffer.size(), true);
  *pp = pBuffer;

  return true;
}

bool ZipDataSource::FileExist(const char *szPath) {
  if (!m_unzip) {
    return false;
  }

  // 第二个参数："folder/test.txt"
  // 第三个参数：0表示大小写敏感，1为不敏感
  int result = unzLocateFile(m_unzip, szPath, 0);
  return result == UNZ_OK;
}

} // namespace ui
