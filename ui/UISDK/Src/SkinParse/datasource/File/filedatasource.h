#pragma once
#include "Inc\Interface\iskindatasource.h"
#include "..\skindatasource.h"

namespace UI
{

class FileDataSource : public SkinDataSource
{
public:
	FileDataSource();
	static FileDataSource*  Create();

    virtual void  Release() override;
	virtual ISkinDataSource*  GetISkinDataSource() override;

    virtual void  SetPath(LPCTSTR szPath) override;
    virtual LPCTSTR  GetPath() override;
	virtual void  SetData(byte* data, int size) override;
    virtual SKIN_PACKET_TYPE  GetType() override;

    virtual bool  Load_UIDocument(UIDocument* pDocument, LPCTSTR szPath) override;
    virtual bool  Load_RenderBitmap(IRenderBitmap* pBitmap, LPCTSTR szPath, RENDER_BITMAP_LOAD_FLAG e) override;
    virtual bool  Load_Image(LPCTSTR szPath, ImageWrap* pImage) override;
    virtual bool  Load_GdiplusImage(LPCTSTR szPath, GdiplusBitmapLoadWrap* pImage) override;
    virtual bool  Load_StreamBuffer(LPCTSTR szPath, IStreamBufferReader** pp) override;
	virtual bool  FileExist(LPCTSTR szPath) override;

private:
    String  m_strPath;
	ISkinDataSource  m_ISkinDataSource;
};

}