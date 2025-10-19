#include "object.h"
#include "include/interface/imessage.h"
#include "include/interface/imeta.h"
#include "include/interface/iobject.h"
#include "include/macro/msg.h"
#include "include/macro/uidefine.h"
#include "object_layer.h"
#include "include/interface/ilayout.h"
#include "include/interface/iwindow.h"
// #include "include/interface/ipanel.h"
#include "src/layout/canvaslayout.h"
#include "src/object/layout/layout_object.h"
#include "src/resource/uiresource.h"
// #include "src/Renderbase\renderbase\renderbase.h"
// #include "src/Renderbase\textrenderbase\textrender.h"
// #include "src/UIObject\Window\windowbase.h"
// #include "src/UIObject\HwndHost\HwndHost.h"
// #include "src/resource/uicursor.h"
// #include "src/layout/layout.h"
#include "include/interface/iuires.h"
#include "include/interface/ixmlwrap.h"
#include "src/application/uiapplication.h"
#include "src/window/window.h"
#include "src/panel/root_object.h"
#include "src/resource/colorres.h"
#include "src/resource/cursorres.h"
#include "src/resource/res_bundle.h"
#include "src/resource/stylemanager.h"
// #include "src/Atl\image.h"
#include "include/interface/imapattr.h"
#include <algorithm>
#include <memory>
// #include "..\Accessible\accessibleimpl.h"
// #include "..\Accessible\object_accessible.h"
#include "object_meta.h"

using namespace ui;

Object::Object(IObject *p) : ObjTree(p), m_objLayer(*this), layout(*this) {
  m_pIObject = p;
  // m_lCanRedrawRef = 0;
  m_pIMapAttributeRemain = nullptr;
#if 0 // defined(OS_WIN)
  m_pAccessible = nullptr;
#endif

  memset(&m_objStyle, 0, sizeof(m_objStyle));
  memset(&m_objState, 0, sizeof(m_objState));
  m_objState.visibility_ = VISIBILITY_VISIBLE;
}

// 注意：不要在构造或者析构函数中调用虚函数

Object::~Object(void) {
  if (m_ppOutRef)
    *m_ppOutRef = nullptr;

#if 0 // defined(OS_WIN)
  if (m_pAccessible)
    SAFE_RELEASE(m_pAccessible);
#endif
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
  if (msg->message == UI_MSG_FINALRELEASE) {
    FinalRelease();
    return;
  }
  if (msg->message == UI_MSG_SERIALIZE) {
    onSerialize(static_cast<SerializeMessage*>(msg)->param);
    return;
  }
}

int Object::FinalConstruct(IResource *pSkinRes) {
  m_resource = pSkinRes->GetImpl();
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
  m_objLayer.DestroyLayer();
}

IObject *Object::GetIObject() { return m_pIObject; }

// 注：如果在其它模块直接调用 pCtrl->m_strID=L"..."的话，在对象释放时将会崩溃
void Object::SetId(const char *szText) {
  if (szText)
    m_strId = szText;
  else
    m_strId.clear();
}
const char *Object::GetId() { return m_strId.c_str(); }

Layer *Object::GetSelfLayer() const { return m_objLayer.GetLayer(); }

// 获取对象所在Layer
Layer *Object::GetLayer() {
  ObjectLayer *pLayer = GetLayerEx();
  if (pLayer)
    return pLayer->GetLayer();

  return nullptr;
}

// 为动画准备一个layer，如果没有则创建layer
Layer *Object::GetLayerForAnimate() {
  Layer *layer = GetSelfLayer();
  if (layer)
    return layer;

  m_objLayer.CreateLayer();
  return GetSelfLayer();
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

#if 0 // defined(OS_WIN)
HWND Object::GetHWND() {
  WindowBase *pWindow = this->GetWindow();
  if (!pWindow)
    return nullptr;

  return pWindow->GetHWND();
}
#endif

ObjectLayer *Object::GetLayerEx() {
  Object *pObj = this;
  while (pObj) {
    if (pObj->GetSelfLayer())
      return &pObj->m_objLayer;

    pObj = pObj->m_pParent;
  }

  return nullptr;
}

// 获取自己所在层的层对象
Object *Object::GetLayerCreator() {
  ObjectLayer *p = GetLayerEx();
  if (p)
    return &p->GetObject();

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
    UI_LOG_WARN("Find \"%s\" from \"%s\" failed.", szObjId,
                this->m_strId.c_str());
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

void Object::LoadAttributes(bool bReload) {
  if (!m_pIMapAttributeRemain)
    return;

  std::string strStyle;
  std::string strId;

  const char *szText = m_pIMapAttributeRemain->GetAttr(XML_STYLECLASS, false);
  if (szText)
    strStyle = szText;

  szText = m_pIMapAttributeRemain->GetAttr(XML_ID, false);
  if (szText)
    strId = szText;

  StyleRes &styleRes = m_resource->GetStyleRes();
  styleRes.LoadStyle(m_meta->Name(), strStyle.c_str(),
                     strId.c_str(), m_pIMapAttributeRemain.get());

  SerializeParam data = {0};
  data.resource = m_resource->GetIResource();
  data.pMapAttrib = m_pIMapAttributeRemain.get();
  data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
  if (bReload)
    data.nFlags |= SERIALIZEFLAG_RELOAD;

  SerializeMessage msg;
  msg.param = &data;
  m_pIObject->RouteMessage(&msg);

  // 如果没有多余的属性，直接释放，节省内存
  if (m_pIMapAttributeRemain && 0 == m_pIMapAttributeRemain->GetAttrCount()) {
    m_pIMapAttributeRemain.reset();
  }
}

// 从xml获取自己的属性
void Object::LoadAttributeFromXml(UIElement *pElement, bool bReload) {
  if (!pElement)
    return;

  m_pIMapAttributeRemain.reset();
  m_pIMapAttributeRemain = UICreateIMapAttribute();
  pElement->GetAttribList(m_pIMapAttributeRemain.get());
  this->LoadAttributes(bReload); 

  // 通知编辑器关联控件和xml结点.
  // 将通知放在这里，而不是layoutmanager中，是为了解决复合控件中的子控件加载问题
  // 这种子控件不会在layoutmanager中被加载属性，而是由复合控件自己解决，最终调用
  // 到子控件的SetAttributeFromXmlElement函数，因此将通知放在这里。
  IUIEditor *pEditor = GetUIApplication()->GetUIEditorPtr();
  if (pEditor) {
    pEditor->OnObjectAttributeLoad(m_pIObject, pElement->GetIUIElement());
  }
}

// 获取一个未解析的属性。如果bErase==true，则将返回一个临时的字符串指针，调用者应该尽快保存或者仅临时使用
const char *Object::GetAttribute(const char *szKey, bool bErase) {
  if (nullptr == szKey || nullptr == m_pIMapAttributeRemain)
    return nullptr;

  return m_pIMapAttributeRemain->GetAttr(szKey, bErase);
}
void Object::AddAttribute(const char *szKey, const char *szValue) {
  if (nullptr == m_pIMapAttributeRemain) {
    m_pIMapAttributeRemain = UICreateIMapAttribute();
  }
  m_pIMapAttributeRemain->AddAttr(szKey, szValue);
}
std::shared_ptr<IMapAttribute> Object::GetMapAttribute() {
  return m_pIMapAttributeRemain;
}
void Object::ClearMapAttribute() {
  m_pIMapAttributeRemain.reset();
}

// 设置padding的值，同时更新非客户区的大小
void Object::SetPaddingRegion(REGION4 *prc) { m_rcPadding.CopyFrom(*prc); }

void Object::SetBorderRegion(Rect *prc) { m_rcBorder.CopyFrom(*prc); }

// void*  Object::QueryInterface(const IID* pIID)
// {
//     if (IsEqualIID(*pIID, uiiidof(IObject)) ||
//         IsEqualIID(*pIID, uiiidof(IMessage)) )
//     {
//         return m_pIObject;
//     }
//
//     return nullptr;
// }
//
//
// void  Object::OnCreateByEditor(CREATEBYEDITORDATA* pData)
// {
//     //
//     在编辑框中创建的对象，也给创建一个空属性列表，用于序列化默认的TEXTSTYLE等。
//     assert(nullptr == m_pIMapAttributeRemain);
//     m_pIMapAttributeRemain = UICreateIMapAttribute();
// }
//

// TODO: 优化
//
//	设置Object的默认鼠标样式
//
//	Remark:
//		如果不实现该消息，可能导致鼠标移动到一个Object上面时，仍然显示的是Window的OnSetCursor
//
//
// bool Object::OnSetCursor(HWND hWnd, unsigned int nHitTest, unsigned int
// message)
// {
// 	if (nullptr == m_pCursor || nullptr == m_pCursor->GetCursor())
// 	{
// 		::SetCursor(::LoadCursor(nullptr, IDC_ARROW));
//  		return true;
// 	}
//
// 	::SetCursor(m_pCursor->GetCursor());
// 	return true;
// }

//	获取自己的字体,这里返回的对象只是一个临时对象，
//	如果需要保存使用，则需要调用AddRef
// IRenderFont* Object::GetRenderFont()
// {
//     IRenderFont* pRenderFont = nullptr;
//
//     // 向自己的textrender获取
//     if (m_text_render)
//         pRenderFont = m_text_render->GetRenderFont();
//
//     if (pRenderFont)
//         return pRenderFont;
//
//     // 向窗口获取默认
//     WindowBase* pWindow = GetWindow();
//     if (pWindow)
//         return pWindow->GetWindowDefaultRenderFont();
//
//     return nullptr;
// }

//
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

  // 	if (this->m_hRgn)
  // 	{
  //      UIASSERT(0);  // 未实现
  // 		bIn = ::PtInRegion(m_hRgn, ptInParent->x, ptInParent->y);
  //      if (bIn)
  // 		    return HTCLIENT;
  // 	}

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
    ;
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

void Object::ModifyObjectStyle(OBJSTYLE *add, OBJSTYLE *remove) {

#define __REMOVE(x)                                                            \
  if (remove->x)                                                               \
    m_objStyle.x = 0;
#define __ADD(x)                                                               \
  if (add->x)                                                                  \
    m_objStyle.x = 1;

  if (add) {
    __ADD(initialized);
    __ADD(transparent);
    __ADD(float_on_parent_content);
    __ADD(post_paint);
    __ADD(hscroll);
    __ADD(vscroll);
    __ADD(ncobject);
    __ADD(reject_all_mouse_msg);
    __ADD(reject_self_mouse_msg);
    __ADD(receive_dragdrop_event);
    __ADD(enable_ime);
    __ADD(zindex_overlap);
    __ADD(noclip);
    __ADD(clip_client);
    __ADD(tabstop)
    __ADD(want_tab)
    __ADD(want_return)

    if (add->layer)
      m_objLayer.CreateLayer();
      
    // 默认值为1时，如果没有在xml中配置，不会触发setter函数
    // 因此在设置默认值的时候，应该同步一次该值
    if (add->default_ncobject) {
      m_objStyle.default_ncobject = 1;
      m_objStyle.ncobject = 1;
    }
    if (add->default_reject_all_mouse_msg) {
      m_objStyle.default_reject_all_mouse_msg = 1;
      m_objStyle.reject_all_mouse_msg = 1;
    }
    if (add->default_reject_self_mouse_msg) {
      m_objStyle.default_reject_self_mouse_msg = 1;
      m_objStyle.reject_self_mouse_msg = 1;
    }
    if (add->default_transparent) {
      m_objStyle.default_transparent = 1;
      m_objStyle.transparent = 1;
    }
    if (add->default_tabstop) {
      m_objStyle.default_tabstop = 1;
      m_objStyle.tabstop = 1;
    }
  }

  if (remove) {
    __REMOVE(initialized);
    __REMOVE(transparent);
    __REMOVE(float_on_parent_content);
    __REMOVE(post_paint);
    __REMOVE(hscroll);
    __REMOVE(vscroll);
    __REMOVE(ncobject);
    __REMOVE(reject_all_mouse_msg);
    __REMOVE(reject_self_mouse_msg);
    __REMOVE(receive_dragdrop_event);
    __REMOVE(enable_ime);
    __REMOVE(zindex_overlap);
    __REMOVE(noclip);
    __REMOVE(clip_client);
    __REMOVE(tabstop);
    __REMOVE(want_tab)
    __REMOVE(want_return)

    if (remove->layer) {
      m_objLayer.TryDestroyLayer();
    }

    if (remove->default_ncobject) {
      m_objStyle.default_ncobject = 0;
      m_objStyle.ncobject = 0;
    }
    if (remove->default_reject_all_mouse_msg) {
      m_objStyle.default_reject_all_mouse_msg = 0;
      m_objStyle.reject_all_mouse_msg = 0;
    }
    if (remove->default_reject_self_mouse_msg) {
      m_objStyle.default_reject_self_mouse_msg = 0;
      m_objStyle.reject_self_mouse_msg = 0;
    }
    if (remove->default_transparent) {
      m_objStyle.default_transparent = 0;
      m_objStyle.transparent = 0;
    }
    if (remove->default_tabstop) {
      m_objStyle.default_tabstop = 0;
      m_objStyle.tabstop = 0;
    }
  }
}

bool Object::TestObjectStyle(const OBJSTYLE &test) {

#define TEST(x)                                                                \
  if (test.x && !m_objStyle.x)                                                 \
    return false;

  TEST(transparent);
  TEST(float_on_parent_content);
  TEST(post_paint);
  TEST(hscroll);
  TEST(vscroll);
  TEST(ncobject);
  TEST(reject_all_mouse_msg);
  TEST(reject_self_mouse_msg);
  TEST(receive_dragdrop_event);
  TEST(enable_ime);
  TEST(zindex_overlap);
  TEST(noclip);
  TEST(clip_client);
  TEST(tabstop);
  TEST(want_tab);
  TEST(want_return);
  TEST(layer);

  return true;
}

// void Object::ModifyStyleEx(unsigned int nStyleAdd, unsigned int nStyleRemove,
// bool bNotify)
// {
//     STYLESTRUCT s;
//     s.styleOld = m_nStyle2;
//     s.styleNew = m_nStyle2;
//
//     if (nStyleAdd != 0)
//     {
//         s.styleNew |= nStyleAdd;
//     }
//     if (nStyleRemove != 0)
//     {
//         s.styleNew &= ~nStyleRemove;
//     }
//
//     if (s.styleNew == s.styleOld)
//         return;
//
//     if (bNotify)
//     {
//         ::UISendMessage(this, WM_STYLECHANGING, (int)GWL_EXSTYLE, (int)&s);
//     }
//
//     m_nStyle2 = s.styleNew;
//
//     if (bNotify)
//     {
//         s.styleOld = s.styleOld;
//         ::UISendMessage(this, WM_STYLECHANGED, (int)GWL_EXSTYLE, (int)&s);
//     }
// }

bool Object::IsTransparent() {
  if (m_objStyle.transparent)
    return true;

  return false;
}
void Object::SetTransparent(bool b) { m_objStyle.transparent = b; }

bool Object::IsNoClip() { return !NeedClip(); }

bool Object::NeedClip() {
  if (m_objStyle.noclip)
    return false;

  //  TODO: 需要重写。
  // 	//
  // 如果子对象是一个层，并且有变换，则不更新剪裁区域，而是切换到父对象的剪裁区域
  // 	// 2015.9.23 add
  // 如果是硬件合成模式，则不用管，层与层之间的数据没有关联。 	if
  // (m_pRenderLayer
  // && m_pRenderLayer->HasTransform())
  // 	{
  // 		WindowBase* pWindow = GetWindow();
  // 		if (!pWindow || !pWindow->IsGpuComposite())
  // 			return false;
  // 	}
  //
  return true;
}

void Object::SetNoClip(bool b) { m_objStyle.noclip = b; }

void Object::SetClipClient(bool b) { m_objStyle.clip_client = b; }
bool Object::NeedClipClient() { return m_objStyle.clip_client; }

bool Object::IsFocus() { return m_objState.focus; }

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

bool Object::IsTabstop() { return m_objStyle.tabstop; }

bool Object::CanTabstop() {
  return this->IsTabstop() && this->IsEnable() && this->IsVisible();
}

void Object::SetTabstop(bool b) { m_objStyle.tabstop = b; }

bool Object::IsSelfVisible() {
  bool bVisible = m_objState.visibility_ == VISIBILITY_VISIBLE ? true : false;
  return bVisible;
}

bool Object::IsVisible() {
  bool bVisible = m_objState.visibility_ == VISIBILITY_VISIBLE ? true : false;
  if (false == bVisible || nullptr == m_pParent) {
    return bVisible;
  }
  // 判断父对象不可见的情况
  else {
    // 注：在这里不对最外层的窗口进行判断的原因是：在类似于窗口初始化的函数里面，
    // 虽然窗口暂时是不可见的，但里面的对象的IsVisible应该是返回true才好处理
    if (m_pParent->GetMeta() &&
        OBJ_WINDOW == m_pParent->GetMeta()->Detail().major_type) {
      return true;
    } else {
      return m_pParent->IsVisible();
    }
  }
  return true;
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

bool Object::IsEnable() { return !m_objState.disable; }

void Object::LoadVisibleEx(int l) {
  m_objState.visibility_ = static_cast<VISIBILITY_TYPE>(l);
}
int Object::SaveVisibleEx() { return m_objState.visibility_; }

void Object::SetVisible(bool b) {
  SetVisibleEx(b ? VISIBILITY_VISIBLE : VISIBILITY_COLLAPSED);
}

void Object::virtualSetVisibleEx(VISIBILITY_TYPE eType) {}

//
//	[static]
//   给pParent的子孙对象递归转发消息
//
void Object::ForwardMessageToChildObject(Object *pParent, ui::Msg *message) {
  Object *pChild = nullptr;
  while ((pChild = pParent->EnumChildObject(pChild))) {
    pChild->RouteMessage(message);
    Object::ForwardMessageToChildObject(pChild, message);
  }

  Object *pNcChild = nullptr;
  while ((pNcChild = pParent->EnumNcChildObject(pNcChild))) {
    pNcChild->RouteMessage(message);
    Object::ForwardMessageToChildObject(pNcChild, message);
  }
}

// 发给父对象，再发给子对象，最后再通知一次父对象
// 应用场景：UI_WM_INITIALIZE/UI_WM_INITIALIZE2
// 注：如果已经初始化过了的控件，不再发送消息，如换肤（换布局）过程中有些控件不重新创建，
//    但是还会发送一个bindind通知，用于重新绑定新的子控件，例如重新绑定滚动条
void Object::ForwardInitializeMessageToDecendant(Object *pObj) {
  DoBindPlzMessage bind_message;
  bind_message.bind = true;

  Object *pChild = nullptr;
  while ((pChild = pObj->EnumAllChildObject(pChild))) {

    if (pChild->m_objStyle.initialized) {
      Object::ForwardInitializeMessageToDecendant(pChild);
    } else {
      pChild->m_objStyle.initialized = 1;

      pChild->virtualOnLoad();

      pChild->RouteMessage(UI_MSG_INITIALIZE);
      Object::ForwardInitializeMessageToDecendant(pChild);
      pChild->RouteMessage(UI_MSG_INITIALIZE2);
    }

    pChild->RouteMessage(&bind_message);
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
  Object::ForwardMessageToChildObject(this, &message);

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
        scale
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

void Object::SetDisableDirect(bool b) { m_objState.disable = b; }
bool Object::IsSelfDisable() { return m_objState.disable; }

void Object::virtualSetEnable(bool b) { SetDisableDirect(!b); }

void Object::SetEnable(bool b, bool bNoitfy) {
  bool bOld = IsEnable();

  virtualSetEnable(b);

  if (bNoitfy && b != bOld) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_DISABLE;
    RouteMessage(&message);
  }

  if (b != bOld) {
    // [注] 如果没有指定刷新，则需要外部显示调用UpdateObject，因为该控件所在层
    //      并没有设置为dirty，直接刷新整个窗口也不会更新该控件
    this->Invalidate();

#if 0 // defined(OS_WIN)
    // 重新发送鼠标消息，例如鼠标正好位于该控件上面，则需要将该控件设置为hover，否则点击无效
    Point pt = {0, 0};
    ::GetCursorPos(&pt);
    HWND hWnd = GetHWND();
    ::MapWindowPoints(nullptr, hWnd, &pt, 1);
    ::PostMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
#endif
  }
}

bool Object::IsDefault() { return m_objState.default_; }

// bool Object::IsReadonly()
// {
// 	return this->testStateBit(OSB_READONLY);
// }

bool Object::IsHover() { return m_objState.force_hover || m_objState.hover; }

bool Object::IsPress() { return m_objState.force_press || m_objState.press; }

bool Object::IsForceHover() { return m_objState.force_hover; }

bool Object::IsForcePress() { return m_objState.force_press; }
bool Object::IsSelected() { return m_objState.selected; }

void Object::SetDefault(bool b, bool bNotify) {
  if (m_objState.default_ == b)
    return;

  m_objState.default_ = b;
  if (bNotify) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_DEFAULT;
    RouteMessage(&message);
  }
}

void Object::SetSelected(bool b, bool bNotify) {
  if (m_objState.selected == b)
    return;

  m_objState.selected = b;
  if (bNotify) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_SELECTED;
    RouteMessage(&message);
  }
}

void Object::SetForceHover(bool b, bool bNotify) {
  if (m_objState.force_hover == b)
    return;

  bool bOldHover = IsHover();

  m_objState.force_hover = b;
  if (bNotify && IsHover() != bOldHover) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_HOVER;
    RouteMessage(&message);
  }
}

void Object::SetForcePress(bool b, bool bNotify) {
  if (m_objState.force_press == b)
    return;

  bool bOldPress = IsPress();

  m_objState.force_press = b;
  if (bNotify && IsPress() != bOldPress) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_PRESS;
    RouteMessage(&message);    
  }
}

void Object::SetHover(bool b, bool bNotify) {
  if (m_objState.hover == b)
    return;

  m_objState.hover = b;
  if (bNotify) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_HOVER;
    RouteMessage(&message);  
  }
}

void Object::SetPress(bool b, bool bNotify) {
  if (m_objState.press == b)
    return;

  m_objState.press = b;
  if (bNotify) {
    StateChangedMessage message;
    message.state_changed_mask = OSB_PRESS;
    RouteMessage(&message);  
  }
}

void Object::SetAsNcObject(bool b) { m_objStyle.ncobject = b; }
bool Object::IsNcObject() { return m_objStyle.ncobject; }
bool Object::IsRejectMouseMsgAll() { return m_objStyle.reject_all_mouse_msg; }
bool Object::IsRejectMouseMsgSelf() { return m_objStyle.reject_self_mouse_msg; }
void Object::SetRejectMouseMsgAll(bool b) {
  m_objStyle.reject_all_mouse_msg = b;
}
void Object::SetRejectMouseMsgSelf(bool b) {
  m_objStyle.reject_self_mouse_msg = b;
}

// 当手动创建一个对象（非从xml中加载时，可以调用该函数完全默认属性的加载)
void Object::InitDefaultAttrib() {
  std::shared_ptr<IMapAttribute> pMapAttrib;
  if (m_pIMapAttributeRemain) {
    pMapAttrib = m_pIMapAttributeRemain;
  } else {
    pMapAttrib = UICreateIMapAttribute();
  }

  UIASSERT(m_strId.empty() && "将setid放在该函数之后调用，避免覆盖");
  // pMapAttrib->AddAttr(XML_ID, m_strId.c_str()); // 防止id被覆盖??

  // 解析样式
  UIASSERT(m_resource);

  StyleRes &styleRes = m_resource->GetStyleRes();

  const char *szStyle = pMapAttrib->GetAttr(nullptr, XML_STYLE, true);
  std::string strStyle;
  if (szStyle)
    strStyle = szStyle;

  styleRes.LoadStyle(m_meta->Name(), strStyle.c_str(), nullptr,
                     pMapAttrib.get());

  SerializeParam data = {0};
  data.resource = m_resource->GetIResource();
  data.pMapAttrib = pMapAttrib.get();
  data.nFlags = SERIALIZEFLAG_LOAD;

  m_pIMapAttributeRemain = pMapAttrib;
  pMapAttrib = nullptr;

  SerializeMessage msg;
  msg.param = &data;
  m_pIObject->RouteMessage(&msg);

  // 如果没有多余的属性，直接释放，节省内存
  if (m_pIMapAttributeRemain && 0 == m_pIMapAttributeRemain->GetAttrCount()) {
    m_pIMapAttributeRemain.reset();
  }
}
//
// void Object::SetUserData(LPVOID p)
// {
// 	m_pUserData = p;
// }
//
// LPVOID Object::GetUserData()
// {
// 	return m_pUserData;
// }

void Object::SetOutRef(void **ppOutRef) { m_ppOutRef = ppOutRef; }

Resource *Object::GetResource() { return m_resource; }

IResource *Object::GetIResource() {
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

void Object::SetBackRender(std::shared_ptr<IRenderBase> p) {
  m_back_render = p;
}

void Object::SetForegndRender(std::shared_ptr<IRenderBase> p) {
  m_fore_render = p;
}

void Object::SetTextRender(std::shared_ptr<ITextRenderBase> p) {
  m_text_render = p;
}

std::shared_ptr<ITextRenderBase> Object::GetTextRender() { return m_text_render; }
std::shared_ptr<IRenderBase> Object::GetBackRender() { return m_back_render; }
std::shared_ptr<IRenderBase> Object::GetForeRender() { return m_fore_render; }

//	获取自己的字体,这里返回的对象只是一个临时对象，
//	如果需要保存使用，则需要调用AddRef
IRenderFont *Object::GetRenderFont() {
  IRenderFont *pRenderFont = nullptr;

  // 向自己的textrender获取
  if (m_text_render)
    pRenderFont = m_text_render->GetRenderFont();

  if (pRenderFont)
    return pRenderFont;

#if 0 // defined(OS_WIN)
  // 向窗口获取默认
  Window *pWindow = GetWindow();
  if (pWindow)
    return pWindow->GetWindowDefaultRenderFont();
#else
  UIASSERT(0);
#endif
  return nullptr;
}

void Object::load_renderbase(const char *szName, std::shared_ptr<IRenderBase> &pRender) {
  UIASSERT(false);
//   SAFE_RELEASE(pRender);
//   if (szName) {
// #if 0 // defined(OS_WIN)
//     GetUIApplication()->GetRenderBaseFactory().CreateRenderBaseByName(
//         m_resource->GetIResource(), szName, m_pIObject, &pRender);
// #else
//     UIASSERT(false);
// #endif
  // }
}

void Object::load_textrender(const char *szName,
                             std::shared_ptr<ITextRenderBase> &pTextRender) {
                              UIASSERT(false);
//   SAFE_RELEASE(pTextRender);
//   if (szName) {
// #if 0 // defined(OS_WIN)
//     GetUIApplication()->GetTextRenderFactroy().CreateTextRenderBaseByName(
//         m_resource->GetIResource(), szName, m_pIObject, &pTextRender);
// #endif
//   }
}

const char *Object::get_renderbase_name(std::shared_ptr<IRenderBase> &pRender) {
  if (!pRender)
    return nullptr;
#if 0 // defined(OS_WIN)
  return GetUIApplication()->GetRenderBaseFactory().GetRenderBaseName(
      pRender->GetType());
#else
  return nullptr;
#endif
}
const char *Object::get_textrender_name(std::shared_ptr<ITextRenderBase> &pTextRender) {
  if (!pTextRender)
    return nullptr;
#if 0 // defined(OS_WIN)
  return GetUIApplication()->GetTextRenderFactroy().GetTextRenderBaseName(
      pTextRender->GetType());
#else
  return nullptr;
#endif
}

void Object::LoadBkgndRender(const char *szName) {
  load_renderbase(szName, m_back_render);
}
void Object::LoadForegndRender(const char *szName) {
  load_renderbase(szName, m_fore_render);
}
void Object::LoadTextRender(const char *szName) {
  load_textrender(szName, m_text_render);
}
const char *Object::SaveBkgndRender() {
  return get_renderbase_name(m_back_render);
}
const char *Object::SaveForegndRender() {
  return get_renderbase_name(m_fore_render);
}
const char *Object::SaveTextRender() {
  return get_textrender_name(m_text_render);
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

void Object::load_layer_config(bool b) { m_objStyle.layer = b; }

bool Object::HasLayer() { return m_objStyle.layer; }

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

// 序列化辅助函数
/*
void  Object::LoadColor(const char* szColorId, Color*& pColor)
{
    SAFE_RELEASE(pColor);
    if (!szColorId || !m_pUIApplication)
        return;

    ColorRes* pColorRes = m_pUIApplication->GetActiveSkinColorRes();
    if (!pColorRes)
        return;

    pColorRes->GetColor(szColorId, &pColor);
}

const char*  Object::SaveColor(Color*& pColor)
{
    if (!pColor || !m_pUIApplication)
        return nullptr;

    ColorRes* pColorRes = m_pUIApplication->GetActiveSkinColorRes();
    if (pColorRes)
    {
        const char* szId = pColorRes->GetColorId(pColor);
        if (szId)
            return szId;
    }

    char* szBuffer = GetTempBuffer();
    pColor->ToHexString(szBuffer);
    return szBuffer;
}
*/

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
