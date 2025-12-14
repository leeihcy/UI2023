#pragma once
#include "message.h"
#include "object_layer.h"
#include "object_tree.h"
#include "object_state.h"
#include "object_style.h"
#include "object_rect.h"
#include "object_animate.h"
#include "object_layout.h"
#include "object_prop.h"

#include "include/interface/iobject.h"
#include "include/macro/msg.h"
#include "src/private_inc.h"


namespace ui {
class ResourceBundle;
class Application;
class Window;
class Layer;
class ObjectLayer;
struct IRenderTarget;

class Object : 
  public Message,
  public ObjectAnimate,
  public ObjectLayer,
  public ObjectLayout,
  public ObjectProp,
  public ObjectRect,
  public ObjectState,
  public ObjectStyle,
  public ObjectTree
{
public:
  Object(IObject *);
  virtual ~Object();

  IObject *GetIObject();
  void onRouteMessage(ui::Msg *msg);
public:
  Application *GetUIApplication();
  IApplication *GetIUIApplication();
  ResourceBundle *GetResource();
  IResourceBundle *GetIResource();
  Window *GetWindow();
  Message *GetWindowMessagePtr();
  IMKMgr *GetIMKMgr();
  Object* GetRootObject();

public:
  Object *FindObject(const char *szObjId);
  Object *TryFindObject(const char *szObjId);
  Object *FindNcObject(const char *szobjId);
  Object *FindObject(Uuid uuid);
  Object *FindNcObject(Uuid uuid);
  unsigned int GetChildCount();
  Object *GetChildObjectByIndex(unsigned int lIndex);
  unsigned int GetChildObjectIndex(Object *pChild);

public:
  Layer *GetLayer();
  Object *GetLayerCreator();
  Layer *GetSelfLayer() const;
  ObjectLayer *GetLayerEx();
  void OnLayerDestory();
  void OnLayerCreate();
  void OnLayerStyleChanged(bool);

  void Invalidate();
  void Invalidate(const Rect *prcObj);
  void Invalidate(Rect *prcObjArray, int nCount);
  void DrawToLayer__(IRenderTarget *pRenderTarget);
  void DrawChildObject__(IRenderTarget *pRenderTarget, Object *pChildStart);

public:
  void SetFocus(bool b, bool bNoitfy = true);
  bool SetFocusInWindow();
  bool CanTabstop();
  virtual bool IsVisible();
  bool IsCollapsed();
  bool IsSelfCollapsed();
  void SetVisible(bool b);
  void SetVisibleEx(VISIBILITY_TYPE eType);
  void SortChildByZorder();

  Object *GetObjectByPos(Point *pt);
  bool SetMouseCapture(int nNotifyMsgId);
  bool ReleaseMouseCapture();
  bool SetKeyboardCapture(int nNotifyMsgId);
  bool ReleaseKeyboardCapture();

  ObjectLayout* GetLayoutObject();

  Size GetDesiredSize();
  void UpdateLayout();
  void UpdateLayoutPos();
  void UpdateObjectNonClientRegion();
  virtual void SetObjectPos(int x, int y, int cx, int cy, SetPositionFlags flags);
  void SetObjectPos(const Rect *prc, SetPositionFlags flags);

  void GetNonClientRegion(REGION4 *prc);
  void GetClientRectInParent(Rect *prc);
  void GetClientRectInWindow(Rect *prc);

  bool GetScrollOffset(int *pxOffset, int *pyOffset);
  bool GetScrollRange(int *pxRange, int *pyRange);
public:
  void ForwardMessageToChildren(ui::Msg *pMsg);
  void ForwardInitializeMessageToDecendant();

protected:
  int FinalConstruct(IResourceBundle *resource_bundle);
  void FinalRelease();
  eHitTest OnHitTest(const Point& ptInParent, Point& ptInChild);
  void OnVisibleChanged(bool bVisible, IObject *pObjChanged);
  void onLoaded();
  void onSerialize(SerializeParam *pData);
  
protected:
  Object *find_child_object(const char *szobjId, bool bFindDecendant);
  Object *find_child_object(Uuid uuid, bool bFindDecendant);
  Object *find_ncchild_object(Uuid uuid, bool bFindDecendant);

public:
  void notify_WM_SIZE(unsigned int nType, unsigned int nWidth,
                      unsigned int nHeight, float scale);
  void notify_WM_MOVE(int x, int y);

protected: // virtual
  virtual IMKMgr *virtualGetIMKMgr() { return nullptr; }
  virtual void virtualSetVisibleEx(VISIBILITY_TYPE eType);
  virtual void virtualOnPostDrawObjectErasebkgnd(){};

protected:
  // 用于支持多皮肤包共存（插件模式）
  ResourceBundle *m_resource = nullptr; 
 
#if 0
  bool CreateAccesible(IAccessible **pp);
  IAccessible *m_pAccessible;
  friend class ObjectAccessible;
#endif
  friend class ObjectTree;
  friend class ObjectRect;
  friend class ObjectProp;
  friend class ObjectAnimate;
  friend class ObjectLayout;
};

} // namespace ui
