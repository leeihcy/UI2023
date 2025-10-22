#pragma once
#include "include/interface/iskindatasource.h"
// #include "../3rd/zip/unzip.h"
#include "../skindatasource.h"

namespace ui
{

class ZipDataSource : public SkinDataSource
{
public:
    ZipDataSource();
    ~ZipDataSource();

	static ZipDataSource*  Create();
    
	virtual void  Release() override;
	virtual ISkinDataSource*  GetISkinDataSource() override;

    void  SetPath(const wchar_t* szPath);
	const wchar_t*  GetPath();
	void  SetData(byte* data, int size);

    eResourceFormat  GetType();

    bool  Load_UIDocument(UIDocument* pDocument, const wchar_t* szPath);
    bool  Load_RenderBitmap(IRenderBitmap* pBitmap, const wchar_t* szPath, RENDER_BITMAP_LOAD_FLAG e);
    bool  Load_Image(const wchar_t* szPath, ImageWrap* pImage);
    bool  Load_GdiplusImage(const wchar_t* szPath, GdiplusBitmapLoadWrap* pImage);
    bool  Load_StreamBuffer(const wchar_t* szPath, IStreamBufferReader** pp);
	bool  FileExist(const wchar_t* szPath);
	
private:
    void  TranslatePath(const wchar_t* szOrignPath, wchar_t* szLastPath);
	bool  Init();

private:
    std::string  m_strPath;

    HZIP  m_hZip;
	ISkinDataSource  m_ISkinDataSource;
};

}