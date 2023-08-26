#ifndef _UI_IIMAGERENDER_H_
#define _UI_IIMAGERENDER_H_
#include "irenderbase.h"
#include "sdk/include/util/color.h"

namespace ui {
struct IRenderBitmap;
class C9Region;

enum BKCOLOR_FILL_TYPE {
  BKCOLOR_FILL_ALL, // 默认
  BKCOLOR_FILL_EMPTY // 只在背景图没有绘制的地方进行填充。因为如果先fill
                     // all，再画透明位图的话，位图的背景色就被污染了。例如IM背景窗口皮肤图片与其背景色
};
class ImageRender;
struct UIAPI IImageRender : public IRenderBase {
  void SetAlpha(int nAlpha);
  int GetAlpha();
  void SetColor(Color c);
  Color GetColor();
  void SetImageDrawType(int n);
  int GetImageDrawType();
  void SetImageStretch9Region(const C9Region &r);

  BKCOLOR_FILL_TYPE GetBkColorFillType();
  void SetBkColorFillType(BKCOLOR_FILL_TYPE eType);

  IRenderBitmap *GetRenderBitmap();
  void SetRenderBitmap(IRenderBitmap *pBitmap);

  static Uuid UUID() { return Uuid("6DAC8F58-390D-4660-A35F-2EBE956ED442"); }
  UI_DECLARE_INTERFACE(ImageRender);
};

class ImageListItemRender;
struct UIAPI IImageListItemRender : public IImageRender {
  static Uuid UUID() { return Uuid("51FFF758-737E-4252-BDBF-7DAF9DB261A6"); }
  UI_DECLARE_INTERFACE(ImageListItemRender);
};

class ImageListRender;
struct IImageListRenderBitmap;
struct UIAPI IImageListRender : public IRenderBase {
  void SetIImageListRenderBitmap(IImageListRenderBitmap *pBitmap);
  IRenderBitmap *GetRenderBitmap();
  void SetImageDrawType(int n);
  int GetImageDrawType();
  void SetImageStretch9Region(C9Region *p);
  int GetItemWidth();
  int GetItemHeight();
  int GetItemCount();

  static Uuid UUID() { return Uuid("34CBE966-3ADE-49F9-98D5-79584248DBDB"); }
  UI_DECLARE_INTERFACE(ImageListRender);
};

} // namespace ui

#endif