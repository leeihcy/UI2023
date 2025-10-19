#pragma once
#include "include/interface/iobject.h"
#include "include/macro/msg.h"
#include "object_layer.h"
#include "objtree.h"
#include "src/private_inc.h"
#include "src/object/layout/layout_object.h"

namespace ui {
class Resource;
class Application;
class Window;
class Layer;
struct UIElement;
struct IRenderBase;
struct ITextRenderBase;
struct ILayoutParam;
class ObjectLayer;
struct IRenderTarget;

class Object : public ObjTree {
  friend class ObjTree;

public:
  Object(IObject *);
  virtual ~Object();

  // UI_BEGIN_MSG_MAP()
  // UIMSG_ERASEBKGND(OnEraseBkgnd)
  // UIMSG_VISIBLE_CHANGED(OnVisibleChanged)
  // UI_END_MSG_MAP()
  
  void onRouteMessage(ui::Msg *msg);

public:
  IObject *GetIObject();

  const char *GetId();
  void SetId(const char *szText);

#if 0 // defined(OS_WIN)
  bool CreateAccesible(IAccessible **pp);
#endif
  Application *GetUIApplication();
  IApplication *GetIUIApplication();
  Resource *GetResource();
  IResource *GetIResource();
  Window *GetWindow();
  Message *GetWindow2();
  Object* GetRootObject();
#if 0 // defined(OS_WIN)
  HWND GetHWND();
#endif
  Layer *GetLayer();
  Layer *GetLayerForAnimate();
  Object *GetLayerCreator();
  Layer *GetSelfLayer() const;
  ObjectLayer *GetLayerEx();
  bool HasLayer();
  void OnLayerDestory();
  void OnLayerCreate();

  void Invalidate();
  void Invalidate(const Rect *prcObj);
  void Invalidate(Rect *prcObjArray, int nCount);
  void DrawToLayer__(IRenderTarget *pRenderTarget);
  void DrawChildObject__(IRenderTarget *pRenderTarget, Object *pChildStart);

  Object *FindObject(const char *szObjId);
  Object *TryFindObject(const char *szObjId);
  Object *FindNcObject(const char *szobjId);
  Object *FindObject(Uuid uuid);
  Object *FindNcObject(Uuid uuid);
  unsigned int GetChildCount();
  Object *GetChildObjectByIndex(unsigned int lIndex);
  unsigned int GetChildObjectIndex(Object *pChild);

  bool IsTransparent();
  void SetTransparent(bool b);
  bool IsNoClip();
  bool NeedClip();
  void SetNoClip(bool b);
  void SetClipClient(bool b);
  bool NeedClipClient();
  bool IsFocus();
  void SetFocus(bool b, bool bNoitfy = true);
  bool SetFocusInWindow();
  bool IsTabstop();
  bool CanTabstop() override;
  void SetTabstop(bool b);
  bool IsSelfVisible();
  virtual bool IsVisible();
  bool IsCollapsed();
  bool IsSelfCollapsed();
  virtual bool IsEnable();
  void LoadVisibleEx(int l);
  int SaveVisibleEx();
  void SetVisible(bool b);
  void SetVisibleEx(VISIBILITY_TYPE eType);
  void SetDisableDirect(bool b);
  bool IsSelfDisable();
  void SetEnable(bool b, bool bNoitfy = true);
  bool IsDefault();
  bool IsHover();
  bool IsPress();
  bool IsForceHover();
  bool IsForcePress();
  bool IsSelected();
  void SetDefault(bool b, bool bNotify = true);
  void SetSelected(bool b, bool bNotify = true);
  void SetForceHover(bool b, bool bNotify = true);
  void SetForcePress(bool b, bool bNotify = true);
  void SetHover(bool b, bool bNotify = true);
  void SetPress(bool b, bool bNotify = true);
  void SetAsNcObject(bool b) override;
  bool IsNcObject() override;
  bool IsRejectMouseMsgAll();
  bool IsRejectMouseMsgSelf();
  void SetRejectMouseMsgAll(bool b);
  void SetRejectMouseMsgSelf(bool b);
  void SortChildByZorder();

  void ModifyObjectStyle(OBJSTYLE *add, OBJSTYLE *remove);
  bool TestObjectStyle(const OBJSTYLE &test);

  void LoadAttributes(bool bReload);
  void LoadAttributeFromXml(UIElement *pElement, bool bReload);

  const char *GetAttribute(const char *szKey, bool bErase);
  void AddAttribute(const char *szKey, const char *szValue);
  std::shared_ptr<IMapAttribute> GetMapAttribute();
  void ClearMapAttribute();
  void InitDefaultAttrib();
   
  void SetOutRef(void **ppOutRef);
  Object *GetObjectByPos(Point *pt);
  bool SetMouseCapture(int nNotifyMsgId);
  bool ReleaseMouseCapture();
  bool SetKeyboardCapture(int nNotifyMsgId);
  bool ReleaseKeyboardCapture();

  void SetBackRender(std::shared_ptr<IRenderBase> p);
  void SetForegndRender(std::shared_ptr<IRenderBase> p);
  void SetTextRender(std::shared_ptr<ITextRenderBase> p);
  std::shared_ptr<ITextRenderBase> GetTextRender();
  std::shared_ptr<IRenderBase> GetBackRender();
  std::shared_ptr<IRenderBase> GetForeRender();
  IRenderFont *GetRenderFont();

  void LoadBkgndRender(const char *szName);
  void LoadForegndRender(const char *szName);
  void LoadTextRender(const char *szName);
  const char *SaveBkgndRender();
  const char *SaveForegndRender();
  const char *SaveTextRender();

  LayoutObject* GetLayoutObject();
  Size GetDesiredSize();
  void UpdateLayout();
  // void  UpdateMyLayout();
  void UpdateLayoutPos();
  void UpdateObjectNonClientRegion();
  virtual void SetObjectPos(int x, int y, int cx, int cy, SetPositionFlags flags);
  void SetObjectPos(const Rect *prc, SetPositionFlags flags);

  void GetParentRect(Rect *prc);
  Point GetWindowPoint();
  void GetWindowRect(Rect *lprc);
  bool GetRectInWindow(Rect *prc, bool bOnlyVisiblePart);
  void GetClientRectInObject(Rect *prc);
  void GetClientRectWithZeroOffset(Rect *prc);
  bool CalcRectInAncestor(Object *pObjAncestor, const Rect *prcObjPart,
                          bool bCalcVisible, Rect *prcOut);
  bool GetScrollOffset(int *pxOffset, int *pyOffset);
  bool GetScrollRange(int *pxRange, int *pyRange);

  int GetParentRectL() { return m_rcParent.left; }
  int GetParentRectT() { return m_rcParent.top; }
  int GetParentRectR() { return m_rcParent.right; }
  int GetParentRectB() { return m_rcParent.bottom; }

  void LoadBorder(REGION4 *prc);
  void SaveBorder(REGION4 *prc);
  int GetBorderL() { return m_rcBorder.left; }
  int GetBorderT() { return m_rcBorder.top; }
  int GetBorderR() { return m_rcBorder.right; }
  int GetBorderB() { return m_rcBorder.bottom; }
  void SetBorderRegion(Rect *prc);
  void GetBorderRegion(REGION4 *prc) { prc->CopyFrom(m_rcBorder); }

  void LoadPadding(REGION4 *prc);
  void SavePadding(REGION4 *prc);
  int GetPaddingL() { return m_rcPadding.left; }
  int GetPaddingT() { return m_rcPadding.top; }
  int GetPaddingR() { return m_rcPadding.right; }
  int GetPaddingB() { return m_rcPadding.bottom; }
  int GetPaddingW() { return m_rcPadding.left + m_rcPadding.right; }
  int GetPaddingH() { return m_rcPadding.top + m_rcPadding.bottom; }
  void GetPaddingRegion(REGION4 *prc) { prc->CopyFrom(m_rcPadding); }
  void SetPaddingRegion(REGION4 *prc);

  void LoadMargin(REGION4 *prc);
  void SaveMargin(REGION4 *prc);
  void SetMarginRegion(REGION4 *prc) { m_rcMargin.CopyFrom(*prc); }
  void GetMarginRegion(REGION4 *prc) { prc->CopyFrom(m_rcMargin); }
  int GetMarginL() { return m_rcMargin.left; }
  int GetMarginT() { return m_rcMargin.top; }
  int GetMarginR() { return m_rcMargin.right; }
  int GetMarginB() { return m_rcMargin.bottom; }
  int GetMarginW() { return m_rcMargin.left + m_rcMargin.right; }
  int GetMarginH() { return m_rcMargin.top + m_rcMargin.bottom; }

  void GetNonClientRegion(REGION4 *prc);
  void SetExtNonClientRegion(REGION4 *prc) { m_rcExtNonClient.CopyFrom(*prc); }
  void GetExtNonClientRegion(REGION4 *prc) { prc->CopyFrom(m_rcExtNonClient); }

  void GetClientRectInParent(Rect *prc);
  void GetClientRectInWindow(Rect *prc);
  int GetWidth();
  int GetHeight();
  int GetWidthWithMargins();
  int GetHeightWithMargins();
  

  bool IntersectWindowRect(const Rect *prcWindow, Rect *prcIntersectWnd,
                           Rect *prcIntersectObj);
  void WindowPoint2ObjectPoint(const Point *ptWindow, Point *ptObj,
                               bool bCalcTransform);
  void WindowPoint2ObjectClientPoint(const Point *ptWindow, Point *ptClient,
                                     bool bCalcTransform);
  void WindowRect2ObjectClientRect(const Rect *rcWindow, Rect *rcObj);
  void WindowRect2ObjectRect(const Rect *rcWindow, Rect *rcObj);

  static void ParentClientPoint2ChildPoint(Object *pObjChild, const Point *pt,
                                           Point *pOut);

  static void ObjectPoint2ObjectClientPoint(Object *pObj, const Point *pt,
                                            Point *pOut);
  static void ObjectPoint2ObjectNonClientPoint(Object *pObj, const Point *pt,
                                               Point *pOut);
  static void ObjectRect2ObjectClientRect(Object *pObj, const Rect *prc,
                                          Rect *pOut);
  static void ObjectClientRect2WindowRect(Object *pObj, const Rect *prcClient,
                                          Rect *prcWnd);
  static void ObjectRect2WindowRect(Object *pObj, const Rect *prcObj,
                                    Rect *prcWnd);

  static void ParentClientPoint2ChildClientPoint(Object *pObjChild,
                                                 const Point *pt, Point *pOut);
  static void ParentClientRect2ChildClientRect(Object *pObjChild,
                                               const Rect *prc, Rect *pOut);

  static void ParentPoint2ChildPoint(Object *pObjChild, const Point *pt,
                                     Point *pOut);
  static void ParentRect2ChildRect(Object *pObjChild, const Rect *prc,
                                   Rect *pOut);

  static void ChildPoint2ParentClientPoint(Object *pObjChild,
                                           const Point *ptChild, Point *ptOut);
  static void ChildRect2ParentClientRect(Object *pObjChild, const Rect *prc,
                                         Rect *pOut);

  static void ObjectClientPoint2ObjectPoint(Object *pObj, const Point *ptChild,
                                            Point *ptOut);
  static void ObjectClientRect2ObjectRect(Object *pObj, const Rect *prc,
                                          Rect *pOut);

  static void ChildPoint2ParentPoint(Object *pObjChild, const Point *ptChild,
                                     Point *ptOut);
  static void ChildRect2ParentRect(Object *pObjChild, const Rect *prc,
                                   Rect *pOut);

  // 动画属性
  unsigned char GetOpacity() const;
  void SetOpacity(const unsigned char alpha, LayerAnimateParam *param);

  void TranslateBy(float x, float y, float z, LayerAnimateParam *param);
  void TranslateTo(float x, float y, float z, LayerAnimateParam *param);
  void TranslateXTo(float x, LayerAnimateParam *param);
  void TranslateYTo(float y, LayerAnimateParam *param);
  void TranslateToParent(int x, int y, LayerAnimateParam *param);

  void RotateXTo(float degree, LayerAnimateParam *param);
  void RotateXBy(float degree, LayerAnimateParam *param);
  void RotateYTo(float degree, LayerAnimateParam *param);
  void RotateYBy(float degree, LayerAnimateParam *param);
  void RotateZTo(float degree, LayerAnimateParam *param);
  void RotateZBy(float degree, LayerAnimateParam *param);

  void ScaleTo(float x, float y, LayerAnimateParam *param = nullptr);

protected:
  int FinalConstruct(IResource *pSkinRes);
  void FinalRelease();
  eHitTest OnHitTest(const Point& ptInParent, Point& ptInChild);
  void OnVisibleChanged(bool bVisible, IObject *pObjChanged);
  void onSerialize(SerializeParam *pData);
  void OnEraseBkgnd(IRenderTarget *pRenderTarget);

protected:
  Object *find_child_object(const char *szobjId, bool bFindDecendant);
  Object *find_child_object(Uuid uuid, bool bFindDecendant);
  Object *find_ncchild_object(Uuid uuid, bool bFindDecendant);
  void load_renderbase(const char *szName, std::shared_ptr<IRenderBase> &pRender);
  void load_textrender(const char *szName, std::shared_ptr<ITextRenderBase> &pTextRender);
  const char *get_renderbase_name(std::shared_ptr<IRenderBase> &pRender);
  const char *get_textrender_name(std::shared_ptr<ITextRenderBase> &pTextRender);

  void load_layer_config(bool b);

public:
  void notify_WM_SIZE(unsigned int nType, unsigned int nWidth,
                      unsigned int nHeight, float scale);
  void notify_WM_MOVE(int x, int y);

  IMKMgr *GetIMKMgr();

protected: // virtual
  virtual IMKMgr *virtualGetIMKMgr() { return nullptr; }
  virtual void virtualSetVisibleEx(VISIBILITY_TYPE eType);
  virtual void virtualSetEnable(bool b);
  virtual void virtualOnSize(unsigned int nType, unsigned int nWidth,
                             unsigned int nHeight, float scale);
  virtual void virtualOnMove();
  virtual void virtualOnPostDrawObjectErasebkgnd(){};

public:
  virtual void virtualOnLoad();

public:
  static void ForwardMessageToChildObject(Object *pParent, ui::Msg *pMsg);
  static void ForwardInitializeMessageToDecendant(Object *pParent);

public:
  LayoutObject layout;

protected:
  IObject *m_pIObject;
  // 用于支持多皮肤包共存（插件模式）
  Resource *m_resource = nullptr; 

  std::string m_strId; // 该对象在XML中的标识
#ifdef EDITOR_MODE
  std::string m_strStyle; // 控件样式
#endif

  ObjectLayer m_objLayer;

#pragma region //坐标相关数据
// 该对象的范围，相对于parent的client区域.对于Window对象是客户区域位置，即左上角为0，0
  Rect m_rcParent = { 0 }; 

  // 扩展的非客户区，与border、padding共同做为对象的非客户区。
  // 对于窗口类型，这个值会动态计算，带上边框、标题栏范围。
  Rect m_rcExtNonClient = { 0 }; 

  Rect m_rcMargin = { 0 };
  Rect m_rcPadding = { 0 };
  Rect m_rcBorder = { 0 };
  // HRGN     m_hRgn;                  //
  // （未使用）如果该对象是一个不规则区域，必须设置该值，该值对window类型对象无效.
  // rgn是相对于窗口左上角的。
#pragma endregion

public: // TODO:
  OBJSTYLE m_objStyle;
  OBJSTATE m_objState;
protected:
  std::shared_ptr<IMapAttribute> m_pIMapAttributeRemain; // 用于扩展。未解析的属性
  std::shared_ptr<IRenderBase> m_back_render;           // 背景渲染
  std::shared_ptr<IRenderBase> m_fore_render;         // 前景渲染
  std::shared_ptr<ITextRenderBase> m_text_render; // 文字渲染，由control负责读取该属性
  
#if 0 // defined(OS_WIN)
  IAccessible *m_pAccessible;
#endif
  
  // 为了解决一个类成员对象，有可能被自己的父对象删除后，这个类却不知道，再删除该对象时崩溃了.
  void ** m_ppOutRef = nullptr; 

  friend class ObjectAccessible;
};

} // namespace ui
