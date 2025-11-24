#pragma once
#include "../bundle_source.h"
#include "include/interface/ibundlesource.h"

namespace ui {

class FileDataSource : public BundleSource {
public:
  FileDataSource();

  IBundleSource *GetIBundleSource() override;

  void SetPath(const char *szPath) override;
  const char *GetPath() override;
  eBundleFormat GetFormat() override;

  bool FileExist(const char *szPath) override;
  bool loadBuffer(const char *path,
                  std::vector<unsigned char> &buffer) override;
  bool loadFullPath(const char *path, std::string &full_path) override;
  bool LoadBuffer(const char *szPath,
                  slot<void(const char *, unsigned int)> &&callback) override;
  bool LoadBuffer(const char *path, IBufferData **pp) override;

private:
  std::string m_path;
  IBundleSource m_IBundleSource;
};

} // namespace ui