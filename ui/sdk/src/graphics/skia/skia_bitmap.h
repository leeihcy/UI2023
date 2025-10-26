#include "include/interface/graphics.h"
#include "include/interface/ibundlesource.h"
#include "third_party/skia/src/include/core/SkBitmap.h"
#include <memory>
// #include "Src\Util\DPI\dpihelper.h"

namespace ui
{
 
class SkiaRenderBitmap : public IRenderBitmap
{
public:
  static std::shared_ptr<IRenderBitmap> CreateInstance();
  ~SkiaRenderBitmap() override;

public:
  eGraphicsLibraryType GetGraphicsRenderLibraryType() override;

  bool LoadFromFile(const char *szPath)override;
  bool LoadFromData(IBufferData* data)override;

  eImageItemType GetImageType()override;
  bool Create(int nWidth, int nHeight)override;
  void Destroy()override;
  int GetWidth()override;
  int GetHeight()override;
  int GetBPP()override;

public:
  SkBitmap m_bitmap;
  IBufferData* m_buffer = nullptr;
};

// class GDIIconRenderBitmap : public SkiaRenderBitmapImpl<IRenderResourceImpl<IImageIconRenderBitmap > >// : public SkiaRenderBitmap
// {
// public:
// 	GDIIconRenderBitmap();

// 	static  void  CreateInstance(IRenderBitmap** pOut);
// 	virtual bool  LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e);
//     virtual bool  LoadFromData(byte* pData, int nSize, RENDER_BITMAP_LOAD_FLAG e);
    
//     virtual eImageItemType  GetImageType() { return IMAGE_ITEM_TYPE_ICON; }

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

//     virtual eImageItemType  GetImageType() { return IMAGE_ITEM_TYPE_IMAGE_LIST; }
// private:
// 	IMAGELIST_LAYOUT_TYPE   m_eLayout;
// 	int     m_nCount;
// };
}

