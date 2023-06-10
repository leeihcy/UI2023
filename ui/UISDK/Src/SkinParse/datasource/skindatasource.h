#pragma once
#include "Inc\Interface\iskindatasource.h"
#include "Inc\Interface\renderlibrary.h"

namespace UI
{
interface ISkinDataSource;
interface UIDocument;

interface SkinDataSource
{
	virtual void  Release() = 0;
	virtual ISkinDataSource*  GetISkinDataSource() = 0;

	virtual void  SetPath(LPCTSTR szPath) = 0;
	virtual LPCTSTR  GetPath() = 0;
	virtual void  SetData(byte* data, int size) = 0;
	virtual SKIN_PACKET_TYPE  GetType() = 0;

	virtual bool  FileExist(LPCTSTR szPath) = 0;

	virtual bool  Load_UIDocument(UIDocument* pDocument, LPCTSTR szPath) = 0;
	virtual bool  Load_RenderBitmap(IRenderBitmap* pBitmap, LPCTSTR szPath, RENDER_BITMAP_LOAD_FLAG e) = 0;
	virtual bool  Load_Image(LPCTSTR szPath, ImageWrap* pImage) = 0;
	virtual bool  Load_StreamBuffer(LPCTSTR szPath, IStreamBufferReader** pp) = 0;
	virtual bool  Load_GdiplusImage(LPCTSTR szPath, GdiplusBitmapLoadWrap* pImage) = 0;

};

void  CreateDataSourceInstance(SKIN_PACKET_TYPE e, SkinDataSource** pp);
void  CreateStreamBuffer(STREAM_TYPE e, IStreamBufferReader** pp);



}