#ifndef _UI_SDK_SRC_SKINPARSE_DATASOURCE_ZIP_ZIPDATASOURCE_H_
#define _UI_SDK_SRC_SKINPARSE_DATASOURCE_ZIP_ZIPDATASOURCE_H_

#include "include/interface/iskindatasource.h"
#include "src/skin_parse/datasource/skindatasource.h"
#include "3rd/zlib/contrib/minizip/unzip.h"

namespace ui {

class ZipDataSource : public SkinDataSource {
public:
  ZipDataSource();
  ~ZipDataSource();

  static ZipDataSource *Create();

  virtual void Release() override;
  virtual ISkinDataSource *GetISkinDataSource() override;

  void SetPath(const char *szPath) override;
  const char *GetPath() override;
  void SetData(byte *data, int size) override;

  eResourceFormat GetType() override;

  bool Load_UIDocument(UIDocument *pDocument, const char *szPath) override;
  bool Load_RenderBitmap(IRenderBitmap *pBitmap, const char *szPath,
                         RENDER_BITMAP_LOAD_FLAG e) override;
  bool Load_StreamBuffer(const char *szPath, IStreamBufferReader **pp);
  bool FileExist(const char *szPath) override;
  bool Load(const char* szPath, slot<void(const char*)>&& callback) override { return false; }

private:
  void TranslatePath(const char *szOrignPath, char *szLastPath);
  bool Init();

  bool unzip_path(const char* path, std::vector<byte>& buffer);

private:
  std::string m_strPath;
  ISkinDataSource m_ISkinDataSource;

  unzFile m_unzip;
};

} // namespace ui
#endif