#ifndef _UI_ISKINDATASOURCE_H_
#define _UI_ISKINDATASOURCE_H_
#include <ios>
#include "sdk/include/interface.h"
#include "sdk/include/common/signalslot/slot.h"

namespace Gdiplus {
class Bitmap;
}
namespace ui {

enum SKIN_PACKET_TYPE {
  SKIN_PACKET_TYPE_DIR,
  SKIN_PACKET_TYPE_ZIP,
  SKIN_PACKET_TYPE_RESZIP,
};

// 跨模块加载gdiplus::bitmap时使用。确保Bitmap*都在同一个模块new/delete
class UIAPI GdiplusBitmapLoadWrap {
public:
  GdiplusBitmapLoadWrap();
  ~GdiplusBitmapLoadWrap();
  Gdiplus::Bitmap *GetBitmap();

  void Destroy();
  bool LoadFromFile(const char *szPath);
  bool LoadFromByte(unsigned char *pByte, int nSize);

private:
  Gdiplus::Bitmap *m_pBitmap;
};

struct ImageWrap;
struct IStreamBufferReader;
struct SkinDataSource;
struct UIAPI ISkinDataSource {
  ISkinDataSource(SkinDataSource *);

  const char *GetPath();
  SKIN_PACKET_TYPE GetType();
  bool Load(const char* szPath, slot<void(const char*)>&& callback);
  bool Load_Image(const char *szPath, ImageWrap *pImage);
  bool Load_GdiplusImage(const char *szPath, GdiplusBitmapLoadWrap *pImage);
  bool Load_StreamBuffer(const char *szPath, IStreamBufferReader **pp);

private:
  SkinDataSource *m_pImpl;
};

enum STREAM_TYPE {
  STREAM_TYPE_FILE,
  STREAM_TYPE_BYTE,
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