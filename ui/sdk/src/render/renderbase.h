#ifndef _UI_SDK_SRC_RENDER_RENDERBASE_H_
#define _UI_SDK_SRC_RENDER_RENDERBASE_H_
#include "include/interface/irenderbase.h"
#include "include/util/color.h"
#include "include/macro/uimsg.h"
#include "include/util/rect.h"
#include "src/object/message.h"

namespace ui {
struct IRenderBase;
struct IRenderBitmap;
class Application;
class ColorRes;
class ImageRes;
class ResourceBundle;
class Object;

const unsigned int WINDOW_BKGND_RENDER_STATE_ACTIVE = 0; // 窗口激活样式
const unsigned int WINDOW_BKGND_RENDER_STATE_INACTIVE = 1; // 窗口非激活样式

  //
  //	由于Render可以被用在很多地方，如背景、图标等等。为了区分设置的是哪个部分的Render属性，
  //	会在Render属性前面添加一个前缀，如bkgnd.render.type  icon.render.type
  //	在SetAttribute中，需要同时根据前缀+属性名来进行操作
  //

// 窗口背景绘制基类，用于实现各种样式窗口背景的绘制，如：平铺，居中，拉伸，9宫等
class RenderBase : public Message {
public:
  RenderBase(IRenderBase *p);
  virtual ~RenderBase(){};

  void onRouteMessage(ui::Msg *msg);

  void SetRenderType(const RENDER_TYPE &nType) { m_nRenderType = nType; }
  RENDER_TYPE GetRenderType() { return m_nRenderType; }

  ResourceBundle *GetResource();
  ColorRes *GetSkinColorRes();
  ImageRes *GetSkinImageRes();

  std::shared_ptr<IRenderBitmap> _LoadBitmap(const char *id);

  const char *_GetBitmapId(IRenderBitmap* pBitmap);

protected:
  ResourceBundle* m_resouce;
  IRenderBase *m_pIRenderBase;

  RENDER_TYPE m_nRenderType; // 自己的类型

protected:
  // 通用属性

  // 渲染内缩间距
  Rect m_deflate_margin = { 0 };
};

// 将bkgnd.render.type=null表示不使用默认背景
// class NullRender : public RenderBase
// {
// public:
// 	NullRender(INullRender* p) : RenderBase(p)
// 	{
// 		m_pINullRender = p;
// 	}

//     UI_DECLARE_RENDERBASE(NullRender, XML_RENDER_TYPE_NULL, RENDER_TYPE_NULL)

// 	UI_BEGIN_MSG_MAP()
//         UIMSG_QUERYINTERFACE(NullRender)
//     UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

//     INullRender*  m_pINullRender;
// };

} // namespace ui
#endif