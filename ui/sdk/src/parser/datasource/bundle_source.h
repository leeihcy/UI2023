#ifndef _UI_SDK_SRC_SKINPARSE_DATASOURCE_SKINDATASOURCE_H_
#define _UI_SDK_SRC_SKINPARSE_DATASOURCE_SKINDATASOURCE_H_

#include "include/common/signalslot/slot.h"
#include "include/interface/ibundlesource.h"
#include "include/interface/renderlibrary.h"
#include <vector>

namespace ui {
struct IBundleSource;
struct UIDocument;

struct BundleSource {
  virtual ~BundleSource() {};
  virtual IBundleSource *GetIBundleSource() = 0;

  virtual void SetPath(const char *path) = 0;
  virtual const char *GetPath() = 0;
  // virtual void SetData(unsigned char *data, int size) = 0;

  virtual eBundleFormat GetType() = 0;

  virtual bool FileExist(const char *path) = 0;
  // 不要跨模块调用
  virtual bool loadBuffer(const char *path, std::vector<unsigned char> &buffer) = 0;
  // 不要跨模块调用
  // 仅对Directory有效。获取资源在磁盘上的完整路径。
  virtual bool loadFullPath(const char *path, std::string &full_path) = 0;
  // 可以跨模块调用
  virtual bool LoadBuffer(const char *path,
                    slot<void(const char *, unsigned int)> &&callback) = 0;
};

std::unique_ptr<BundleSource> CreateBundleSource(eBundleFormat e);
void CreateStreamBuffer(eStreamType e, IStreamBufferReader **pp);

} // namespace ui
#endif