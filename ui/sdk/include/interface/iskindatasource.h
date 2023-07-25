#ifndef _UI_ISKINDATASOURCE_H_
#define _UI_ISKINDATASOURCE_H_
#if defined(OS_WIN)
#include <xiosbase>
#else
#include <ios>
#endif

#include "inc.h"

namespace Gdiplus { class Bitmap; }
namespace ui
{

enum  SKIN_PACKET_TYPE
{
    SKIN_PACKET_TYPE_DIR,
    SKIN_PACKET_TYPE_ZIP,
	SKIN_PACKET_TYPE_RESZIP,
};

// ��ģ�����gdiplus::bitmapʱʹ�á�ȷ��Bitmap*����ͬһ��ģ��new/delete
class UIAPI GdiplusBitmapLoadWrap
{
public:
    GdiplusBitmapLoadWrap();
    ~GdiplusBitmapLoadWrap();
    Gdiplus::Bitmap*  GetBitmap();

    void  Destroy();
    bool  LoadFromFile(const wchar_t* szPath);
    bool  LoadFromByte(unsigned char* pByte, int nSize);

private:
    Gdiplus::Bitmap*  m_pBitmap;
};

struct ImageWrap;
struct IStreamBufferReader;
struct SkinDataSource;
struct UIAPI ISkinDataSource
{
	ISkinDataSource(SkinDataSource*);

    const wchar_t*  GetPath();
    SKIN_PACKET_TYPE  GetType();
	bool  Load_Image(const wchar_t* szPath, ImageWrap* pImage);
	bool  Load_GdiplusImage(const wchar_t* szPath, GdiplusBitmapLoadWrap* pImage);
	bool  Load_StreamBuffer(const wchar_t* szPath, IStreamBufferReader** pp);

private:
	SkinDataSource* m_pImpl;
};


enum  STREAM_TYPE
{
    STREAM_TYPE_FILE,
    STREAM_TYPE_BYTE,
};

struct IStreamBufferReader
{
    virtual void  Release() = 0;
    virtual int   read(char* pread, int nread) = 0;
#if defined(OS_WIN)
    virtual void  seek(int npos, std::ios_base::seek_dir dir) = 0;
#else
    virtual void  seek(int npos, std::ios::seekdir dir) = 0;
#endif
    virtual int   tell() = 0;
    virtual bool  save(const wchar_t* szPath) = 0;
};

struct IFileBufferReader : public IStreamBufferReader
{
    virtual bool  load(const wchar_t* szPath) = 0;
};
struct IByteBufferReader : public IStreamBufferReader
{
    virtual bool  load(const byte* pdata, int nsize, bool bdeletedata) = 0;
};

}

#endif // _UI_ISKINDATASOURCE_H_