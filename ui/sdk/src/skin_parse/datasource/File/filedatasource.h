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

    virtual void  SetPath(const wchar_t* szPath) override;
    virtual const wchar_t*  GetPath() override;
	virtual void  SetData(byte* data, int size) override;
    virtual SKIN_PACKET_TYPE  GetType() override;

    virtual bool  Load_UIDocument(UIDocument* pDocument, const wchar_t* szPath) override;
    virtual bool  Load_RenderBitmap(IRenderBitmap* pBitmap, const wchar_t* szPath, RENDER_BITMAP_LOAD_FLAG e) override;
#if defined(OS_WIN)
    virtual bool  Load_Image(const wchar_t* szPath, ImageWrap* pImage) override;
    virtual bool  Load_GdiplusImage(const wchar_t* szPath, GdiplusBitmapLoadWrap* pImage) override;
    virtual bool  Load_StreamBuffer(const wchar_t* szPath, IStreamBufferReader** pp) override;
#endif
	virtual bool  FileExist(const wchar_t* szPath) override;

private:
    String  m_strPath;
	ISkinDataSource  m_ISkinDataSource;
};

}