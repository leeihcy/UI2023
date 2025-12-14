#include "object_layer.h"
#include "include/interface/iobject.h"
#include "include/macro/msg.h"
#include "object.h"
// #include "src/layer/windowrender.h"
#include "src/window/window.h"
// #include "Src/UIObject/ListCtrl/ListItemBase/listitembase.h"

namespace ui {

ObjectLayer::~ObjectLayer() { DestroyLayer(); }

void ObjectLayer::CreateLayer() {
  Object& self = *static_cast<Object*>(this);
  if (m_pLayer)
    return;

  WindowRender *pWndRender = nullptr;

  Window *pWindow = self.GetWindow();
  if (pWindow)
    pWndRender = &pWindow->GetWindowRender();

  if (pWndRender) {
    m_pLayer =
        pWndRender->CreateLayer(static_cast<IObjectLayerContent *>(this));

    Rect rcParent;
    self.GetParentRect(&rcParent);
    if (!rcParent.IsEmpty()) {
      m_pLayer->OnSize(rcParent.Width(), rcParent.Height(), pWindow->m_dpi.GetScaleFactor());
    }

    SerializeParam data = {0};
    data.resource = self.GetIResource();
    data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
    data.attribute_map = self.GetMapAttribute().get();

    m_pLayer->Serialize(&data);

    self.OnLayerCreate();
  } else {
    // 在resize的时候创建
    // UIASSERT(0);// 有可能窗口正在退了销毁期间，逻辑又调用了一次动画。
  }
}

void ObjectLayer::TryDestroyLayer() {
  if (!m_pLayer)
    return;

  m_pLayer->TryDestroy();
}

void ObjectLayer::DestroyLayer() {
  if (!m_pLayer)
    return;

  Layer *p = m_pLayer;
  m_pLayer = nullptr;

  if (p)
    p->Destroy();
}

Object& ObjectLayer::GetLayerContentObject() {
  return *static_cast<Object*>(this);
}

void ObjectLayer::LayerDraw(IRenderTarget *pRenderTarget) {
  Object& self = *static_cast<Object*>(this);
  self.DrawToLayer__(pRenderTarget);
}

void ObjectLayer::GetLayerWindowRect(Rect *prcOut) { 
  Object& self = *static_cast<Object*>(this);
  self.GetWindowRect(prcOut); 
}

void ObjectLayer::GetLayerParentWindowRect(Rect *prcOut) {
  Object& self = *static_cast<Object*>(this);
  if (self.GetParentObject())
    self.GetParentObject()->GetWindowRect(
        prcOut); // TODO: -->> visible part only
}

void ObjectLayer::OnSize(uint width, uint height, float scale) {
  if (m_pLayer) {
    m_pLayer->OnSize(width, height, scale);
  }
}

bool ObjectLayer::IsChildOf(ILayerContent *pParentLayer) {
  Object& self = *static_cast<Object*>(this);

  if (!pParentLayer)
    return false;

  if (pParentLayer->GetLayerContentType() == LayerContentTypeObject) {
    Object &parentObject =
        *static_cast<Object*>(static_cast<ObjectLayer *>(pParentLayer));

    return parentObject.IsMyChild(&self, true);
  } else if (pParentLayer->GetLayerContentType() == LayerContentTypeListItem) {
    assert(false);
#if 0 // TODO:
    Panel *panel = static_cast<IListItemLayerContent *>(pParentLayer)
                       ->GetListItem()
                       .GetRootPanel();
    if (!panel)
      return false;

    return panel->IsMyChild(&m_obj, true);
#endif
  } else {
    UIASSERT(0);
  }
  return false;
}

bool ObjectLayer::IsLayerVisible() { 
  Object& self = *static_cast<Object*>(this);
  return self.IsVisible(); 
}

float ObjectLayer::GetLayerScale() {
  Object& self = *static_cast<Object*>(this);

  float scale = 1.0f;
  Window *window = self.GetWindow();
  if (window) {
    scale = window->m_dpi.GetScaleFactor();
  }
  return scale;
}

// object在对象树中的位置改变了，同步到分层树上面
void ObjectLayer::OnObjPosInTreeChanged() {
  UIASSERT(m_pLayer);

  Layer *pParentLayer = m_pLayer->GetParent();
  UIASSERT(pParentLayer);

  m_pLayer->RemoveMeInTheTree();
  pParentLayer->AddSubLayer(m_pLayer, GetNextLayer());
}

void ObjectLayer::OnLayerDestory() {
  Object& self = *static_cast<Object*>(this);
  if (m_pLayer) // 由ObjectLayer::~ObjectLayer()触发的，不通知
  {
    m_pLayer = nullptr;
    self.OnLayerDestory();
  }
}

Layer *ObjectLayer::GetParentLayer() {
  Object& self = *static_cast<Object*>(this);
  Object *parent = self.GetParentObject();
  if (!parent)
    return nullptr;

  return parent->GetLayer();
}

// Layer Tree 生成关键函数，参照webkit：
// RenderLayer* RenderObject::findNextLayer(RenderLayer* parentLayer,
// RenderObject* startPoint, bool checkParent)
// http://opensource.apple.com//source/WebCore/WebCore-658.28/rendering/RenderObject.cpp
// 获取pStartObj的layer应该插入在pParentLayer的哪个位置
// 注：这里并没有处理自己的子对象已经有layer在pParentLayer下面的情况，需要单独处理
// Layer*  Object::FindNextLayer(
//             Layer* pParentLayer,
//             Object* pStartObj,
//             bool bCheckParent)
// {
//     if (!pParentLayer)
//         return nullptr;
//
//     Layer* pThisLayer = m_pLayer ? m_pLayer->GetLayer() : nullptr;
//
//     // 找到一个同级的layer，这个对象是在pStartObj的后面，因此可以将pStartObj
//     // 对应的layer插入到这个对象的layer前面。
//     if (pThisLayer && pThisLayer->GetParent() == pParentLayer)
//         return pThisLayer;
//
//     // 如果这个对象没有layer，则向后递归遍历
//     if (!pThisLayer || pThisLayer == pParentLayer)
//     {
//         Object* p = pStartObj ? pStartObj->GetNextObject() :
//         GetChildObject(); for (; p; p = p->GetNextObject())
//         {
//             Layer* pReturn = p->FindNextLayer(
//                 pParentLayer, nullptr, false);
//
//             if (pReturn)
//                 return pReturn;
//         }
//     }
//
//     // 找到顶了，没有其它layer
//     if (pThisLayer == pParentLayer)
//         return nullptr;
//
//     // 向上找
//     if (m_pParent && bCheckParent)
//         return m_pParent->FindNextLayer(pParentLayer, this, true);
//
//     return nullptr;
// }

// 判断思路：这个对象的下一个层，只会在这个对象的下一个Object中出现。
Layer *ObjectLayer::GetNextLayer() {
  Object& self = *static_cast<Object*>(this);
  Layer *pParentLayer = GetParentLayer();

  Object *pNextTreeObject = self.GetNextTreeItemObject();
  while (pNextTreeObject) {
    Layer *pThisLayer = pNextTreeObject->GetSelfLayer();

    // 找回到父layer了，说明自己就是最后一个，没有next
    if (pThisLayer && pThisLayer == pParentLayer)
      return nullptr;

    // 在后面的结点中，找到了一个同级层，插在它的前面
    if (pThisLayer && pThisLayer->GetParent() == pParentLayer)
      return pThisLayer;

    // 否则继续往下一个树结点遍历
    pNextTreeObject = pNextTreeObject->GetNextTreeItemObject();
  }

  return nullptr;
}

} // namespace ui