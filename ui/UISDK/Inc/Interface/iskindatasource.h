#ifndef _UI_ISKINDATASOURCE_H_
#define _UI_ISKINDATASOURCE_H_
#include <xiosbase>

namespace Gdiplus { class Bitmap; }
namespace UI
{

enum  SKIN_PACKET_TYPE
{
    SKIN_PACKET_TYPE_DIR,
    SKIN_PACKET_TYPE_ZIP,
	SKIN_PACKET_TYPE_RESZIP,
};

// 跨模块加载gdiplus::bitmap时使用。确保Bitmap*都在同一个模块new/delete
class UIAPI GdiplusBitmapLoadWrap
{
public:
    GdiplusBitmapLoadWrap();
    ~GdiplusBitmapLoadWrap();
    Gdiplus::Bitmap*  GetBitmap();

    void  Destroy();
    bool  LoadFromFile(LPCTSTR szPath);
    bool  LoadFromByte(LPBYTE pByte, int nSize);

private:
    Gdiplus::Bitmap*  m_pBitmap;
};

interface ImageWrap;
interface IStreamBufferReader;
interface SkinDataSource;
interface UIAPI ISkinDataSource
{
	ISkinDataSource(SkinDataSource*);

    LPCTSTR  GetPath();
    SKIN_PACKET_TYPE  GetType();
	bool  Load_Image(LPCTSTR szPath, ImageWrap* pImage);
	bool  Load_GdiplusImage(LPCTSTR szPath, GdiplusBitmapLoadWrap* pImage);
	bool  Load_StreamBuffer(LPCTSTR szPath, IStreamBufferReader** pp);

private:
	SkinDataSource* m_pImpl;
};


enum  STREAM_TYPE
{
    STREAM_TYPE_FILE,
    STREAM_TYPE_BYTE,
};

interface IStreamBufferReader
{
    virtual void  Release() = 0;
    virtual int   read(char* pread, int nread) = 0;
    virtual void  seek(int npos, ios_base::seek_dir dir) = 0;
    virtual int   tell() = 0;
    virtual bool  save(LPCTSTR szPath) = 0;
};

interface IFileBufferReader : public IStreamBufferReader
{
    virtual bool  load(LPCTSTR szPath) = 0;
};
interface IByteBufferReader : public IStreamBufferReader
{
    virtual bool  load(const byte* pdata, int nsize, bool bdeletedata) = 0;
};

}

#endif // _UI_ISKINDATASOURCE_H_