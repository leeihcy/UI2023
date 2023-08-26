#pragma once
#include "include/interface/iskindatasource.h"
#include "../skindatasource.h"

namespace ui
{

class FileDataSource : public SkinDataSource
{
public:
	FileDataSource();
	static FileDataSource*  Create();

    virtual void  Release() override;
	virtual ISkinDataSource*  GetISkinDataSource() override;

    virtual void  SetPath(const char* szPath) override;
    virtual const char*  GetPath() override;
	virtual void  SetData(byte* data, int size) override;
    virtual SKIN_PACKET_TYPE  GetType() override;

    virtual bool  Load_UIDocument(UIDocument* pDocument, const char* szPath) override;
    virtual bool  Load_RenderBitmap(IRenderBitmap* pBitmap, const char* szPath, RENDER_BITMAP_LOAD_FLAG e) override;
#if defined(OS_WIN)
    virtual bool  Load_Image(const char* szPath, ImageWrap* pImage) override;
    virtual bool  Load_GdiplusImage(const char* szPath, GdiplusBitmapLoadWrap* pImage) override;
    virtual bool  Load_StreamBuffer(const char* szPath, IStreamBufferReader** pp) override;
#endif
	virtual bool  FileExist(const char* szPath) override;

private:
    std::string  m_strPath;
	ISkinDataSource  m_ISkinDataSource;
};

}