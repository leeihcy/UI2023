#pragma once
#include "Inc\Interface\iskindatasource.h"
#include "..\3rd\zip\unzip.h"
#include "..\skindatasource.h"

namespace UI
{

class ZipDataSource : public SkinDataSource
{
public:
    ZipDataSource();
    ~ZipDataSource();

	static ZipDataSource*  Create();
    
	virtual void  Release() override;
	virtual ISkinDataSource*  GetISkinDataSource() override;

    void  SetPath(LPCTSTR szPath);
	LPCTSTR  GetPath();
	void  SetData(byte* data, int size);

    SKIN_PACKET_TYPE  GetType();

    bool  Load_UIDocument(UIDocument* pDocument, LPCTSTR szPath);
    bool  Load_RenderBitmap(IRenderBitmap* pBitmap, LPCTSTR szPath, RENDER_BITMAP_LOAD_FLAG e);
    bool  Load_Image(LPCTSTR szPath, ImageWrap* pImage);
    bool  Load_GdiplusImage(LPCTSTR szPath, GdiplusBitmapLoadWrap* pImage);
    bool  Load_StreamBuffer(LPCTSTR szPath, IStreamBufferReader** pp);
	bool  FileExist(LPCTSTR szPath);
	
private:
    void  TranslatePath(LPCTSTR szOrignPath, TCHAR* szLastPath);
	bool  Init();

private:
    String  m_strPath;

    HZIP  m_hZip;
	ISkinDataSource  m_ISkinDataSource;
};

}