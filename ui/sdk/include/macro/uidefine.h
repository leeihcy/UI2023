#ifndef _UIDEFINE_H_
#define _UIDEFINE_H_
#include "sdk/include/interface.h"

namespace ui {

// 特殊含义常量
enum {
  // 使用-1,-2,-3这些小值会有歧义。例如位置m_configRight刚好出了parent等于-1/-2，
  // 会导致这个特殊值被污染。因此将值改大。以后再加其它标识位
  WH_SET = 0, // 默认，指定
  NDEF = -1,  // 没有定义的变量
  AUTO = -2,  // 自动调整（如width/height=auto）
  WH_AUTO = AUTO,
  WH_AVG = -3,
  WH_PERCENT = -4
};

typedef void* TimerID;
struct IAnimateTimer {
  virtual void OnTick() = 0;
};

#define EMPTYTEXT ""

#define __pImpl static_cast<ImplName *>(m_pImpl)

#define UI_DECLARE_INTERFACE(T)                                                \
public:                                                                        \
  typedef T ImplName;                                                          \
  friend struct T##Meta;                                                       \
  static std::unique_ptr<I##T, void (*)(I##T *)> create(ui::IResourceBundle *);      \
  I##T(ui::eCreateImpl);                                                       \
  void onRouteMessage(ui::Msg *msg);                                           \
  static Uuid UUID();                                                          \
  T *GetImpl();

// 跨模块时，作为本模块的根对象，需要增加成员变量及销毁该变量
#define UI_DECLARE_INTERFACE_ACROSSMODULE(T)                                   \
  UI_DECLARE_INTERFACE(T)                                                      \
  virtual ~I##T();                                                             \
                                                                               \
protected:                                                                     \
  T *m_pImpl;

// 这里采用operator new的原因：
//   如果采用m_pImpl = new T(this);
//   那么在T构造函数中调用Ixxx类的方法时，Ixxx的m_pImpl还没有
//   被赋值。因此将该操作分解成两步：malloc + construct
#define UI_IMPLEMENT_INTERFACE(T, SUPER)                                       \
  I##T::I##T(ui::eCreateImpl b) : I##SUPER(ui::eCreateImpl::No_) {           \
    if (b == ui::eCreateImpl::Yes_) {                                          \
      m_pImpl = (T *)operator new(sizeof(T));                                  \
      new (m_pImpl) T(this);                                                   \
    } else {                                                                   \
      m_pImpl = nullptr;                                                       \
    }                                                                          \
  }                                                                            \
  T *I##T::GetImpl() { return static_cast<T *>(m_pImpl); }                     \
  std::unique_ptr<I##T, void (*)(I##T *)> I##T::create(IResourceBundle *res) {       \
    return T##Meta::Get().CreateUnique(res);                                   \
  }                                                                            \
  Uuid I##T::UUID() { return T##Meta::Get().UUID(); }                          \
  void I##T::onRouteMessage(ui::Msg *msg) { __pImpl->onRouteMessage(msg); }

#define UI_IMPLEMENT_INTERFACE_ACROSSMODULE(T, SUPER)                          \
  I##T::~I##T() {                                                              \
    if (m_pImpl) {                                                             \
      delete m_pImpl;                                                          \
      m_pImpl = nullptr;                                                       \
    }                                                                          \
  }                                                                            \
  I##T::I##T(ui::eCreateImpl b) : I##SUPER(ui::eCreateImpl::Yes_) {            \
    if (b == eCreateImpl::Yes_) {                                              \
      m_pImpl = (T *)operator new(sizeof(T));                                  \
      new (m_pImpl) T(this);                                                   \
    } else {                                                                   \
      m_pImpl = nullptr;                                                       \
    }                                                                          \
  }                                                                            \
  T *I##T::GetImpl() { return static_cast<T *>(m_pImpl); }                     \
  std::unique_ptr<I##T, void (*)(I##T *)> I##T::create(IResourceBundle *res) {       \
    return T##Meta::Get().CreateUnique(res);                                   \
  }                                                                            \
  Uuid I##T::UUID() { return T##Meta::Get().UUID(); }                          \
  void I##T::onRouteMessage(ui::Msg *msg) {                                    \
    static_cast<T *>(m_pImpl)->onRouteMessage(msg);                            \
  }

// inline  int RECTW(const Rect* prc) { return prc->right - prc->left; }
// inline  int RECTH(const Rect* prc) { return prc->bottom - prc->top; }
// inline  int RECTW(Rect& rc)  { return rc.right - rc.left; }
// inline  int RECTH(Rect& rc)  { return rc.bottom - rc.top; }
//
// #define RGBA(r,g,b,a)  (((BYTE)(r))|((WORD)(((BYTE)(g))<<8))|(unsigned
// int(((BYTE)(b))<<16))|((unsigned int)(((BYTE)(a))<<24)))

// 是否处理返回值
enum HANDLED_VALUE {
  NOT_HANDLED = 0,
  HANDLED = 1,
};

class UIAPI IRootInterface {
public:
  virtual ~IRootInterface(){}; // 确保delete时能调用到派生类的析构函数
};

// 编辑器的一切功能不好实现，只能将一些代码加进UISDK工程
// 这些代码都用这个宏包起来，在release模式下面不启用
#ifdef _DEBUG
#define EDITOR_MODE
#endif

// 对象序列化消息。用于取代WM_SETATTRIBUTE
enum SERIALIZEFLAG {
  SERIALIZEFLAG_LOAD = 0x01,
  SERIALIZEFLAG_RELOAD = 0x03, // reload也是load
  SERIALIZEFLAG_SAVE = 0x04,
  SERIALIZEFLAG_EDITOR = 0x08, // 获取属性列表和提示信息

  // load 标识
  SERIALIZEFLAG_LOAD_ERASEATTR =
      0x0200, // 获取到属性后，将该属性从mapattr中删除

  // save 标识
};
struct IAttributeMap;
struct IAttributeList;
struct IAttributeEditorProxy;
struct IApplication;
struct IResourceBundle;

struct SerializeParam {
  union {
    IAttributeMap *attribute_map;                    // load [in] / getlist [out]
    IAttributeList *attribute_list;                  // save [out]
    IAttributeEditorProxy *attribute_editor_proxy;   // editor [in]
  };

  // IApplication *pUIApplication; // TODO: 废弃该变量，只使用pSkinRes
  IResourceBundle *resource;
  const char *szPrefix;    // 属性前缀
  const char *szParentKey; // 父属性（仅用于editor），如bkg.render.type
  unsigned int nFlags;

  bool IsReload() {
    return ((nFlags & SERIALIZEFLAG_RELOAD) == SERIALIZEFLAG_RELOAD) ? true
                                                                     : false;
  }
  bool IsLoad() {
    return ((nFlags & SERIALIZEFLAG_LOAD) == SERIALIZEFLAG_LOAD) ? true : false;
  }
  bool IsSave() {
    return ((nFlags & SERIALIZEFLAG_SAVE) == SERIALIZEFLAG_SAVE) ? true : false;
  }
  bool IsEditor() {
    return ((nFlags & SERIALIZEFLAG_EDITOR) == SERIALIZEFLAG_EDITOR) ? true
                                                                     : false;
  }
  bool NeedErase() {
    return (nFlags & SERIALIZEFLAG_LOAD_ERASEATTR) ? true : false;
  }
  void SetErase(bool b) {
    if (b) {
      nFlags |= SERIALIZEFLAG_LOAD_ERASEATTR;
    } else {
      nFlags &= ~SERIALIZEFLAG_LOAD_ERASEATTR;
    }
  }
};

// object的state bit，状态位
enum OBJECT_STATE_BIT {
  OSB_UNVISIBLE = 0x0001, // 自己是否可见
  OSB_COLLAPSED =
      0x0002, // listitem为收起来，不显示子结点（TODO:以后可用于扩展为隐藏并且不占用布局?）
  OSB_DISABLE = 0x0004, // 自己是否可用

  OSB_PRESS = 0x0010,
  OSB_HOVER = 0x0020,
  OSB_FORCEPRESS = 0x0040,
  OSB_FORCEHOVER = 0x0080,
  OSB_READONLY = 0x0100, // 还是由各个控件自己实现更好一些。
  OSB_FOCUS = 0x0200,
  OSB_DEFAULT = 0x0400,
  OSB_SELECTED = 0x0800, // 被选中
  OSB_CHECKED = 0x1000,
  OSB_RADIOCHECKED = 0x2000,
  OSB_DRAGDROPHOVER = 0x4000, // 拖拽过程中，位于鼠标下。目前仅listitem使用
  OSB_DRAGING = 0x8000,  // 正在被拖拽
  OSB_EDITING = 0x10000, // 正在被编辑

  WSB_ACTIVE = 0x1000, // 窗口为active
};

#define SWP_LAYEREDWINDOW_SIZEMOVE                                             \
  0x80000000 // 表示这是个分层窗口模拟的窗口大小改变
#define SWP_NOUPDATELAYOUTPOS 0x40000000 // 调用SetObjectPos时不更新布局属性
#define SWP_FORCESENDSIZEMSG                                                   \
  0x20000000 // 即使大小没有改变，也强制发送一个WM_SIZE消息，用于走通逻辑
#define SWP_FORCEUPDATEOBJECT                                                  \
  0x10000000 // 即使大小没有改变，也强制柠檬素，用于走通逻辑
// #define SWP_NO_MOVE 1
// #define SWP_NO_SIZE 2
// #define SWP_NO_ZORDER 4
// #define SWP_NO_REDRAW 8
// #define SWP_NO_SENDCHANGING 0x10

#define UI_DECLARE_TEXTRENDERBASE(className, xml, rendertype)                  \
  static const char *GetXmlName() { return xml; }                              \
  static int GetType() { return rendertype; }

#define UI_DECLARE_TEXTRENDERBASE2(className, xml, rendertype)                 \
  static const char *GetXmlName() { return xml; }                              \
  static int GetType() { return rendertype; }

#define TESTWNDSTYLE(hWnd, flag) GetWindowLongPtr(hWnd, GWL_STYLE) & flag
#define ADDWNDSTYLE(hWnd, flag)                                                \
  SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) | flag)
#define REMOVEWNDSTYLE(hWnd, flag)                                             \
  SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) & ~flag)

// 窗口类名称
#define WND_CLASS_NAME _T("ui")
#define WND_ANIMATE_CLASS_NAME _T("UI_Animate")
#define WND_POPUP_CONTROL_SHADOW_NAME                                          \
  _T("UI_PopupControlShadow") // 带系统阴影功能
#define WND_POPUP_CONTROL_NAME _T("UI_PopupControl")
#define WND_DRAGBITMAPWND_CLASS_NAME _T("UI_DragBitmapWnd")

// 窗口创建完成后，获取xml中的控件
#define INIT_CONTROL(x, id) x = (decltype(x))m_pWindow->FindObject(id);
#define INIT_CONTROL2(x, id) (decltype(x))m_pWindow->FindObject(id);
#define TRY_INIT_CONTROL(x, id) x = (decltype(x))m_pWindow->TryFindObject(id);

// 单例类。放在类里的最后面声明
#define SINGLE_INSTANCE(classname)                                             \
public:                                                                        \
  static classname &Get() {                                                    \
    static classname s;                                                        \
    return s;                                                                  \
  }


} // namespace ui

#endif // _UIDEFINE_H_