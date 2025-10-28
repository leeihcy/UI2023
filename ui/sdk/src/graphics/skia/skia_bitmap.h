#include "include/interface/graphics.h"
#include "include/interface/ibundlesource.h"
#include "third_party/skia/src/include/core/SkBitmap.h"
#include "third_party/skia/src/include/core/SkImage.h"
#include "third_party/skia/src/include/core/SkData.h"
#include "third_party/skia/src/include/codec/SkCodec.h"
#include <memory>

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

  eImageType GetImageType()override;
  bool Create(int nWidth, int nHeight)override;
  void Destroy()override;
  int GetWidth()override;
  int GetHeight()override;
  int GetBPP()override;

  void Dump(const char* path);

  SkImage* GetImage() { return m_image.get(); }

protected:
  virtual bool loadBySkData(sk_sp<SkData> data);

public:
  // SkBitmap m_bitmap;
  sk_sp<SkImage>  m_image;
  IBufferData* m_buffer = nullptr;
};

class SkiaRenderGif : public SkiaRenderBitmap {
public:
  static std::shared_ptr<IRenderBitmap> CreateInstance();
  bool loadBySkData(sk_sp<SkData> data) override;

  bool Tick();

private: 
  void advanceFrame();

private:
  std::vector<sk_sp<SkImage>> m_frames;
  std::vector<SkCodec::FrameInfo> m_frame_infos;
  int m_current_frame = 0;
  int m_frame_count = 0;

  std::chrono::steady_clock::time_point m_last_frame_time;
  
};

// class GDIIconRenderBitmap : public SkiaRenderBitmapImpl<IRenderResourceImpl<IImageIconRenderBitmap > >// : public SkiaRenderBitmap
// {
// public:
// 	GDIIconRenderBitmap();

// 	static  void  CreateInstance(IRenderBitmap** pOut);
// 	virtual bool  LoadFromFile(const TCHAR* szPath, RENDER_BITMAP_LOAD_FLAG e);
//     virtual bool  LoadFromData(byte* pData, int nSize, RENDER_BITMAP_LOAD_FLAG e);
    
//     virtual eImageType  GetImageType() { return IMAGE_ITEM_TYPE_ICON; }

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

//     virtual eImageType  GetImageType() { return IMAGE_ITEM_TYPE_IMAGE_LIST; }
// private:
// 	IMAGELIST_LAYOUT_TYPE   m_eLayout;
// 	int     m_nCount;
// };
}

