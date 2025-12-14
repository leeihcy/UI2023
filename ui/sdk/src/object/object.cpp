#include "object.h"
#include "include/interface/imessage.h"
#include "include/interface/imeta.h"
#include "include/interface/iobject.h"
#include "include/macro/msg.h"
#include "include/macro/uidefine.h"
#include "object_layer.h"
#include "include/interface/ilayout.h"
#include "include/interface/iwindow.h"
#include "src/layout/canvaslayout.h"
#include "src/object/object_prop.h"
#include "src/property/property.h"
#include "src/resource/uiresource.h"
#include "include/interface/iuires.h"
#include "include/interface/ixmlwrap.h"
#include "src/application/uiapplication.h"
#include "src/window/window.h"
#include "src/panel/root_object.h"
#include "src/resource/colorres.h"
#include "src/resource/cursorres.h"
#include "src/resource/res_bundle.h"
#include "src/resource/stylemanager.h"
#include "src/property/property_id.h"
#include "include/interface/iattributemap.h"
#include <algorithm>
#include <memory>
#include "object_meta.h"

using namespace ui;

Object::Object(IObject *p) : Message(p)
{
  m_pIObject = p;
  m_attribute_map_remaining = nullptr;

  memset(&m_objStyle, 0, sizeof(m_objStyle));
  memset(&m_objState, 0, sizeof(m_objState));
  m_objState.visibility_ = VISIBILITY_VISIBLE;
}

// 注意：不要在构造或者析构函数中调用虚函数

Object::~Object(void) {
}

void Object::onRouteMessage(ui::Msg *msg) {
  if (msg->message == UI_MSG_HITTEST) {
    HitTestMessage *hittest_msg = static_cast<HitTestMessage *>(msg);
    hittest_msg->hittest =
        OnHitTest(hittest_msg->pt_in_parent, hittest_msg->pt_in_child);
    return;
  }
  if (msg->message == UI_MSG_FINALCONSTRUCT) {
    Message::onRouteMessage(msg);
    FinalConstruct(static_cast<FinalConstructMessage*>(msg)->resource);
    return;
  }
  if (msg->message == UI_MSG_LOADED) {
    onLoaded();
    return;
  }
  if (msg->message == UI_MSG_FINALRELEASE) {
    FinalRelease();
    return;
  }
  if (msg->message == UI_MSG_SERIALIZE) {
    onSerialize(static_cast<SerializeMessage*>(msg)->param);
    return;
  }
}

int Object::FinalConstruct(IResourceBundle *resource_bundle) {
  m_resource = resource_bundle->GetImpl();
  return 0;
}

void Object::FinalRelease() {
  // 防止调用中途或者换肤过程中，一些对象突然被销毁，导致的野指针错误
  // 必须在DestroyChildObject前调用，否则parent关系被清空
  IMKMgr *pMKMgr = this->GetIMKMgr();
  if (pMKMgr) {
    pMKMgr->OnObjectRemoveInd(this);
  }

  this->DestroyChildObject(); // 先删除子对象

  IUIEditor *pEditor = GetUIApplication()->GetUIEditorPtr();
  if (pEditor) {
    // 由子往父进行通知，便于从skinbuilder的tree中一个个删除
    pEditor->OnObjectDeleteInd(m_pIObject);
  }

  //	清理自己的邻居关系
  RemoveMeInTheTree();

  m_back_render.reset();
  m_fore_render.reset();
  m_text_render.reset();

  // 在析构之前销毁，避免窗口的window compositor已经销毁了，但layer还没有被销毁
  ObjectLayer::DestroyLayer();
}

IObject *Object::GetIObject() { return m_pIObject; }

Layer *Object::GetSelfLayer() const { return ObjectLayer::GetLayer(); }

// 获取对象所在Layer
Layer *Object::GetLayer() {
  ObjectLayer *pLayer = GetLayerEx();
  if (pLayer)
    return pLayer->GetLayer();

  return nullptr;
}

// 获取一个控件所在窗口句炳
Object* Object::GetRootObject() {
  Object *pParent = this;
  while (pParent->m_pParent) {
    pParent = pParent->m_pParent;
  }
  return pParent;
}

Window *Object::GetWindow() {
  Object* root = GetRootObject();
  if (!root || !root->m_meta) {
    return nullptr;
  }
  
  // query interface有点慢，直接换meta major type来判断。
  if (root->m_meta->Detail().major_type != OBJ_ROOT) {
    return nullptr;
  }
  return &static_cast<RootObject*>(root)->GetWindow();

  // IWindow *pWindow =
  //     (IWindow *)pParent->GetIObject()->QueryInterface(IWindow::UUID());
  // if (pWindow)
  //   return pWindow->GetImpl();
  // return nullptr;
}

Message *Object::GetWindow2() {
  return static_cast<Message*>(GetWindow());
}

ObjectLayer *Object::GetLayerEx() {
  Object *pObj = this;
  while (pObj) {
    if (pObj->GetSelfLayer())
      return static_cast<ObjectLayer*>(pObj);

    pObj = pObj->m_pParent;
  }

  return nullptr;
}

// 获取自己所在层的层对象
Object *Object::GetLayerCreator() {
  ObjectLayer *p = GetLayerEx();
  if (p)
    return static_cast<Object*>(p);

  return nullptr;
}

// TODO: 函数名修改为find decendant object
//
//	在所有的子子孙孙中查找id为objId的对象，只返回第一个找到的对象
//
//	Parameter
//		objId
//			[in]	要查找的对象id
//
//	Return
//		成功返回对象地址，失败返回nullptr
//
Object *Object::FindObject(const char *szObjId) {
  if (!szObjId)
    return nullptr;

  Object *pRet = this->find_child_object(szObjId, true);
  if (!pRet) {
    UI_LOG_WARN("Find \"%s\" from \"%s\" failed.", szObjId, GetId());
    UIASSERT(0);
  }
  return pRet;
}

// 不写日志
Object *Object::TryFindObject(const char *szObjId) {
  if (!szObjId)
    return nullptr;

  Object *pRet = this->find_child_object(szObjId, true);
  return pRet;
}

// 没有递归
Object *Object::FindNcObject(const char *szobjId) {
  if (nullptr == szobjId)
    return nullptr;

  Object *pObjChild = nullptr;
  while ((pObjChild = this->EnumNcChildObject(pObjChild))) {
    if (0 == strcmp(pObjChild->GetId(), szobjId)) {
      return pObjChild;
    }
  }
  return nullptr;
}

Object *Object::FindObject(Uuid uuid) {
  return this->find_child_object(uuid, true);
}
Object *Object::FindNcObject(Uuid uuid) {
  return this->find_ncchild_object(uuid, true);
}

Object *Object::find_child_object(const char *szobjId, bool bFindDecendant) {
  if (nullptr == szobjId)
    return nullptr;

  Object *pObjChild = nullptr;
  while ((pObjChild = this->EnumChildObject(pObjChild))) {
    if (0 == strcmp(pObjChild->GetId(), szobjId)) {
      return pObjChild;
    }
  }

  if (!bFindDecendant)
    return nullptr;

  pObjChild = nullptr;
  while ((pObjChild = this->EnumChildObject(pObjChild))) {
    Object *pRet =
        ((Object *)pObjChild)->find_child_object(szobjId, bFindDecendant);
    if (pRet)
      return pRet;
  }
  return nullptr;
}

Object *Object::find_child_object(Uuid uuid, bool bFindDecendant) {
  Object *pObjChild = nullptr;
  while ((pObjChild = this->EnumChildObject(pObjChild))) {
    IMeta *pDesc = pObjChild->GetMeta();
    if (pDesc->UUID() == uuid) {
      return pObjChild;
    }
  }

  if (!bFindDecendant)
    return nullptr;

  pObjChild = nullptr;
  while ((pObjChild = this->EnumChildObject(pObjChild))) {
    Object *pRet =
        ((Object *)pObjChild)->find_child_object(uuid, bFindDecendant);
    if (pRet)
      return pRet;
  }
  return nullptr;
}

Object *Object::find_ncchild_object(Uuid uuid, bool bFindDecendant) {
  Object *pObjChild = nullptr;
  while ((pObjChild = this->EnumNcChildObject(pObjChild))) {
    IMeta *pDesc = pObjChild->GetMeta();
    if (pDesc->UUID() == uuid) {
      return pObjChild;
    }
  }

  if (!bFindDecendant)
    return nullptr;

  pObjChild = nullptr;
  while ((pObjChild = this->EnumNcChildObject(pObjChild))) {
    Object *pRet =
        ((Object *)pObjChild)->find_ncchild_object(uuid, bFindDecendant);
    if (pRet)
      return pRet;
  }
  return nullptr;
}

//	[public] [virtual]  unsigned int HitTest( Point* ptInParent )
//
//	Parameters
//		ptInParent
//			[in]  要试探的位置，通常为鼠标当前位置
//
//	Return
//		鼠标不在对象所有警围内返回HTNOWHERE，否则返回HTCLIENT，或其它（子类可扩展）
//
//	Remark
//		这个函数是用来解决一个问题，是为Object配备一个RECT对象表示范围，还是给一个更强大的RGN表示范围?
//		由于RGN更加消耗内存，而有时候就得使用RGN来实现某些功能，因此在这里用这个函数来告诉调用者，
//		你这个位置处于我什么部位。同时给Object配备一个RECT，来表示每一个对象的范围，因为大部分时候使用的都是RECT区域。
//
eHitTest Object::OnHitTest(const Point& ptInParent, Point& ptInChild) {
  bool bIn = false;

  Layer *layer = GetSelfLayer();
  if (layer) {
    Point ptObj = {ptInParent.x - m_rcParent.left,
                   ptInParent.y - m_rcParent.top};

    layer->MapView2Layer(&ptObj);

    Rect rcObj = {0, 0, m_rcParent.Width(), m_rcParent.Height()};
    if (rcObj.PtIn(ptObj)) {
      ptInChild = ptObj;
      return eHitTest::Client;
    }
  } else {
    if (m_rcParent.PtIn(ptInParent)) {
      ptInChild.x = ptInParent.x - m_rcParent.left;
      ptInChild.y = ptInParent.y - m_rcParent.top;

      return eHitTest::Object;
    }
  }

  return eHitTest::Nowhere;
}

// void Object::OnThemeChanged()
// {
// 	SetMsgHandled(false);
//
//     if (m_back_render)
//         m_back_render->CheckThemeChanged();
//     if (m_fore_render)
//         m_fore_render->CheckThemeChanged();
// }
//
// int  Object::OnSkinTextureChanged(unsigned int, int, int)
// {
//     if (m_text_render)
//     {
//         m_text_render->CheckSkinTextureChanged();
//     }
//     return 0;
// }

IMKMgr *Object::GetIMKMgr() {
  IMKMgr *pMKMgr = nullptr;

  Object *pParent = m_pParent;
  while (pParent) {
    pMKMgr = pParent->virtualGetIMKMgr();
    if (pMKMgr)
      return pMKMgr;

    pParent = pParent->m_pParent;
  }
  return nullptr;
}

void Object::OnVisibleChanged(bool bVisible, IObject *pObjChanged) {
  //     if (!bVisible)
  //     {
  //         // 防止调用中途或者换肤过程中，一些对象突然被销毁，导致的野指针错误
  //         WindowBase* pWindow = GetWindow();
  //         if (pWindow)  //
  //         必须在DestroyChildObject前调用，否则parent关系被清空
  //         {
  //             pWindow->OnObjectHideInd(this);
  //         }
  //     }

  // 如果隐藏的对象是一个焦点对象，则将焦点重新切回到第一个对象
  // 注，这里的object也可能是位于一个listctrl中，则其对应的mouse/keyboard
  //     manager则不再是窗口的，而是listctrl自己的

  IMKMgr *pMKMgr = GetIMKMgr();
  if (pMKMgr) {
    pMKMgr->OnObjectVisibleChangeInd(this, bVisible ? true : false);
  }
}

void Object::SetFocus(bool b, bool bNoitfy) {
  if (m_objState.focus == b)
    return;

  m_objState.focus = b;
  if (bNoitfy) {
    StateChangedMessage msg;
    msg.state_changed_mask = OSB_FOCUS;
    RouteMessage(&msg);
  }
}

// 注：控件有可能位于列表项内(listitem)，给该项里的控件设置焦点不应该抢走listtrl的焦点
bool Object::SetFocusInWindow() {
  IMKMgr *pMKMgr = GetIMKMgr();
  if (!pMKMgr)
    return false;

  pMKMgr->SetFocusObject(this);

  // 	WindowBase* pWnd = GetWindow();
  // 	if (!pWnd)
  // 		return false;
  //
  // 	pWnd->SetFocusObject(this);
  return true;
}

bool Object::CanTabstop() {
  return this->IsTabstop() && this->IsEnable() && this->IsVisible();
}

bool Object::IsVisible() {
  bool bVisible = m_objState.visibility_ == VISIBILITY_VISIBLE;
  if (false == bVisible || nullptr == m_pParent) {
    return bVisible;
  }
  // 判断父对象不可见的情况

  // 注：在这里不对最外层的窗口进行判断的原因是：在类似于窗口初始化的函数里面，
  // 虽然窗口暂时是不可见的，但里面的对象的IsVisible应该是返回true才好处理
  if (m_pParent->m_meta &&
      OBJ_WINDOW == m_pParent->m_meta->Detail().major_type) {
    return true;
  } else {
    return m_pParent->IsVisible();
  }
}

bool Object::IsCollapsed() {
  bool bCollapsed =
      m_objState.visibility_ == VISIBILITY_COLLAPSED ? true : false;
  if (bCollapsed || nullptr == m_pParent) {
    return bCollapsed;
  }
  // 判断父对象不可见的情况
  else {
    return m_pParent->IsCollapsed();
  }

  return false;
}
bool Object::IsSelfCollapsed() {
  if (m_objState.visibility_ == VISIBILITY_COLLAPSED)
    return true;

  return false;
}

void Object::SetVisible(bool b) {
  SetVisibleEx(b ? VISIBILITY_VISIBLE : VISIBILITY_COLLAPSED);
}

void Object::virtualSetVisibleEx(VISIBILITY_TYPE eType) {}

void Object::ForwardMessageToChildren(ui::Msg *message) {
  Object *pChild = nullptr;
  while ((pChild = EnumChildObject(pChild))) {
    pChild->RouteMessage(message);
    pChild->ForwardMessageToChildren(message);
  }

  Object *pNcChild = nullptr;
  while ((pNcChild = EnumNcChildObject(pNcChild))) {
    pNcChild->RouteMessage(message);
    pNcChild->ForwardMessageToChildren(message);
  }
}

// 发给父对象，再发给子对象，最后再通知一次父对象
// 应用场景：UI_WM_INITIALIZE/UI_WM_INITIALIZE2
// 注：如果已经初始化过了的控件，不再发送消息，如换肤（换布局）过程中有些控件不重新创建，
//    但是还会发送一个bindind通知，用于重新绑定新的子控件，例如重新绑定滚动条
void Object::ForwardInitializeMessageToDecendant() {
  Object *pChild = nullptr;
  while ((pChild = EnumAllChildObject(pChild))) {

    if (pChild->m_objStyle.initialized) {
      pChild->ForwardInitializeMessageToDecendant();
    } else {
      pChild->m_objStyle.initialized = 1;

      pChild->RouteMessage(UI_MSG_INITIALIZE);
      pChild->ForwardInitializeMessageToDecendant();
      pChild->RouteMessage(UI_MSG_INITIALIZE2);
    }
  }
}

void Object::SetVisibleEx(VISIBILITY_TYPE eType) {
  if (m_objState.visibility_ == eType)
    return;

  bool bOld = IsVisible();
  bool bVisibleCompatible = eType == VISIBILITY_VISIBLE ? true : false;

  m_objState.visibility_ = eType;
  virtualSetVisibleEx(eType);
    assert(false);

  VisibleChangedMessage message;
  message.visible = bVisibleCompatible;
  message.obj_trigger = m_pIObject;

  // 通知子对象
  RouteMessage(&message);
  Object::ForwardMessageToChildren(&message);

  if (m_pParent) {
    GetLayoutMessage msg;
    m_pParent->RouteMessage(&msg);

    if (msg.layout) {
      float scale = 1.0f;
      auto* window = GetWindow();
      if (window) {
        scale = window->m_dpi.GetScaleFactor();
      }
      ArrangeParam param = {
        m_pIObject, 
        ArrangeReason::VisibleChanged,
      };
      msg.layout->Arrange(param);
    }
  }

  if (bVisibleCompatible != bOld) {
    // 确实存在在初始化中调用setvisible，但此时还没有 m_pParent
    // 的情况，如SystemScrollBar
    // 		if (bUpdateLayout)
    // 		{
    // 			this->UpdateLayout(bRedraw);
    //         }
    //         else if (bRedraw)
    //         {
    //             if (bVisibleCompatible)
    //                 this->Invalidate();
    //             else
    //                 this->UpdateObjectBkgnd(true);
    //         }
    //         else
    //         {
    //         }
  }
}

ResourceBundle *Object::GetResource() { return m_resource; }

IResourceBundle *Object::GetIResource() {
  if (!m_resource)
    return nullptr;

  return m_resource->GetIResource();
}

Application *Object::GetUIApplication() {
  UIASSERT(m_resource);
  UIASSERT(m_resource->GetUIApplication());

  return m_resource->GetUIApplication();
}

IApplication *Object::GetIUIApplication() {
  UIASSERT(m_resource);
  UIASSERT(m_resource->GetUIApplication());

  return m_resource->GetUIApplication()->GetIUIApplication();
}

void Object::onSerialize(SerializeParam *data) {
  ObjectProp::SerializeProperty(data);
  ObjectLayout::SerializeLayout(data);
}

void Object::onLoaded() {
  // 等对象在树中位置确定了，相关属性确定了之后，再创建layer
  if (m_objStyle.layer && !GetSelfLayer()) {
    ObjectLayer::CreateLayer();
  }
}

bool SortByZorder(Object *p1, Object *p2) {
  return p1->GetZorder() < p2->GetZorder();
}

void Object::SortChildByZorder() {
  if (!m_pChild || !m_pChild->m_pNext)
    return;

  // 排序
  std::vector<Object *> vec;
  Object *p = m_pChild;
  while (p) {
    vec.push_back(p);
    p = p->m_pNext;
  }
  std::sort(vec.begin(), vec.end(), SortByZorder);

  // 应用
  std::vector<Object *>::size_type count = vec.size();
  m_pChild = vec[0];
  m_pChild->m_pPrev = nullptr;

  for (uint i = 1; i < count; i++) {
    vec[i]->m_pNext = nullptr;
    vec[i]->m_pPrev = vec[i - 1];

    vec[i - 1]->m_pNext = vec[i];
  }
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               消息映射                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//
//   窗口收到WM_DESTROY消息之后，传递给所有的子对象
//
// void Object::OnDestroy()
//{
//	Object*  pObj = nullptr;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// 向子对象发送WM_DESTROY消息
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}

// void Object::OnPaint( HDC hDC )

// 在接收到OnSize消息的时候，需要改变自己的pos,rectP,rectW成员，
// 并通知自己的子对象去更新
// void Object::OnSize( unsigned int nType, int cx, int cy )
//{
//	this->UpdateRect();
//
//	Object*  pObj = nullptr;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// 向子对象发送WM_ERASEBKGND消息
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}

//
//  根据对象所在的背景，计算对象的文本颜色。用于图片换肤
//
// Two colors provide good color visibility if the brightness difference and the
// color difference between the two colors are greater than a set range. Color
// brightness is determined by the following formula:
// ((Red value X 299) + (Green value X 587) + (Blue value X 114)) / 1000
// Color difference is determined by the following formula:
// (max (Red 1, Red 2) - min (Red 1, Red 2)) + (max (Green 1, Green 2) - min
// (Green 1, Green 2)) + (max (Blue 1, Blue 2) - min (Blue 1, Blue 2))
//  The rage for color brightness difference is 125. The range for color
//  difference is 500.
//
//  google text background color black white
//  http://juicystudio.com/services/aertcolourcontrast.php
//  http://www.colourlovers.com/web/blog/2010/03/11/calculating-color-contrast-for-legible-text
//
#if 0
unsigned int  Object::CalcContrastTextColor()
{
    WindowBase*  pWindowBase = GetWindow();
    if (nullptr == pWindowBase)
        return 0;

	assert (0 && TEXT("TODO"));
	return 0;
#if 0
    HDC hDC = pWindowBase->GetRenderLayer2()->GetRenderTarget()->GetHDC();
    if (nullptr == hDC)
        return 0;

    HBITMAP  hBitmap = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
    if (nullptr == hBitmap)
        return 0;

    Rect rcLayer;
    GetRectInWindow(&rcLayer, true);

    Image  image;
    image.Attach(hBitmap, Image::DIBOR_TOPDOWN);
// #ifdef _DEBUG
//     image.Save(L"C:\\aa.png", Gdiplus::ImageFormatPNG);
// #endif
    unsigned int  dwAverageColor = image.GetAverageColor(&rcLayer);
    image.Detach();

    // 将alpha值应用上
//    int a = dwAverageColor >> 24;
    int r = GetRValue(dwAverageColor);
    int g = GetGValue(dwAverageColor);
    int b = GetBValue(dwAverageColor);

    // TODO ?? 是否需要做这一步，这应该看颜色预乘了没？？
//     if (a)
//     {
//         r = (r<<8)/a;
//         g = (g<<8)/a;
//         b = (b<<8)/a;
// 
//         r = min(255, r);
//         g = min(255, g);
//         b = min(255, b);
//     }

//     int v = ((r * 299) + (g * 587) + (b * 114)) >> 10;
//     if (v < 125)
//         return 0xFFFFFFFF;
//     else
//         return 0xFF000000;

    // 这里得计算亮度，而不是灰色
    int max = max(max(r,g),b);
    int min = min(min(r,g),b);
    int light = (min + max) >> 1;
    if (light < 128)
        return 0xFFFFFFFF;
    else
        return 0xFF000000;
#endif
}
#endif

Object *Object::GetObjectByPos(Point *pt) {
#if 0 // defined(OS_WIN)
  WindowBase *pWindow = GetWindow();
  if (nullptr == pWindow)
    return nullptr;

  return pWindow->GetObjectByPos(this, pt, false);
#else
  UIASSERT(false);
  return nullptr;
#endif
}

bool Object::SetMouseCapture(int nNotifyMsgId) {
  Window *window = GetWindow();
  if (!window)
    return false;

  window->SetMouseCapture(this);
  return true;
}
bool Object::ReleaseMouseCapture() {
  Window *window = GetWindow();
  if (!window)
    return false;

  window->ReleaseMouseCapture(this);
  return true;
}

bool Object::SetKeyboardCapture(int nNotifyMsgId) {
#if 0 // defined(OS_WIN)
  WindowBase *pWindow = GetWindow();
  if (!pWindow)
    return false;

  pWindow->GetMouseMgr()->SetKeyboardCapture(m_pIObject, nNotifyMsgId);
  return true;
#else
  UIASSERT(false);
  return false;
#endif
}
bool Object::ReleaseKeyboardCapture() {
#if 0 // defined(OS_WIN)
  WindowBase *pWindow = GetWindow();
  if (!pWindow)
    return false;

  pWindow->GetMouseMgr()->ReleaseKeyboardCapture(m_pIObject);
  return true;
#else
  UIASSERT(false);
  return false;
#endif
}

void Object::OnLayerDestory() {
  m_objStyle.layer = false;

  // 通知父layer更新缓存，子对象有自己的缓存，或者需要缓存子对象
  if (m_pParent) {
    m_pParent->Invalidate(&m_rcParent);
  }
}
void Object::OnLayerCreate() {
  // 通知父layer更新缓存，子对象有自己的缓存，或者需要缓存子对象
  if (m_pParent) {
    m_pParent->Invalidate(&m_rcParent);
  }
}

void Object::OnLayerStyleChanged(bool new_layer_style) {
  if (new_layer_style) {
    ObjectLayer::CreateLayer();
  } else {
    ObjectLayer::TryDestroyLayer();
  }
}

// 辅助函数
unsigned int Object::GetChildCount() {
  int lCount = 0;
  Object *p = this->GetChildObject();
  while (p) {
    lCount++;
    p = p->GetNextObject();
  }
  p = this->GetNcChildObject();
  while (p) {
    lCount++;
    p = p->GetNextObject();
  }

  return lCount;
}

Object *Object::GetChildObjectByIndex(unsigned int lIndex) {
  unsigned int lCount = 0;
  Object *p = this->GetChildObject();
  while (p) {
    if (lCount == lIndex)
      return p;

    lCount++;
    p = p->GetNextObject();
  }
  p = this->GetNcChildObject();
  while (p) {
    if (lCount == lIndex)
      return p;
    lCount++;
    p = p->GetNextObject();
  }

  return nullptr;
}

unsigned int Object::GetChildObjectIndex(Object *pChild) {
  unsigned int lCount = 0;
  Object *p = this->GetChildObject();
  while (p) {
    if (p == pChild)
      return lCount;

    lCount++;
    p = p->GetNextObject();
  }
  p = this->GetNcChildObject();
  while (p) {
    if (p == pChild)
      return lCount;
    lCount++;
    p = p->GetNextObject();
  }

  return 0;
}

const Rect& ObjectProp::GetPadding() {
  return m_property_store.GetRect(OBJECT_PADDING);
}
const Rect& ObjectProp::GetMargin() {
  return m_property_store.GetRect(OBJECT_MARGIN);
}
const Rect& ObjectProp::GetBorder() {
  return m_property_store.GetRect(OBJECT_BORDER);
}
const Rect& ObjectProp::GetExtNonClient() { 
  return m_property_store.GetRect(OBJECT_EXT_NONCLIENT);
}

void ObjectProp::SetPadding(const Rect& rect) {
  m_property_store.SetRect(OBJECT_PADDING, rect);
}
void ObjectProp::SetMargin(const Rect& rect) {
  m_property_store.SetRect(OBJECT_MARGIN, rect);
}
void ObjectProp::SetBorder(const Rect& rect) {
  m_property_store.SetRect(OBJECT_BORDER, rect);
}
void ObjectProp::SetExtNonClient(const Rect&  rect) { 
  m_property_store.SetRect(OBJECT_EXT_NONCLIENT, rect);
}

//
// 获取该对象的偏移量
//
// ReturnQ
//		返回false表示该对象无滚动数据
//
bool Object::GetScrollOffset(int *pxOffset, int *pyOffset) {
  if (nullptr == pxOffset || nullptr == pyOffset)
    return false;

  *pxOffset = 0;
  *pyOffset = 0;

  if (m_objStyle.hscroll || m_objStyle.vscroll) {
    GetScrollOffsetMessage msg;
    RouteMessage(&msg);

    *pxOffset = msg.x_offset;
    *pyOffset = msg.x_offset;
    return true;
  }
  return false;
}

bool Object::GetScrollRange(int *pxRange, int *pyRange) {
  if (nullptr == pxRange || nullptr == pyRange)
    return false;

  *pxRange = 0;
  *pyRange = 0;

  if (m_objStyle.hscroll || m_objStyle.vscroll) {
    GetScrollRangeMessage msg;
    RouteMessage(&msg);
    
    *pxRange = msg.x_range;
    *pyRange = msg.y_range;
    return true;
  }
  return false;
}


#if 0 // defined(OS_WIN)
// 外部添加IUIAccessible的接口实现类
class UIAccessibleCreator : public IUIAccessibleCreator {
public:
  UIAccessibleCreator(AccessibleImpl *p) { m_pAccessible = p; }
  virtual void Add(IUIAccessible *p) override {
    if (m_pAccessible)
      m_pAccessible->AddUIAccessible(p);
  }

private:
  AccessibleImpl *m_pAccessible;
};

bool Object::CreateAccesible(IAccessible **pp) {
  if (!pp)
    return false;

  if (!m_pAccessible) {
    AccessibleImpl *p = new AccessibleImpl;
    m_pAccessible = static_cast<IAccessible *>(p);
    m_pAccessible->AddRef();

    UIAccessibleCreator creator(p);

    // 添加其它accessible
    UISendMessage(m_pIObject, UI_WM_CREATE_ACCESSIBLE,
                  (int)static_cast<IUIAccessibleCreator *>(&creator), 0);

    // 添加默认的object accessible
    // 内部会调用addref

    // TODO1: Object销毁时，怎么确保这些接口也销毁，而不仅仅是release？
    // TODO2: 在使用触摸屏滚动页面时，这里居然会造成内存泄露，无法理解
    p->AddUIAccessible(new ObjectAccessible(*this));
  }

  *pp = m_pAccessible;
  m_pAccessible->AddRef();
  return true;
}
#endif
