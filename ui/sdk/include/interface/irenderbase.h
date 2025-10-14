#ifndef _UI_IRENDERBASE_H_
#define _UI_IRENDERBASE_H_
#include "sdk/include/interface.h"
#include "sdk/include/interface/imessage.h"
#include "sdk/include/util/rect.h"

#if 0 // defined(OS_WIN)
#include <uxtheme.h>
#endif

namespace ui {

struct IObject;
struct IColorRes;
struct IImageRes;
struct IRenderTarget;
struct Rect;

// 注：libo 20120927
//     每次光给render传递一个数值，里面无法只能用这个数值与图片项索引进行匹配，无法做更多的判断
//     因此决定扩展DrawState参数。低位为图片索引值，高位为状态值

#define RENDER_STATE_MASK 0xFFFF0000
#define RENDER_STATE_NORMAL 0x00010000
#define RENDER_STATE_HOVER 0x00020000
#define RENDER_STATE_PRESS 0x00040000
#define RENDER_STATE_DISABLE 0x00080000
#define RENDER_STATE_SELECTED 0x00100000
#define RENDER_STATE_DEFAULT 0x00200000
#define RENDER_STATE_READONLY 0x00400000
#define RENDER_STATE_NOTFOCUS 0x00800000 // 被选中了，但失去了焦点

const unsigned int LISTCTRLITEM_FOREGND_RENDER_STATE_NORMAL =
    RENDER_STATE_NORMAL | 0;
const unsigned int LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER =
    RENDER_STATE_HOVER | 1;
const unsigned int LISTCTRLITEM_FOREGND_RENDER_STATE_PRESS =
    RENDER_STATE_PRESS | 2;
const unsigned int LISTCTRLITEM_FOREGND_RENDER_STATE_DISABLE =
    RENDER_STATE_DISABLE | 3;
const unsigned int LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL =
    RENDER_STATE_NORMAL | RENDER_STATE_SELECTED | 4;
const unsigned int LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER =
    RENDER_STATE_HOVER | RENDER_STATE_SELECTED | 5;
const unsigned int LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS =
    RENDER_STATE_PRESS | RENDER_STATE_SELECTED | 6;
const unsigned int LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE =
    RENDER_STATE_DISABLE | RENDER_STATE_SELECTED | 7;
const unsigned int LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NOT_FOCUS =
    RENDER_STATE_SELECTED | RENDER_STATE_NOTFOCUS | 8;

const unsigned int LISTCTRLITEM_FOCUS_RENDER_STATE_NORMAL =
    RENDER_STATE_NORMAL | 0;
const unsigned int LISTCTRLITEM_FOCUS_RENDER_STATE_CTRLNOFOCUS =
    RENDER_STATE_NOTFOCUS | 1;

enum RENDER_TYPE {
  RENDER_TYPE_NULL,
  RENDER_TYPE_COLOR,
  RENDER_TYPE_GRADIENT,
  RENDER_TYPE_COLORLIST,

  RENDER_TYPE_IMAGE,
  RENDER_TYPE_IMAGELIST,
  RENDER_TYPE_IMAGELISTITEM,

  //	RENDER_TYPE_NOTHEME,    //
  //强制使用系统无主题样式的样式（win2000样式），目前仅GroupBox支持
  RENDER_TYPE_THEME_FIRST = 1000,
  //	RENDER_TYPE_THEME,
  RENDER_TYPE_THEME_TOOLTIP_WINDOW_BKGND,

  RENDER_TYPE_THEME_LAST = 9999,
};

class RenderBase;
struct UIAPI IRenderBase : public IMessage {
  void CheckThemeChanged();
  bool IsThemeRender();

  void SetObject(IApplication *pUIApp, IObject *pObject);
  IObject *GetObject();

  void SetType(RENDER_TYPE nType);
  RENDER_TYPE GetType();

  IColorRes *GetSkinColorRes();
  IImageRes *GetSkinImageRes();

  void Serialize(SerializeParam *pData);
  void DrawState(IRenderTarget *, const Rect *prc, int nState);
  Size GetDesiredSize();
  void Init();

  UI_DECLARE_INTERFACE(RenderBase);
};

// -- 2015.4.1过期，不再使用
// class NullRender;
// struct __declspec(uuid("4A0A8C42-CA22-4BD4-8875-C58FB8FC2788"))
// INullRender : public IRenderBase
// {
//     UI_DECLARE_INTERFACE(NullRender);
// };
//////////////////////////////////////////////////////////////////////////

} // namespace ui

#endif // _UI_IRENDERBASE_H_