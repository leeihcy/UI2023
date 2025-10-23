#ifndef _UI_SDK_SRC_SKINPARSE_DATASOURCE_ZIP_ZIPDATASOURCE_H_
#define _UI_SDK_SRC_SKINPARSE_DATASOURCE_ZIP_ZIPDATASOURCE_H_

#include "include/interface/ibundlesource.h"
#include "src/parser/datasource/bundle_source.h"
#include "3rd/zlib/contrib/minizip/unzip.h"

namespace ui {

class ZipDataSource : public BundleSource {
public:
  ZipDataSource();
  ~ZipDataSource();

  virtual IBundleSource *GetIBundleSource() override;

  void SetPath(const char *szPath) override;
  const char *GetPath() override;
  // void SetData(byte *data, int size) override;

  eBundleFormat GetType() override;

  bool FileExist(const char *szPath) override;
  bool loadBuffer(const char* path, std::vector<unsigned char>& buffer) override;
  bool loadFullPath(const char *path, std::string &full_path) override;
  bool LoadBuffer(const char* szPath, slot<void(const char*, unsigned int)>&& callback) override;

  bool Load_UIDocument(UIDocument *pDocument, const char *szPath) override;
  bool Load_RenderBitmap(IRenderBitmap *pBitmap, const char *szPath,
                         RENDER_BITMAP_LOAD_FLAG e) override;
  bool Load_StreamBuffer(const char *szPath, IStreamBufferReader **pp);

private:
  void TranslatePath(const char *szOrignPath, char *szLastPath);
  bool Init();

private:
  std::string m_strPath;
  IBundleSource m_IBundleSource;

  unzFile m_unzip;
};

} // namespace ui
#endif