#ifndef _UI_IIMAGERENDER_H_
#define _UI_IIMAGERENDER_H_
#include "irenderbase.h"

namespace UI
{
	interface IRenderBitmap;

enum BKCOLOR_FILL_TYPE
{
    BKCOLOR_FILL_ALL,    // 默认
    BKCOLOR_FILL_EMPTY   // 只在背景图没有绘制的地方进行填充。因为如果先fill all，再画透明位图的话，位图的背景色就被污染了。例如IM背景窗口皮肤图片与其背景色
};
class ImageRender;
interface UIAPI_UUID(6DAC8F58-390D-4660-A35F-2EBE956ED442) IImageRender
 : public IRenderBase
{
    void  SetAlpha(int nAlpha);
    int   GetAlpha();
    void  SetColor(Color c);
    Color GetColor();
    void  SetImageDrawType(int n);
    int   GetImageDrawType();
	void  SetImageStretch9Region(const C9Region& r);

    BKCOLOR_FILL_TYPE  GetBkColorFillType();
    void  SetBkColorFillType(BKCOLOR_FILL_TYPE eType);

    IRenderBitmap*  GetRenderBitmap();
    void  SetRenderBitmap(IRenderBitmap* pBitmap);

	UI_DECLARE_INTERFACE(ImageRender);
};

class ImageListItemRender;
interface UIAPI_UUID(51FFF758-737E-4252-BDBF-7DAF9DB261A6) IImageListItemRender
 : public IImageRender
{
    UI_DECLARE_INTERFACE(ImageListItemRender);
};

class ImageListRender;
interface IImageListRenderBitmap;
interface UIAPI_UUID(34CBE966-3ADE-49F9-98D5-79584248DBDB) IImageListRender
 : public IRenderBase
{
    void  SetIImageListRenderBitmap(IImageListRenderBitmap* pBitmap);
    IRenderBitmap*  GetRenderBitmap();
    void  SetImageDrawType(int n);
    int   GetImageDrawType();
    void  SetImageStretch9Region(C9Region* p);
    int   GetItemWidth();
    int   GetItemHeight();
    int   GetItemCount();

	UI_DECLARE_INTERFACE(ImageListRender);
};


}

#endif 