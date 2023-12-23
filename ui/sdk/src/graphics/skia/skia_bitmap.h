#include "include/interface/renderlibrary.h"
#include "ui/3rd/skia/src/include/core/SkBitmap.h"
#include <memory>
// #include "Src\Util\DPI\dpihelper.h"

namespace ui
{
 
class SkiaRenderBitmap : public IRenderBitmap
{
public:
  static std::shared_ptr<IRenderBitmap> CreateInstance();

public:
  GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() override;

  bool LoadFromFile(const char *szPath,
                            RENDER_BITMAP_LOAD_FLAG e)override;
  bool LoadFromData(unsigned char *pData, int nSize,
                            RENDER_BITMAP_LOAD_FLAG e)override;

  IMAGE_ITEM_TYPE GetImageType()override;
  bool Create(int nWidth, int nHeight)override;
  void Destroy()override;
  int GetWidth()override;
  int GetHeight()override;
  int GetBPP()override;

public:
  SkBitmap m_bitmap;
};

// class GDIIconRenderBitmap : public SkiaRenderBitmapImpl<IRenderResourceImpl<IImageIconRenderBitmap > >// : public SkiaRenderBitmap
// {
// public:
// 	GDIIconRenderBitmap();

// 	static  void  CreateInstance(IRenderBitmap** pOut);
// 	virtual bool  LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e);
//     virtual bool  LoadFromData(byte* pData, int nSize, RENDER_BITMAP_LOAD_FLAG e);
    
//     virtual IMAGE_ITEM_TYPE  GetImageType() { return IMAGE_ITEM_TYPE_ICON; }

//     virtual SIZE  GetDrawSize();
//     virtual void  SetDrawSize(SIZE* ps);
// private:
// 	int    m_nIconWidth;
// 	int    m_nIconHeight;
// };

// class GDIImageListRenderBitmap : public SkiaRenderBitmapImpl<IRenderResourceImpl<IImageListRenderBitmap> >
// {
// protected:
// 	GDIImageListRenderBitmap();

// public:
// 	static  void CreateInstance(IRenderBitmap** pOut);

// 	virtual int  GetItemWidth();
// 	virtual int  GetItemHeight();
// 	virtual IMAGELIST_LAYOUT_TYPE GetLayoutType();
// 	virtual bool GetIndexPos(int nIndex, POINT* pPoint);
// 	virtual int  GetItemCount() { return m_nCount; }
//     virtual void  SetItemCount(int);
//     virtual void  SetLayoutType(IMAGELIST_LAYOUT_TYPE);

// 	virtual bool  LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e);
// 	virtual bool  virtualLoadImageFromBitmap(
// 		Gdiplus::Bitmap* pBmp, RENDER_BITMAP_LOAD_FLAG e) override;

//     virtual IMAGE_ITEM_TYPE  GetImageType() { return IMAGE_ITEM_TYPE_IMAGE_LIST; }
// private:
// 	IMAGELIST_LAYOUT_TYPE   m_eLayout;
// 	int     m_nCount;
// };
}

