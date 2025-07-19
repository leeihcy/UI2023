#ifndef _UI_SDK_INCLUDE_INTERFACE_IMETA_H_
#define _UI_SDK_INCLUDE_INTERFACE_IMETA_H_
#include "sdk/include/macro/msg.h"
#include "sdk/include/util/rect.h"
namespace ui {
struct Msg;
struct IMessage;

enum OBJ_TYPE {
  OBJ_UNKNOWN = 0,
  OBJ_CONTROL = 1,
  OBJ_HWNDHOST = 2,
  OBJ_PANEL = 3,
  OBJ_ROOT = 4,
  OBJ_WINDOW = 5,
  RENDER_BASE = 6,
};
enum CONTROL_TYPE {
  // Extent Type
  CONTROL_UNKNOWN = 0,
  CONTROL_BUTTON = 1,
  CONTROL_GROUPBOX = 2,
  CONTROL_CHECKBUTTON = 3,
  CONTROL_RADIOBUTTON = 4,
  CONTROL_HYPERLINK = 5,
  CONTROL_LABEL = 6,
  CONTROL_PICTURE = 7,
  CONTROL_EDIT = 8,
  CONTROL_PROGRESSBAR = 9,
  CONTROL_SLIDERCTRL = 10,
  CONTROL_LISTBOX = 11,
  CONTROL_POPUPLISTBOX = 12,
  CONTROL_HEADERCTRL = 13,
  CONTROL_LISTVIEWCTRL = 14,
  CONTROL_HSCROLLBAR = 15,
  CONTROL_VSCROLLBAR = 16,
  CONTROL_COMBOBOX = 17,
  CONTROL_RICHEDIT = 18,
  CONTROL_LEDCTRL = 19,
  CONTROL_GIFCTRL = 20,
  CONTROL_MENU = 21,
  //    CONTROL_TOOLTIPWINDOW= 22, -> WINDOW_TOOLTIP
  CONTROL_TREEVIEWCTRL = 23,
  CONTROL_LISTTREEVIEWCTRL = 24,
  CONTROL_FLASH = 25,
  CONTROL_IE = 26,
  CONTROL_LISTCTRLITEM = 27,
  CONTROL_TABCTRL = 28,
  CONTROL_SPLITTERBAR = 29,
  CONTROL_CEF = 30,
  CONTROL_RICHTEXT = 31,
  CONTROL_SVG = 32,
};

enum WINDOW_TYPE {
  WINDOW_SIMPLE = 0,
  WINDOW_CUSTOM = 1,
  WINDOW_TOOLTIP = 2,
};

enum PANEL_TYPE {
  PANEL_SIMPLE = 0,
  PANEL_SCROLL = 1,
  PANEL_LISTCTRLITEMROOT = 2,
  PANEL_STAGE3D = 3,
};

enum LAYER_TYPE {
  LAYER_DIRECT = 1,
  LAYER_BUFFER = 2,
  LAYER_3D = 3,
};

enum RENDER_BASE_TYPE {
  RENDER_BASE_IMAGE,
};

enum E_BOOL_CREATE_IMPL {
  CREATE_IMPL_FALSE = 0,
  CREATE_IMPL_TRUE = 1,
};

#define CATEGORY_CONTROL  "Control";
#define CATEGORY_CONTAINER "Container";
#define CATEGORY_WINDOW "Window";

// meta 类的作用：
// 1. 静态成员变量，如UUID
// 2. 属性定义，如给编辑器定义的一些参数
// 3. 作为触达最外层继承链的桥梁，如在message/object中要调用实际对象的方法时，可以走meta
//    meta没有中间环节，直达最外层的对象。

struct MetaDetail {
  long major_type;  // OBJ_TYPE
  long minor_type;
  const char* category;
  const char* descrption;
  Size default_size;
  // TODO:
  // icon
};

struct IMeta {
  // 创建、销毁。做成通用型参数，用于overide
  virtual void Create(ui::IResource *p, void **) = 0;
  virtual void Destroy(void*) = 0;

  // 静态成员
  virtual Uuid UUID() = 0; 

  // 直接给最终对象发消息，取代之前的 object creator 类
  virtual void RouteMessage(IMessage*, Msg*) = 0;
  // 兼容
  // virtual bool virtualProcessMessage(UIMSG *pMsg, int nMsgMapID, bool bDoHook) = 0;

  // 对应xml中的名字
  virtual const char* Name() = 0;

  // 编辑器参数
  virtual MetaDetail Detail() = 0;
};

template<class Ixx>
struct MetaImpl : public IMeta {
  using IxxPtr = std::unique_ptr<Ixx, void (*)(Ixx*)>;
  using This = MetaImpl<Ixx>;

  IxxPtr create(IResource *resource) {
    Ixx *p = new Ixx(E_BOOL_CREATE_IMPL::CREATE_IMPL_TRUE);

    FinalConstructMessage msg;
    msg.resource = resource;
    msg.meta = static_cast<IMeta*>(this);
    
    p->onRouteMessage(&msg);
    if (!msg.success) {
      delete p;
      return IxxPtr(nullptr, This::destroy);
    }
    return IxxPtr(p, This::destroy);
  }

  static void destroy(Ixx* p) {
    if (!p) return;
  
    FinalReleaseMessage msg;
    p->onRouteMessage(&msg);

    delete p;  
  }
  
  void Create(IResource *p, void **pp) override {
    *pp = This::create(p).release();
  }
  void Destroy(void* obj) override {
    This::destroy((Ixx*)obj);
  }

  void RouteMessage(IMessage* obj, Msg* msg) override {
    static_cast<Ixx*>(obj)->onRouteMessage(msg);
  }
  // 兼容老版本的消息映射
  // bool virtualProcessMessage(UIMSG *pMsg, int nMsgMapID, bool bDoHook) override {
  //   return static_cast<Ixx*>(pMsg->pMsgTo)->nvProcessMessage(pMsg, nMsgMapID, bDoHook);
  // }
};

} // namespace ui
#endif