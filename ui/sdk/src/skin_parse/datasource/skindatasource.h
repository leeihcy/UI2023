#pragma once
#include "include/interface/iskindatasource.h"
#include "include/interface/renderlibrary.h"
#include "include/common/signalslot/slot.h"

namespace ui
{
struct ISkinDataSource;
struct UIDocument;

struct SkinDataSource
{
  virtual ~SkinDataSource() {}
	virtual void  Release() = 0;
	virtual ISkinDataSource*  GetISkinDataSource() = 0;

	virtual void  SetPath(const char* szPath) = 0;
	virtual const char*  GetPath() = 0;
	virtual void  SetData(unsigned char* data, int size) = 0;
	virtual eResourceFormat  GetType() = 0;

	virtual bool  FileExist(const char* szPath) = 0;
  virtual bool  Load(const char* szPath, slot<void(const char*)>&& callback) = 0;

	virtual bool  Load_UIDocument(UIDocument* pDocument, const char* szPath) = 0;
	virtual bool  Load_RenderBitmap(IRenderBitmap* pBitmap, const char* szPath, RENDER_BITMAP_LOAD_FLAG e) = 0;
#if 0 // defined(OS_WIN)
	virtual bool  Load_Image(const char* szPath, ImageWrap* pImage) = 0;
	virtual bool  Load_StreamBuffer(const char* szPath, IStreamBufferReader** pp) = 0;
	virtual bool  Load_GdiplusImage(const char* szPath, GdiplusBitmapLoadWrap* pImage) = 0;
#endif
};

void  CreateDataSourceInstance(eResourceFormat e, SkinDataSource** pp);
void  CreateStreamBuffer(eStreamType e, IStreamBufferReader** pp);



}