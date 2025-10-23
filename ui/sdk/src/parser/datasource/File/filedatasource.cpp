#include "filedatasource.h"
#include "include/util/path.h"
#include "sdk/include/macro/helper.h"
#include "src/parser/datasource/file/filebufferreader.h"
#include "src/parser/xml/xmlwrap.h"
#include "src/util/util.h"
#include <filesystem>
#include <fstream>
#include <ios>
#include <vector>

namespace ui {

FileDataSource::FileDataSource() : m_IBundleSource(this) {}
IBundleSource *FileDataSource::GetIBundleSource() {
  return &m_IBundleSource;
}

void FileDataSource::SetPath(const char *path) {
  if (path)
    m_path = path;
  else
    m_path.clear();

  const std::filesystem::path p = path;
  if (!p.is_absolute()) {
    m_path = std::filesystem::absolute(p).string();
  }

  int nLength = (int)m_path.length();
  if (nLength > 1) {
    if (m_path[nLength - 1] != Path::SEPARATOR) {
      m_path.push_back(Path::SEPARATOR);
    }
  }
}

const char *FileDataSource::GetPath() { return m_path.c_str(); }

eBundleFormat FileDataSource::GetType() { return eBundleFormat::Directory; }

bool FileDataSource::loadFullPath(const char *path, std::string &full_path) {
  std::filesystem::path p(m_path.c_str());
  p.append(path);
  full_path = p.string();
  return true;
}

bool FileDataSource::loadBuffer(const char* path, std::vector<unsigned char>& buffer) {
 if (!path)
    return false;

  std::string strTemp = m_path;
  strTemp.append(path);
 
  if (!util::PathExists(strTemp.c_str())) {
    return false;
  }

  std::fstream f(strTemp.c_str(), std::ios_base::in|std::ios_base::binary);
  if (!f) {
    return false;
  }

  f.seekg(0, std::ios::end);
  size_t size = f.tellg();

  buffer.reserve(size);
  f.seekg(0, std::ios::beg);

  f.read((char*)buffer.data(), size);
  return true;
}

bool FileDataSource::LoadBuffer(const char *path,
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

bool FileDataSource::Load_UIDocument(UIDocument *pDocument,
                                     const char *path) {
  if (nullptr == pDocument || nullptr == path)
    return false;

  std::string strTemp = m_path;
  strTemp.append(path);

  return pDocument->LoadFile(strTemp.c_str());
}

bool FileDataSource::Load_RenderBitmap(IRenderBitmap *pBitmap,
                                       const char *path,
                                       RENDER_BITMAP_LOAD_FLAG e) {
  if (nullptr == pBitmap || nullptr == path)
    return false;

  std::string strTemp = m_path;
  strTemp.append(path);

  return pBitmap->LoadFromFile(strTemp.c_str(), e);
}


bool FileDataSource::FileExist(const char *path) {
  std::string strTemp = m_path;
  strTemp.append(path);

  return ui::util::PathExists(strTemp.c_str()) ? true : false;
}

} // namespace ui