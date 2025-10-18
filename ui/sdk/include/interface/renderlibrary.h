#ifndef _UI_RENDERLIBRARY_H_
#define _UI_RENDERLIBRARY_H_
#include "ui/sdk/include/macro/xmldefine.h"
#include "ui/sdk/include/util/color.h"
#include "ui/sdk/include/util/rect.h"
#include "ui/sdk/include/util/rect_region.h"
#include "ui/sdk/include/util/struct.h"
#include "ui/sdk/include/common/signalslot/slot.h"

#include <memory>
#include <string>
#include <shared_mutex>

namespace ui {
struct IRenderFont;
struct IRenderPen;
struct IRenderTarget;
class ImageData;
struct IGpuLayer;
struct Rect;
struct Point;
struct Size;
struct LOGFONT;
struct FontDesc;
struct IGpuCompositor;

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
    // scale_factor = 1.0f;
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
  C9Region nine_region; // 不需要拉伸时，不使用
  unsigned char nAlpha;

  // float scale_factor;

  // out param (废弃。多线程渲染后已获取不到，外部调用者自己计算吧。)
  // Rect *prcRealDraw; // 图片真正绘制的区域。当prcRealDraw不为空时表示需要获取

} DRAWBITMAPPARAM, *LPDRAWBITMAPPARAM;

struct DrawTextEffects {};

struct FontDesc {
  std::string face;
  int size = 0;
  int weight = 0;
  int orientation = 0;
  union {
    uint32_t style = 0;
    struct {
      bool italic : 1;
      bool underline : 1;
      bool strikeout : 1;
      bool cleartype : 1;
      bool subpixel : 1;
    };
  };
  bool operator==(const FontDesc &o) const {
    return face == o.face && size == o.size && weight == o.weight &&
           orientation == o.orientation && style == o.style;
  }

  size_t hash() const {
    size_t seed = 0;
    seed ^=
        std::hash<std::string>()(face) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<unsigned short>()(size) + 0x9e3779b9 + (seed << 6) +
            (seed >> 2);
    seed ^= std::hash<unsigned short>()(weight) + 0x9e3779b9 + (seed << 6) +
            (seed >> 2);
    seed ^= std::hash<unsigned short>()(orientation) + 0x9e3779b9 +
            (seed << 6) + (seed >> 2);
    seed ^= std::hash<uint32_t>()(style);
    return seed;
  }
};

struct DrawTextParam {
  DrawTextParam() {
    effects = 0;
    color.value = 0xff000000;
    text = nullptr;
    bound.SetEmpty();
    multiline = false;
    align = 0;

    // bkcolor.a = 0xff000000;
  }

  FontDesc font_desc;

  const char *text;
  Rect bound; // 绘制范围
  ui::Color color;

  int align; // ALIGN_TYPE
  bool multiline;

  int effects; // 特效标志

  // 特效中可能会使用到的参数
  // Color bkcolor;
};

enum TEXT_EFFECT {
  TEXT_EFFECT_NONE = 0,
  TEXT_EFFECT_HALO = 1, // 光圈效果 bkcolor指定光晕颜色。wParam高斯模糊半径系统
  TEXT_EFFECT_ENDALPHAMASK =
      2, // 当指定区域显示不下时，将文本的末尾进行虚化。避免使用...这种形式体验不是很好。
};

struct Render2TargetParam {
  int xDst;
  int yDst;
  unsigned int wDst;
  unsigned int hDst;
  int xSrc;
  int ySrc;
  unsigned int wSrc;
  unsigned int hSrc;
  bool bAlphaBlend;
  unsigned char opacity;

  // pTransfrom2d和pTransform3d中只有一个有效
  // 	LPMATRIX33  pTransform2d;   // 2d变换
  // 	LPMATRIX44  pTransform3d;   // 3d变换
};

struct IRenderResource {
  virtual ~IRenderResource(){};
  virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() = 0;
  //   virtual void SetOutRef(IRenderResource **ppOutRef) = 0;
  //   virtual long AddRef() = 0;
  //   virtual long Release() = 0;
};

enum RENDER_BITMAP_LOAD_FLAG {
  RENDER_BITMAP_LOAD_FLAG_NONE = 0,
  RENDER_BITMAP_LOAD_CREATE_ALPHA_CHANNEL = 1,
  RENDER_BITMAP_LOAD_DPI_ADAPT = 2,

  // dpi 文件已经被放大的系数存在高8位上面，例如加载一张@2x的图片时，标志位为
  // (2 << 24) | DPI_ADAPT
};
struct IRenderBitmap : public IRenderResource {
  virtual bool LoadFromFile(const char *szPath, RENDER_BITMAP_LOAD_FLAG e) = 0;
  virtual bool LoadFromData(unsigned char *pData, int nSize,
                            RENDER_BITMAP_LOAD_FLAG e) = 0;

  virtual IMAGE_ITEM_TYPE GetImageType() = 0;
  virtual bool Create(int nWidth, int nHeight) = 0;
  virtual void Destroy() = 0;
  virtual int GetWidth() = 0;
  virtual int GetHeight() = 0;
  virtual int GetBPP() = 0;
#if 0
  virtual Color GetAverageColor() { return Color::Make(0); }

  // virtual void Attach(HBITMAP /*hBitmap*/, bool /*bDelete*/){};
  // virtual HBITMAP Detach() { return 0; }

  virtual unsigned char *LockBits() = 0;
  virtual void UnlockBits() = 0;

  virtual bool SaveBits(ImageData *pImageData) = 0;
  virtual bool ChangeHSL(const ImageData *pOriginImageData, short h, short s,
                         short l, int nFlag) = 0;
#endif
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
  static std::shared_ptr<IRenderBitmap>
  CreateInstance(IApplication *pUIApp,
                 GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType,
                 IMAGE_ITEM_TYPE eType);
};

#define FONTITEM_FLAG_UNDERLINE 0x01
#define FONTITEM_FLAG_BOLD 0x02
#define FONTITEM_FLAG_ITALIC 0x04
#define FONTITEM_FLAG_STRIKEOUT 0X08

struct IRenderPen : public IRenderResource {
public:
  virtual bool CreateSolidPen(int nWidth, Color *pColor) = 0;
  virtual bool CreateDotPen(int nWidth, Color *pColor) = 0;
};

struct IRenderBrush : public IRenderResource {
public:
  virtual bool CreateSolidBrush(Color *pColor) = 0;
};


// 渲染好的帧缓存数据，可用于主线程窗口提交渲染。
// 其实是对应于front render target的buffer数据。
struct FrameBuffer {
  // software
  int width = 0;
  int height = 0;
  const void *data = nullptr;
  int rowbytes = 0;

  // hardware
  std::shared_ptr<IGpuLayer> gpu_layer;
};

struct FrameBufferWithReadLock : public FrameBuffer {
  std::shared_lock<std::shared_mutex> read_lock;
};

struct IClipOrigin {
  virtual void SetDirtyRegion(const DirtyRegion& dirty_region) = 0;
  virtual void PushRelativeClipRect(const Rect& rect) = 0;
  virtual void PopRelativeClipRect() = 0;

  virtual void SetOrigin(int x, int y) = 0;
  virtual void OffsetOrigin(int x, int y) = 0;

  virtual bool IsRelativeRectInClip(const Rect& rect) = 0;
};

//
// IRenderTarget的几种渲染方式：
// 1. 调用BindHDC，将目标绘制在该HDC上面
// 2. 调用CreateRenderBuffer，将目标绘制在自己的内部缓存当中
//
struct IRenderTarget : public IClipOrigin {
  virtual ~IRenderTarget(){};
  virtual void Release() = 0; // delete this;

  virtual GRAPHICS_RENDER_LIBRARY_TYPE Type() = 0;

  virtual bool BeginDraw(float scale) = 0;
  virtual void EndDraw() = 0;
  virtual void Clear(const Rect& rect) = 0;
  virtual bool Resize(unsigned int nWidth, unsigned int nHeight) = 0;
  virtual void* GetHandle() = 0;

  virtual void Save() = 0;
  virtual void Restore() = 0;
  virtual void ClipRoundRect(const Rect& rect, const CornerRadius& radius) = 0;
  virtual void ClipRect(const Rect& rect) = 0;

  virtual void CreateSwapChain(bool is_hardware, IGpuCompositor* ) = 0;
  virtual bool SwapChain(slot<void()>&& callback) = 0;
  virtual void DumpToImage(const char *szPath) = 0;
  virtual bool GetFrontFrameBuffer(FrameBufferWithReadLock* fb) = 0;

  virtual void RenderOnThread(slot<void(IRenderTarget*)>&& callback) = 0;
  virtual void Render2Target(IRenderTarget *pDst,
                             Render2TargetParam *pParam) = 0;

  virtual void FillRect(const Rect &rc, const Color &color) = 0;
  virtual void StrokeRect(const Rect &rc, const Color &color, int width) = 0;
  virtual void FillRoundRect(const Rect &rc, const Color &color,
                             const CornerRadius &radius) = 0;
  virtual void StrokeRoundRect(const Rect &rc, const Color &color,
                               const CornerRadius &radius, int width) = 0;
  // 使用shared_ptr，用于多线程传递参数。
  virtual void DrawBitmap(std::shared_ptr<IRenderBitmap>, DRAWBITMAPPARAM *pParam) = 0;
  virtual void DrawString(const DrawTextParam &param) = 0;

  virtual void _DrawString2(void* text_blob, const Color& color, float x, float y) = 0;

#if 0
  virtual bool CreateRenderBuffer(IRenderTarget *pSrcRT) = 0;
  virtual void GetRenderBufferData(ImageData *pData) = 0;

  // 获取原生指针进行绘制。如HDC, SkCanvas
  virtual void* GetHandle() = 0;
#endif
};

struct IUICursor {
  virtual long AddRef() = 0;
  virtual long Release() = 0;

  // virtual HCURSOR GetCursor() = 0;
};

} // namespace ui

#endif // _UI_RENDERLIBRARY_H_