#ifndef _UI_IOBJECT_H_
#define _UI_IOBJECT_H_
#include "sdk/include/inc.h"

namespace ui {
struct IWindow;
struct IResource;
struct IMeta;
struct ITextRenderBase;
struct IRenderFont;
struct IRenderBase;
struct ILayer;
struct IMapAttribute;
struct ILayoutParam;
struct LayerAnimateParam;

typedef struct tagObjStyle {
  bool initialized : 1; // 已初始化完成，接收过了init通知
  bool transparent : 1;
  bool
      float_on_parent_content : 1; // 在刷新该控件时，不仅仅需要重绘父对象背景(wm_erasebkgbkgnd)，还要重绘父对象内容(wm_paint)
  bool post_paint : 1;
  bool hscroll : 1;
  bool vscroll : 1;
  bool ncobject : 1;
  bool reject_all_mouse_msg : 1;
  bool reject_self_mouse_msg : 1;
  bool receive_dragdrop_event : 1;
  bool enable_ime : 1;
  bool zindex_overlap : 1;
  bool noclip : 1;
  bool
      clip_client : 1; // 绘制完对象的背景后，再绘制对象的client区域时，是否需要对client区域作剪裁。（一般的对象不做，但对于会滚动的listctrl则需要剪裁，否则滚动的内容会超出来覆盖背景
  bool tabstop : 1;
  bool layer; // 开启分层

  // 默认值字段。用于派生类覆盖父类的默认行为
  // 可以考虑专门为default也构造一个同样的tagObjStyle，用于对应每一个样式的默认值
  bool default_ncobject : 1; // scrollbar默认为ncobj
  bool
      default_reject_all_mouse_msg : 1; // 默认该控件就不接收鼠标消息。如panel/label
  bool default_reject_self_mouse_msg : 1;
  bool
      default_transparent : 1; // 该控件默认是透明的。默认所有的控件不透明，用于提升绘制效率
  bool default_tabstop : 1; // 默认该控件能否tabstop

} OBJSTYLE;
typedef struct tagObjState {
  unsigned char visibility_ : 2; // 0:collapsed 1:visible 2:hidden
  bool disable : 1;
  bool press : 1;
  bool hover : 1;
  bool force_press : 1;
  bool force_hover : 1;
  bool focus : 1;
  bool default_ : 1;
  bool selected : 1;
  bool checked : 1;
  bool radio_checked : 1; // ??
  bool readonly : 1;
  bool activate : 1; // 给窗口使用
} OBJSTATE;

// struct DECLSPEC_UUID(x);

class Object;
struct UIAPI IObject : public IMessage {
  IApplication *GetUIApplication();
  IResource *GetResource();
  IWindow *GetWindow();
  const char *GetId();
  void SetId(const char *szText);
  // HWND GetHWND();
  void InitDefaultAttrib();
  std::shared_ptr<IMapAttribute> GetMapAttribute();
  void ClearMapAttribute();
  void SetOutRef(void **ppOutRef);
  // void LoadAttributeFromMap(IMapAttribute *pMatAttrib, bool bReload);
  void LoadAttributeFromXml(IUIElement *pXmlElement, bool bReload);
  const char *GetAttribute(const char *szKey, bool bErase);
  void AddAttribute(const char *szKey, const char *szValue);

  IObject *GetParentObject();
  IObject *FindObject(const char *szObjId);
  IObject *TryFindObject(const char *szObjId);
  IObject *FindNcObject(const char *szObjId);
  IObject *FindObject(Uuid uuid);
  IObject *FindNcObject(Uuid uuid);
  IObject *GetChildObject();
  IObject *GetNcChildObject();
  IObject *GetNextObject();
  IObject *GetPrevObject();
  IObject *GetLastChildObject();

  IObject *EnumChildObject(IObject *p);
  IObject *REnumChildObject(IObject *p);
  IObject *EnumNcChildObject(IObject *p);
  IObject *EnumAllChildObject(IObject *p);

  IObject *AddChild(IObject *p);
  IObject *AddNcChild(IObject *p);
  bool IsMyChild(IObject *pChild, bool bFindInGrand);
  bool RemoveChildInTree(IObject *pChild);
  void InsertChild(IObject *pObj, IObject *pInsertAfter);

  void Invalidate();
  void Invalidate(const Rect *prcObj);
  void Invalidate(Rect *prcObj, int nCount);
  void UpdateLayout();

  void SetObjectPos(int x, int y, int cx, int cy, SetPositionFlags nFlag);
  void SetObjectPos(const Rect *prc, SetPositionFlags nFlag);

  bool IsVisible();
  bool IsCollapsed();
  bool IsSelfVisible();
  void SetVisible(bool b);
  void SetVisibleEx(VISIBILITY_TYPE b);
  bool IsHover();
  bool IsPress();
  bool IsForcePress();
  bool IsForceHover();
  bool IsDefault();
  bool IsEnable();
  bool IsFocus();
  bool CanTabstop();
  void SetEnable(bool b);
  void SetForceHover(bool b, bool bNotify = true);
  void SetForcePress(bool b, bool bNotify = true);
  bool SetMouseCapture(int nNotifyMsgId);
  bool ReleaseMouseCapture();
  bool SetFocusInWindow();

  void ModifyObjectStyle(OBJSTYLE *add, OBJSTYLE *remove);
  bool TestObjectStyle(const OBJSTYLE &test);
  void RemoveMeInTheTree();
  void SetZorderDirect(int lz);
  int GetZOrder();
  void SortChildByZorder();

  void GetParentRect(Rect *prc);
  void GetWindowRect(Rect *prc);
  Size GetDesiredSize();
  int GetWidth();
  int GetHeight();
  int GetMaxWidth();
  int GetMaxHeight();
  void SetMaxWidth(int);
  void SetMaxHeight(int);
  int GetMinWidth();
  int GetMinHeight();
  void SetMinWidth(int);
  void SetMinHeight(int);
  ILayoutParam *GetSafeLayoutParam();
  ILayoutParam *GetLayoutParam();
  void SetLayoutParam(ILayoutParam *);
  void SetPaddingRegion(REGION4 *prc);
  void GetPaddingRegion(REGION4 *prc);
  void SetMarginRegion(REGION4 *prc);
  void GetMarginRegion(REGION4 *prc);
  void GetBorderRegion(REGION4 *prc);
  void GetNonClientRegion(REGION4 *prc);
  void SetExtNonClientRegion(REGION4 *prc);
  void GetExtNonClientRegion(REGION4 *prc);
  void GetClientRectInObject(Rect *prc);
  void GetObjectClientRect(Rect *prc);
  void GetClientRectInWindow(Rect *prc);
  void ClientRect2ObjectRect(const Rect *rcClient, Rect *rcObj);
  void ClientRect2WindowRect(const Rect *rcClient, Rect *rcWnd);
  void WindowRect2ObjectRect(const Rect *rcWindow, Rect *rcObj);
  void WindowPoint2ObjectPoint(const Point *ptWindow, Point *ptObj,
                               bool bCalcTransform);
  void WindowPoint2ObjectClientPoint(const Point *ptWindow, Point *ptObj,
                                     bool bCalcTransform);

  ITextRenderBase *GetTextRender();
  IRenderFont *GetRenderFont();
  IRenderBase *GetBackRender();
  IRenderBase *GetForeRender();
  void SetBackRender(IRenderBase *);
  void SetTextRender(ITextRenderBase *);
  ILayer *GetLayer();
  ILayer *GetSelfLayer();

  void ForwardMessageToChildObject(ui::Msg *pMsg);
  void ForwardInitializeMessageToDecendant();
  // 动画属性
  unsigned char GetOpacity() const;
  void SetOpacity(const unsigned char alpha,
                  LayerAnimateParam *param = nullptr);

  void TranslateBy(float x, float y, float z,
                   LayerAnimateParam *param = nullptr);
  void TranslateTo(float x, float y, float z,
                   LayerAnimateParam *param = nullptr);
  void TranslateToParent(int x, int y, LayerAnimateParam *param = nullptr);
  void TranslateXTo(float x, LayerAnimateParam *param = nullptr);
  void TranslateYTo(float x, LayerAnimateParam *param = nullptr);

  void RotateXTo(float degree, LayerAnimateParam *param = nullptr);
  void RotateXBy(float degree, LayerAnimateParam *param = nullptr);
  void RotateYTo(float degree, LayerAnimateParam *param = nullptr);
  void RotateYBy(float degree, LayerAnimateParam *param = nullptr);
  void RotateZTo(float degree, LayerAnimateParam *param = nullptr);
  void RotateZBy(float degree, LayerAnimateParam *param = nullptr);

  void ScaleTo(float x, float y, LayerAnimateParam *param = nullptr);

  UI_DECLARE_INTERFACE(Object);
};

}
#endif // _UI_IOBJECT_H_