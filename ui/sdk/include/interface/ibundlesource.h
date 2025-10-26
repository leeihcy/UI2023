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

struct IBufferData {
  virtual void AddRef() = 0;
  virtual void Release() = 0;
  virtual const unsigned char* Data() = 0;
  virtual unsigned int Size() = 0;
};

struct BundleSource;
struct UIAPI IBundleSource {
  IBundleSource(BundleSource *);

  const char *GetPath();
  eBundleFormat GetFormat();
  bool LoadBuffer(const char* path, slot<void(const char*, unsigned int)>&& callback);
  bool LoadBuffer(const char *path, IBufferData **pp);

private:
  BundleSource *m_pImpl;
};

} // namespace ui

#endif // _UI_ISKINDATASOURCE_H_