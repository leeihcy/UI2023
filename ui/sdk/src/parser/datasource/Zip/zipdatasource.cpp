#include "zipdatasource.h"
#include "include/inc.h"
#include "include/interface/ibundlesource.h"
#include "include/macro/helper.h"
#include "include/util/log.h"
#include "src/parser/xml/xmlwrap.h"
#include <vector>
#define MAX_PATH 256

namespace ui {

ZipDataSource::ZipDataSource()
    : m_IBundleSource(static_cast<BundleSource *>(this)) {
  m_unzip = 0;
}

ZipDataSource::~ZipDataSource() {
  if (m_unzip) {
    unzClose(m_unzip);
    m_unzip = nullptr;
  }
}

IBundleSource *ZipDataSource::GetIBundleSource() {
  return &m_IBundleSource;
}

eBundleFormat ZipDataSource::GetFormat() { return eBundleFormat::Zip; }

void ZipDataSource::SetPath(const char *szPath) {
  if (szPath)
    m_strPath = szPath;
  else
    m_strPath.clear();

  Init();
}
const char *ZipDataSource::GetPath() { return m_strPath.c_str(); }

// void ZipDataSource::SetData(byte *data, int size) {
//   UIASSERT(!m_unzip);

//   UIASSERT(false && "TODO");

//   // m_hZip = OpenZip(data, size, nullptr);
//   // if (nullptr == m_hZip) {
//   //   UI_LOG_ERROR("OpenZip Failed. file=%s", m_strPath.c_str());
//   //   return;
//   // }
// }

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

bool ZipDataSource::loadFullPath(const char *path, std::string &full_path) { 
  UIASSERT(false);
  return false;
}

bool ZipDataSource::loadBuffer(const char* path, std::vector<unsigned char>& buffer) {
  if (!m_unzip) {
    return false;
  }

  // char path2[MAX_PATH] = {0};
  // TranslatePath(path, path2);

  if (unzLocateFile(m_unzip, path, 0) != UNZ_OK) {
    UI_LOG_ERROR("File %s not found in zip", path);
    return false;
  }

  unz_file_info file_info;
  if (unzGetCurrentFileInfo(m_unzip, &file_info, NULL, 0, NULL, 0, NULL, 0) !=
      UNZ_OK) {
    UI_LOG_ERROR("Cannot get file info for %s\n", path);
    return false;
  }

  if (unzOpenCurrentFile(m_unzip) != UNZ_OK) {
    UI_LOG_ERROR("Cannot open file %s in zip\n", path);
    return false;
  }

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

bool ZipDataSource::LoadBuffer(const char *path,
                          slot<void(const char *, unsigned int)> &&callback) {
  std::vector<byte> buffer;
  if (!loadBuffer(path, buffer)) {
    callback.emit(nullptr, 0);
    return false;
  } else {
    callback.emit((const char*)buffer.data(), buffer.size());
    return true;
  }
}

class BundleBufferImpl : public IBufferData {
public:
  static BundleBufferImpl* Create(std::vector<byte>& data) {
    BundleBufferImpl* p = new BundleBufferImpl();
    p->AddRef();
    p->m_data.swap(data);
    return p;
  }
  void AddRef() override {
    m_ref++;
  }
  void Release() override {
    m_ref--;
    if (m_ref == 0) {
      delete this;
    }
  }
  const unsigned char* Data() override {
    return m_data.data();
  }
  unsigned int Size() override {
    return m_data.size();
  }
public:
  std::vector<byte> m_data;
  std::atomic<int> m_ref = 0;
};
bool ZipDataSource::LoadBuffer(const char *path, IBufferData **pp) {
  std::vector<byte> buffer;
  if (!loadBuffer(path, buffer)) {
    return false;
  }

  BundleBufferImpl *impl = BundleBufferImpl::Create(buffer);
  *pp = impl;
  return true;
}

#if 0
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
#endif

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
