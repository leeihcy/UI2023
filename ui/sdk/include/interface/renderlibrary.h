#ifndef _UI_RENDERLIBRARY_H_
#define _UI_RENDERLIBRARY_H_
#include "sdk/include/util/color.h"

namespace ui {
struct IRenderFont;
struct IRenderPen;
struct IRenderTarget;
class ImageData;
struct IRenderFont;
struct IGpuRenderLayer;
struct Rect;

class C9Region;

// 绘制图片的统一参数，避免需要重写多个DrawBitmap函数
enum DRAW_BITMAP_FLAG {
  // 状态位
  DRAW_BITMAP_DISABLE = 0x0001,

  // 类型位
  DRAW_BITMAP_BITBLT = 0x00010000,
  DRAW_BITMAP_STRETCH = 0x00020000,
  DRAW_BITMAP_TILE = 0x00040000,
  DRAW_BITMAP_CENTER = 0x00080000,
  DRAW_BITMAP_ADAPT = 0x00100000,
  DRAW_BITMAP_STRETCH_BORDER = 0x00200000,  // 只拉伸边框，不绘制中间
  DRAW_BITMAP_BITBLT_RIGHTTOP = 0x00400000, // 将图片绘制在右上角对齐
  DRAW_BITMAP_BITBLT_LEFTBOTTOM = 0x00800000, // 将图片绘制在右下角对齐
  DRAW_BITMAP_BITBLT_RIGHTBOTTOM = 0x01000000, // 将图片绘制在右下角对齐
  DRAW_BITMAP_BITBLT_LEFTVCENTER = 0x02000000, // 将图片绘制在左中对齐
  DRAW_BITMAP_STRETCH_ROUNDCORNER = 0x04000000, // 圆角拉伸
  DRAW_BITMAP_STRETCH_DISABLE_AA =
      0x08000000, // 拉伸时不需要插值，如二维码像素的拉伸
};

typedef struct tagDRAWBITMAPPARAM {
  tagDRAWBITMAPPARAM() {
    memset(this, 0, sizeof(tagDRAWBITMAPPARAM));
    nAlpha = 255;
    nFlag = DRAW_BITMAP_BITBLT;
  }

  int nFlag;

  int xDest;
  int yDest;
  int wDest; // 目标绘制的范围，当不需要拉伸时，不使用
  int hDest; // 目标绘制的范围，当不需要拉伸时，不使用
  int xSrc;
  int ySrc;
  int wSrc;
  int hSrc;
  C9Region *pRegion; // 不需要拉伸时，不使用
  unsigned char nAlpha;

  // out param
  Rect *prcRealDraw; // 图片真正绘制的区域。当prcRealDraw不为空时表示需要获取

} DRAWBITMAPPARAM, *LPDRAWBITMAPPARAM;

typedef struct tagDRAWTEXTPARAM {
  tagDRAWTEXTPARAM() {
    nFormatFlag = 0;
    nEffectFlag = 0;
    color.a = 255;
    szText = nullptr;
    prc = nullptr;
    bkcolor.a = 255;
    wParam = lParam = 0;
  }

  int nFormatFlag; // 对齐标志，同DrawText flag
  int nEffectFlag; // 特效标志

  ui::Color color;
  const wchar_t *szText;
  const Rect *prc;

  // 特效中可能会使用到的参数
  Color bkcolor;
  long wParam;
  long lParam;
} DRAWTEXTPARAM, *LPDRAWTEXTPARAM;

enum TEXT_EFFECT {
  TEXT_EFFECT_NONE = 0,
  TEXT_EFFECT_HALO = 1, // 光圈效果 bkcolor指定光晕颜色。wParam高斯模糊半径系统
  TEXT_EFFECT_ENDALPHAMASK =
      2, // 当指定区域显示不下时，将文本的末尾进行虚化。避免使用...这种形式体验不是很好。
};

struct Render2TargetParam {
  int xDst;
  int yDst;
  int wDst;
  int hDst;
  int xSrc;
  int ySrc;
  int wSrc;
  int hSrc;
  bool bAlphaBlend;
  unsigned char opacity;

  // pTransfrom2d和pTransform3d中只有一个有效
  // 	LPMATRIX33  pTransform2d;   // 2d变换
  // 	LPMATRIX44  pTransform3d;   // 3d变换
};

struct IRenderResource {
  virtual ~IRenderResource(){};
  virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() = 0;
  virtual void SetOutRef(IRenderResource **ppOutRef) = 0;
  virtual long AddRef() = 0;
  virtual long Release() = 0;
};

enum RENDER_BITMAP_LOAD_FLAG {
  RENDER_BITMAP_LOAD_FLAG_NONE = 0,
  RENDER_BITMAP_LOAD_CREATE_ALPHA_CHANNEL = 1,
  RENDER_BITMAP_LOAD_DPI_ADAPT = 2,

  // dpi 文件已经被放大的系数存在高8位上面，例如加载一张@2x的图片时，标志位为
  // (2 << 24) | DPI_ADAPT
};
struct IRenderBitmap : public IRenderResource {
  virtual bool LoadFromFile(const wchar_t *szPath,
                            RENDER_BITMAP_LOAD_FLAG e) = 0;
  virtual bool LoadFromData(unsigned char *pData, int nSize,
                            RENDER_BITMAP_LOAD_FLAG e) = 0;

  virtual bool Create(int nWidth, int nHeight) = 0;
  virtual void Destroy() = 0;
  virtual int GetWidth() = 0;
  virtual int GetHeight() = 0;
  virtual int GetBPP() = 0;
  virtual COLORREF GetAverageColor() { return 0; }

  virtual void Attach(HBITMAP /*hBitmap*/, bool /*bDelete*/){};
  virtual HBITMAP Detach() { return 0; }

  virtual unsigned char *LockBits() = 0;
  virtual void UnlockBits() = 0;

  virtual bool SaveBits(ImageData *pImageData) = 0;
  virtual bool ChangeHSL(const ImageData *pOriginImageData, short h, short s,
                         short l, int nFlag) = 0;
  virtual IMAGE_ITEM_TYPE GetImageType() = 0;
};

struct IImageListRenderBitmap : public IRenderBitmap {
public:
  virtual int GetItemWidth() = 0;
  virtual int GetItemHeight() = 0;
  virtual int GetItemCount() = 0;
  virtual IMAGELIST_LAYOUT_TYPE GetLayoutType() = 0;
  virtual void SetItemCount(int) = 0;
  virtual void SetLayoutType(IMAGELIST_LAYOUT_TYPE) = 0;
  virtual bool GetIndexPos(int nIndex, Point *pPoint) = 0;
};

struct IImageIconRenderBitmap : public IRenderBitmap {
public:
  virtual Size GetDrawSize() = 0;
  virtual void SetDrawSize(Size *ps) = 0;
};

class RenderBitmapFactory {
public:
  static void CreateInstance(IApplication *pUIApp,
                             GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType,
                             IMAGE_ITEM_TYPE eType, IRenderBitmap **ppOut);
};

#define FONTITEM_FLAG_UNDERLINE 0x01
#define FONTITEM_FLAG_BOLD 0x02
#define FONTITEM_FLAG_ITALIC 0x04
#define FONTITEM_FLAG_STRIKEOUT 0X08

struct IRenderFont : public IRenderResource {
public:
  virtual bool Load(LOGFONT *plogfont) = 0;
  virtual bool ModifyFont(LOGFONT *plogfont) = 0;

  virtual void Attach(HFONT hFont) = 0;
  virtual HFONT Detach() = 0;
  virtual bool IsAttach() = 0;

  virtual unsigned int GetCaretHeight() = 0;
  virtual Size MeasureString(const wchar_t *szText, int nLimitWidth = -1) = 0;

  virtual HFONT GetHFONT() = 0;
  virtual bool GetLogFont(LOGFONT *plf) = 0;

  enum FontType {
    CREATE,
    ATTACH,
  };
};

struct IRenderPen : public IRenderResource {
public:
  virtual bool CreateSolidPen(int nWidth, Color *pColor) = 0;
  virtual bool CreateDotPen(int nWidth, Color *pColor) = 0;
};

struct IRenderBrush : public IRenderResource {
public:
  virtual bool CreateSolidBrush(Color *pColor) = 0;
};

//
// IRenderTarget的几种渲染方式：
// 1. 调用BindHDC，将目标绘制在该HDC上面
// 2. 调用CreateRenderBuffer，将目标绘制在自己的内部缓存当中
//
struct IRenderTarget {
  virtual ~IRenderTarget(){};
  virtual void Release() = 0; // delete this;

  virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() = 0;
  virtual bool IsRenderAlphaChannel() = 0;

  // virtual void BindHDC(HDC hDC) = 0;
  // virtual HDC GetBindHDC() = 0;
  virtual bool CreateRenderBuffer(IRenderTarget *pSrcRT) = 0;
  virtual bool ResizeRenderBuffer(unsigned int nWidth,
                                  unsigned int nHeight) = 0;
  virtual void GetRenderBufferData(ImageData *pData) = 0;
  // virtual HDC GetHDC() = 0;
  // virtual void ReleaseHDC(HDC hDC) = 0;
  // virtual void BindHWND(HWND hWnd) = 0; // 仅D2D有用

  virtual bool BeginDraw() = 0;
  virtual void EndDraw() = 0;
  virtual void Clear(Rect *prc) = 0;
  virtual void Save(const wchar_t *szPath) = 0;
  virtual void Render2DC(HDC hDC, Render2TargetParam *pParam) = 0;
  virtual void Render2Target(IRenderTarget *pDst,
                             Render2TargetParam *pParam) = 0;

  virtual void SetMetaClipRegion(Rect *prc, unsigned int nrcCount) = 0;
  virtual void PushRelativeClipRect(const Rect *) = 0;
  virtual void PopRelativeClipRect() = 0;
  virtual bool IsRelativeRectInClip(const Rect *) = 0;
  virtual void SetOrigin(int x, int y) = 0;
  virtual void OffsetOrigin(int x, int y) = 0;
  virtual void GetOrigin(int *px, int *py) = 0;

  virtual IRenderPen *CreateSolidPen(int nWidth, Color *pColor) = 0;
  virtual IRenderPen *CreateDotPen(int nWidth, Color *pColor) = 0;
  virtual IRenderBrush *CreateSolidBrush(Color *pColor) = 0;

  virtual void FillRgn(HRGN hRgn, ui::Color *pColor) = 0;
  virtual void DrawRect(Rect *lprc, ui::Color *pColor) = 0;
  virtual void TileRect(Rect *lprc, IRenderBitmap *) = 0;
  virtual void Rectangle(Rect *lprc, ui::Color *pColBorder, ui::Color *pColBack,
                         int nBorder, bool bNullBack) = 0;
  virtual void DrawFocusRect(Rect *lprc) = 0;
  virtual void DrawLine(int x1, int y1, int x2, int y2, IRenderPen *) = 0;
  virtual void DrawPolyline(Point *lppt, int nCount, IRenderPen *) = 0;
  virtual void GradientFillH(Rect *lprc, COLORREF colFrom, COLORREF colTo) = 0;
  virtual void GradientFillV(Rect *lprc, COLORREF colFrom, COLORREF colTo) = 0;
  virtual void BitBlt(int xDest, int yDest, int wDest, int hDest,
                      IRenderTarget *pSrcHDC, int xSrc, int ySrc,
                      unsigned int dwRop) = 0;
  virtual void ImageList_Draw(IRenderBitmap *, int x, int y, int col, int row,
                              int cx, int cy) = 0;
  virtual void DrawBitmap(IRenderBitmap *, DRAWBITMAPPARAM *pParam) = 0;
  virtual void DrawString(IRenderFont *pFont, DRAWTEXTPARAM *pParam) = 0;

  virtual void Upload2Gpu(IGpuRenderLayer *p, Rect *prcArray, int nCount) = 0;
};

struct IUICursor {
  virtual long AddRef() = 0;
  virtual long Release() = 0;

  virtual HCURSOR GetCursor() = 0;
};

} // namespace ui

#endif // _UI_RENDERLIBRARY_H_