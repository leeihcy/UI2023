#pragma once
#include "src/private_inc.h"

namespace ui {
struct IImageResItem;
struct IRenderBitmap;
struct IImageRes;
class ImageData;
class GDIRenderBitmap;
class GdiplusRenderBitmap;
class Resource;

//
//	image中的一项信息数据
//
class ImageResItem {
public:
  ImageResItem();
  ~ImageResItem();

  bool ModifyImage(const char *szPath);
  IMAGE_ITEM_TYPE GetImageItemType();
  void SetImageItemType(IMAGE_ITEM_TYPE e);

  const char *GetId();
  void SetId(const char *text);
  void SetPath(const char *text);
  const char *GetPath();

  DECLARE_bool_SETGET(UseSkinHLS);
  DECLARE_bool_SETGET(NeedAntiAliasing)

      bool NeedDpiAdapt();
  void SetFileDpiScale(int n) { m_nFileDpiScale = (byte)n; }

public:
  bool ModifyHLS(short h, short l, short s, int nFlag);
  bool ModifyHLS(IRenderBitmap *pBitmap, short h, short l, short s, int nFlag);
  bool ModifyAlpha(byte nAlphaPercent);
  std::shared_ptr<IRenderBitmap>
  GetImage(Resource *pSkinRes,
           GRAPHICS_RENDER_LIBRARY_TYPE eRenderType =
               GRAPHICS_RENDER_LIBRARY_TYPE_GDI,
           bool *pbFirstTimeCreate = nullptr);
  std::shared_ptr<IRenderBitmap>
  GetSkiaImage(Resource *pSkinRes, bool *pbFirstTimeCreate = nullptr);

  bool IsMyRenderBitmap(IRenderBitmap *pRenderBitmap);

  // 创建Ixxx接口
  virtual IImageResItem *GetIImageResItem();
  // 读取xml属性，如imagelist的layout、count
  virtual void SetAttribute(IMapAttribute *pMapAttrib);
  // 在创建了RenderBitmap之后，由各个子类去为其设置扩展属性
  virtual void SetRenderBitmapAttribute(IRenderBitmap *pRenderBitmap);

protected:
  IImageResItem *m_pIImageResItem;

  std::string m_strId;   // image id
  std::string m_strPath; // image path
  byte m_nFileDpiScale;  // 图片dpi放大系数。@2x @3x ...

  bool m_bUseSkinHLS; // 该图片是否参与皮肤色调改变
  bool
      m_bNeedAntiAliasing; // 需要支持抗锯齿(gdi的alphablend缩放不支持SetStretchBltMode的HALFTONE)
  bool
      m_bMustHasAlphaChannel; // 该图片是否需要创建alpha channel。1. 有alpha
                              // channel的图片一率采用gdi创建，采用alphablend绘制。2.
                              // 没有alpha
                              // channel的gdi图片将无法在分层窗口上面绘制
  bool m_bDpiAdapt; // 是否自适应DPI缩放

  IMapAttribute
      *m_pMapAttrib; // 为该图片配置的属性，例如imagelist的count，icon的width
                     // height
  IMAGE_ITEM_TYPE m_eType; // 图片类型

  ImageData *m_pOriginImageData; // 该图片的原始数据（改变图片色调时使用）
#if 0                            // defined(OS_WIN)
  GDIRenderBitmap *m_pGdiBitmap; // 外部引用，该成员不增加引用计数
#endif
  //  注：为了提高绘图效率，同时也避免一张图片加载多种形式，尽量使用GDI(AlphaBlend)来绘图。
  //	    需要注意的是：如果需要使用alpha通道时，必须强制创建一个32位的GDI
  // BITMAP
  // GdiplusRenderBitmap*  m_pGdiplusBitmap;      // 外部引用

  std::shared_ptr<IRenderBitmap> m_render_bitmap;
};

class ImageListResItem : public ImageResItem {
public:
  ImageListResItem();

  virtual IImageResItem *GetIImageResItem();
  virtual void SetAttribute(IMapAttribute *pMapAttrib);
  virtual void SetRenderBitmapAttribute(IRenderBitmap *pRenderBitmap);

  IMAGELIST_LAYOUT_TYPE GetLayoutType();
  int GetItemCount();
  void SetLayoutType(IMAGELIST_LAYOUT_TYPE);
  void SetItemCount(int);

private:
  IMAGELIST_LAYOUT_TYPE m_eLayoutType;
  int m_nCount;
};

class ImageIconResItem : public ImageResItem {
public:
  ImageIconResItem();

  virtual IImageResItem *GetIImageResItem();
  virtual void SetAttribute(IMapAttribute *pMapAttrib);
  virtual void SetRenderBitmapAttribute(IRenderBitmap *pRenderBitmap);

private:
  Size m_sizeDraw;
};

//
//	image列表
//
class ImageRes {
public:
  ImageRes(Resource *pSkinRes);
  ~ImageRes();

  IImageRes &GetIImageRes();

  long GetImageCount();
  IImageResItem *GetImageResItem(long lIndex);
  IImageResItem *GetImageResItem(const char *szID);
  bool ModifyImage(const char *szId, const char *szPath);

  std::shared_ptr<IRenderBitmap>
  GetBitmap(const char *image_id, GRAPHICS_RENDER_LIBRARY_TYPE render_type);

  const char *GetRenderBitmapId(IRenderBitmap *pBitmap);

public:
  ImageResItem *LoadItem(const char *szType, IMapAttribute *pMapAttrib,
                         const char *szFullPath);

  ImageResItem *GetImageItem2(int nIndex);
  ImageResItem *GetImageItem2(const char *szId);

  ImageResItem *InsertImage(IMAGE_ITEM_TYPE eType, const char *szId,
                            const char *szPath);
  bool RemoveImage(const char *szId);
  bool RemoveImage(IImageResItem *pItem);
  bool Clear();
  bool ChangeSkinHLS(short h, short l, short s, int nFlag);
  bool ModifyImageItemAlpha(const std::string &strID, byte nAlphaPercent);

private:
  IImageRes *m_pIImageRes;
  Resource *m_pSkinRes;

  typedef std::map<std::string, ImageResItem *>::iterator _MyIter;
  std::map<std::string, ImageResItem *> m_mapImages;
  // vector<ImageResItem*>   m_vImages;   // vector的查找效率太低了，因此换用map
};

} // namespace ui