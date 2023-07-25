#pragma once
#include "include/interface/iskindatasource.h"
#include "include/interface/renderlibrary.h"

namespace ui
{
struct ISkinDataSource;
struct UIDocument;

struct SkinDataSource
{
    virtual ~SkinDataSource() {}
	virtual void  Release() = 0;
	virtual ISkinDataSource*  GetISkinDataSource() = 0;

	virtual void  SetPath(const wchar_t* szPath) = 0;
	virtual const wchar_t*  GetPath() = 0;
	virtual void  SetData(unsigned char* data, int size) = 0;
	virtual SKIN_PACKET_TYPE  GetType() = 0;

	virtual bool  FileExist(const wchar_t* szPath) = 0;

	virtual bool  Load_UIDocument(UIDocument* pDocument, const wchar_t* szPath) = 0;
	virtual bool  Load_RenderBitmap(IRenderBitmap* pBitmap, const wchar_t* szPath, RENDER_BITMAP_LOAD_FLAG e) = 0;
#if defined(OS_WIN)
	virtual bool  Load_Image(const wchar_t* szPath, ImageWrap* pImage) = 0;
	virtual bool  Load_StreamBuffer(const wchar_t* szPath, IStreamBufferReader** pp) = 0;
	virtual bool  Load_GdiplusImage(const wchar_t* szPath, GdiplusBitmapLoadWrap* pImage) = 0;
#endif
};

void  CreateDataSourceInstance(SKIN_PACKET_TYPE e, SkinDataSource** pp);
void  CreateStreamBuffer(STREAM_TYPE e, IStreamBufferReader** pp);



}