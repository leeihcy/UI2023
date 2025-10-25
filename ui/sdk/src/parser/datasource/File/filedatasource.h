#pragma once
#include "../bundle_source.h"
#include "include/interface/ibundlesource.h"

namespace ui {

class FileDataSource : public BundleSource {
public:
  FileDataSource();

  virtual IBundleSource *GetIBundleSource() override;

  virtual void SetPath(const char *szPath) override;
  virtual const char *GetPath() override;
  virtual eBundleFormat GetType() override;

  virtual bool FileExist(const char *szPath) override;
  virtual bool loadBuffer(const char *path,
                    std::vector<unsigned char> &buffer) override;
  virtual bool loadFullPath(const char *path, std::string &full_path) override;
  virtual bool LoadBuffer(const char *szPath,
                    slot<void(const char *, unsigned int)> &&callback) override;

private:
  std::string m_path;
  IBundleSource m_IBundleSource;
};

} // namespace ui