#ifndef _UI_ISKINDATASOURCE_H_
#define _UI_ISKINDATASOURCE_H_
#include <ios>
#include "sdk/include/interface.h"
#include "sdk/include/common/signalslot/slot.h"

namespace Gdiplus {
class Bitmap;
}
namespace ui {

enum class eBundleFormat {
  // 原始目录结构
  Directory,
  // 压缩包结构
  Zip, 
  // dll/exe/...模块资源
  ModuleResource,
};

enum class eStreamType {
  File,
  Byte,
};

struct IStreamBufferReader;
struct BundleSource;
struct UIAPI IBundleSource {
  IBundleSource(BundleSource *);

  const char *GetPath();
  eBundleFormat GetType();
  bool LoadBuffer(const char* szPath, slot<void(const char*, unsigned int)>&& callback);
  bool Load_StreamBuffer(const char *szPath, IStreamBufferReader **pp);

private:
  BundleSource *m_pImpl;
};

struct IStreamBufferReader {
  virtual void Release() = 0;
  virtual int read(char *pread, int nread) = 0;
  virtual void seek(int npos, std::ios::seekdir dir) = 0;
  virtual int tell() = 0;
  virtual bool save(const char *szPath) = 0;
};

struct IFileBufferReader : public IStreamBufferReader {
  virtual bool load(const char *szPath) = 0;
};
struct IByteBufferReader : public IStreamBufferReader {
  virtual bool load(const byte *pdata, int nsize, bool bdeletedata) = 0;
};

} // namespace ui

#endif // _UI_ISKINDATASOURCE_H_