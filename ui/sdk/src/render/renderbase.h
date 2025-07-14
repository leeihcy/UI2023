#ifndef _UI_SDK_SRC_RENDER_RENDERBASE_H_
#define _UI_SDK_SRC_RENDER_RENDERBASE_H_
#include "include/interface/irenderbase.h"
#include "include/util/color.h"
#include "include/macro/uimsg.h"
#include "src/object/message.h"

namespace ui {
struct IRenderBase;
struct IRenderBitmap;
class Application;
class ColorRes;
class ImageRes;
class Resource;
class Object;

const unsigned int WINDOW_BKGND_RENDER_STATE_ACTIVE = 0; // 窗口激活样式
const unsigned int WINDOW_BKGND_RENDER_STATE_INACTIVE = 1; // 窗口非激活样式

// 窗口背景绘制基类，用于实现各种样式窗口背景的绘制，如：平铺，居中，拉伸，9宫等
class RenderBase : public Message {
public:
  RenderBase(IRenderBase *p);
  virtual ~RenderBase(){};

  //
  //	由于Render可以被用在很多地方，如背景、图标等等。为了区分设置的是哪个部分的Render属性，
  //	会在Render属性前面添加一个前缀，如bkgnd.render.type  icon.render.type
  //	在SetAttribute中，需要同时根据前缀+属性名来进行操作
  //
  //	另外IRender不需要提供RetAttribute操作，因为Object在Reset的时候，一般是delete
  //irender 	因此这里也没有添加 bool bReload 参数
  //
  bool IsThemeRender() {
    return m_nRenderType > RENDER_TYPE_THEME_FIRST &&
           m_nRenderType < RENDER_TYPE_THEME_LAST;
  }
  void CheckThemeChanged();

  void SetObject(Application *pUIApp, Object *pObject) {
    m_pUIApplication = pUIApp;
    m_pObject = pObject;
  }
  Object *GetObject() { return m_pObject; }

  void SetRenderType(const RENDER_TYPE &nType) { m_nRenderType = nType; }
  RENDER_TYPE GetRenderType() { return m_nRenderType; }

  Resource *GetResource();
  ColorRes *GetSkinColorRes();
  ImageRes *GetSkinImageRes();

  void _LoadColor(const char *szColorId, Color *&pColorRef);
  std::shared_ptr<IRenderBitmap> _LoadBitmap(const char *id);

  const char *_GetColorId(Color *&pColorRef);
  const char *_GetBitmapId(IRenderBitmap* pBitmap);

protected:
  Application *m_pUIApplication;
  IRenderBase *m_pIRenderBase;

  Object *m_pObject;         // 绑定的对象，要绘制谁的背景
  RENDER_TYPE m_nRenderType; // 自己的类型
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