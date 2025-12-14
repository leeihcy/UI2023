#include "include/interface/ilayout.h"
#include "object.h"

#include "include/interface/iattributemap.h"
#include "include/interface/irenderbase.h"
#include "include/interface/itextrenderbase.h"
#include "include/interface/iuiapplication.h"
#include "src/attribute/attribute.h"
#include "src/attribute/bool_attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/attribute/stringselect_attribute.h"
#include "src/object/object_layout.h"
#include "src/object/object_prop.h"
#include "src/property/property_id.h"

namespace ui {
extern "C" { extern Property* uisdk_property_register; }

ObjectProp::ObjectProp() : m_property_store(uisdk_property_register, this) {
  
}

// 将xml用的配置转成对象的属性，注意，子类重载该函数时，必须先调用父类的该方法
// bReload表示为换肤时调用，此时不能再给某些属性赋值，例如text属性
void ObjectProp::SerializeProperty(SerializeParam *pData) {
  Object* pthis = static_cast<Object*>(this);
  Object& self = *static_cast<Object*>(this);


  IAttributeMap *attribute_map = pData->attribute_map;
  if (pData->IsReload()) {
    m_back_render.reset();
    m_fore_render.reset();
    m_text_render.reset();
    // SAFE_RELEASE(m_pCursor);
  }

  if (pData->IsLoad()) {
    if (nullptr == attribute_map) {
      UI_LOG_ERROR("Invalid Argument.");
      return;
    }
  }

  {
    AttributeSerializer s(pData, "Object");

    // styelclass被修改时，应该重新解析所有属性
    // s.AddString(XML_STYLECLASS, m_strStyleClass)->ReloadOnChanged();
    // s.AddString(XML_TOOLTIP, m_strTooltip);

    s.AddEnum(XML_VISIBLE, Slot(&Object::LoadVisibleEx, pthis),
              Slot(&Object::SaveVisibleEx, pthis))
        ->AddOption(VISIBILITY_COLLAPSED, XML_VISIBILITY_COLLAPSED)
        ->AddOption(VISIBILITY_VISIBLE, XML_VISIBILITY_VISIBLE)
        ->AddOption(VISIBILITY_HIDDEN, XML_VISIBILITY_HIDDEN)
        ->SetDefault(VISIBILITY_VISIBLE)
        ->AsData();

    s.AddBool(XML_DISABLE, Slot(&Object::SetDisableDirect, pthis),
              Slot(&Object::IsSelfDisable, pthis))
        ->AsData();

    s.AddBool(XML_BACKGND_IS_TRANSPARENT, Slot(&Object::SetTransparent, pthis),
              Slot(&Object::IsTransparent, pthis))
        ->SetDefault(self.m_objStyle.default_transparent);

    s.AddBool(XML_NO_CLIP_DRAW, Slot(&Object::SetNoClip, pthis),
              Slot(&Object::IsNoClip, pthis));

    s.AddBool(XML_REJEST_MOUSE_MSG, Slot(&Object::SetRejectMouseMsgAll, pthis),
              Slot(&Object::IsRejectMouseMsgAll, pthis))
        ->SetDefault(self.m_objStyle.default_reject_all_mouse_msg);
    s.AddBool(XML_REJEST_SELF_MOUSE_MSG,
              Slot(&Object::SetRejectMouseMsgSelf, pthis),
              Slot(&Object::IsRejectMouseMsgSelf, pthis))
        ->SetDefault(self.m_objStyle.default_reject_self_mouse_msg);

    s.AddBool(XML_ISNCCHILD, Slot(&Object::SetAsNcObject, pthis),
              Slot(&Object::IsNcObject, pthis))
        ->SetDefault(self.m_objStyle.default_ncobject);

    s.AddBool(XML_LAYER, Slot(&Object::load_layer_config, pthis),
              Slot(&Object::HasLayer, pthis));

    // 	    s.AddString(XML_CURSOR, this,
    // 		    memfun_cast<pfnStringSetter>(&Object::SetCursorId),
    // 		    memfun_cast<pfnStringGetter>(&Object::SaveCursorId));

    // 设置背景渲染器
    s.AddRenderBase(XML_BACKGND_RENDER_PREFIX, m_back_render);
    // 设置前景绘制
    s.AddRenderBase(XML_FOREGND_RENDER_PREFIX, m_fore_render);

    //         s.AddStringEnum(XML_BACKGND_RENDER_PREFIX XML_RENDER_TYPE, this,
    //             memfun_cast<pfnStringSetter>(&Object::LoadBkgndRender),
    //             memfun_cast<pfnStringGetter>(&Object::SaveBkgndRender))
    //             ->FillRenderBaseTypeData()
    //             ->ReloadOnChanged();
    //         s.AddStringEnum(XML_FOREGND_RENDER_PREFIX XML_RENDER_TYPE, this,
    //             memfun_cast<pfnStringSetter>(&Object::LoadForegndRender),
    //             memfun_cast<pfnStringGetter>(&Object::SaveForegndRender))
    //             ->FillRenderBaseTypeData()
    //             ->ReloadOnChanged();

    // 背景属性
    // 	   if (m_back_render)
    // 	   {
    // 		   SerializeParam data(*pData);
    // 		   data.szParentKey = XML_BACKGND_RENDER_PREFIX XML_RENDER_TYPE;
    // 		   data.szPrefix = XML_BACKGND_RENDER_PREFIX;
    //
    // 		   // 在editor中，动态修改render type不要清除属性。
    // 		   // 1.
    // 属性可能共用一个key，如render.image=，即使换了type，属性也可以共享
    // 		   // 2. 要实现undo/redo，不能丢掉属性
    // 		   if (data.pUIApplication->IsDesignMode())
    // 			   data.SetErase(false);
    //
    // 		   m_back_render->Serialize(&data);
    // 	   }
    //
    // 	   // 前景属性
    // 	   if (m_fore_render)
    // 	   {
    // 		   SerializeParam data(*pData);
    // 		   data.szParentKey = XML_FOREGND_RENDER_PREFIX XML_RENDER_TYPE;
    // 		   data.szPrefix = XML_FOREGND_RENDER_PREFIX;
    // 		   if (data.pUIApplication->IsDesignMode())
    // 			   data.SetErase(false);
    //
    // 		   m_fore_render->Serialize(&data);
    // 	   }
  }

  // 属性逻辑重构：
#if 1
  if (pData->IsReload()) {
    // TODO: 
    // m_property_store->ClearAndPreserveData();
  }

  m_property_store.RegisterString(OBJECT_ID, XML_ID).AsData();
  
  m_property_store.RegisterRect(OBJECT_PADDING, XML_PADDING);
  m_property_store.RegisterRect(OBJECT_MARGIN, XML_MARGIN);
  m_property_store.RegisterRect(OBJECT_BORDER, XML_BORDER);

  // 扩展的非客户区，与border、padding共同做为对象的非客户区。
  // 对于窗口类型，这个值会动态计算，带上边框、标题栏范围。
  // 不需要在xml中配置，由代码自行配置。
  m_property_store.RegisterRect(OBJECT_EXT_NONCLIENT, "");

  // 将xml attribute全灌输给property store
  m_property_store.Serialize(pData->attribute_map);
#endif
}


// 当手动创建一个对象（非从xml中加载时，可以调用该函数完全默认属性的加载)
void ObjectProp::InitDefaultAttrib() {
  Object& self = *static_cast<Object*>(this);

  std::shared_ptr<IAttributeMap> attribute_map;
  if (m_attribute_map_remaining) {
    attribute_map = m_attribute_map_remaining;
  } else {
    attribute_map = UICreateIMapAttribute();
  }

  // UIASSERT(m_strId.empty() && "将setid放在该函数之后调用，避免覆盖");
  // attribute_map->AddAttr(XML_ID, m_strId.c_str()); // 防止id被覆盖??

  // 解析样式
  UIASSERT(self.m_resource);

  StyleRes &styleRes = self.m_resource->GetStyleRes();

  const char *szStyle = attribute_map->GetAttr(nullptr, XML_STYLE, true);
  std::string strStyle;
  if (szStyle)
    strStyle = szStyle;

  styleRes.LoadStyle(self.m_meta->Name(), strStyle.c_str(), nullptr,
                     attribute_map.get());

  SerializeParam data = {0};
  data.resource = self.m_resource->GetIResource();
  data.attribute_map = attribute_map.get();
  data.nFlags = SERIALIZEFLAG_LOAD;

  m_attribute_map_remaining = attribute_map;
  attribute_map = nullptr;

  SerializeMessage msg;
  msg.param = &data;
  self.RouteMessage(&msg);

  // 如果没有多余的属性，直接释放，节省内存
  if (m_attribute_map_remaining && 0 == m_attribute_map_remaining->GetAttrCount()) {
    m_attribute_map_remaining.reset();
  }
}

PropertyStore* ObjectProp::GetInheritPropertyStore() {
  Object& self = *static_cast<Object*>(this);

  if (!self.m_pParent) {
    return nullptr;
  }
  return &self.m_pParent->m_property_store;
}

// 注：如果在其它模块直接调用 pCtrl->m_strID=L"..."的话，在对象释放时将会崩溃
void ObjectProp::SetId(const char *text) {
  m_property_store.SetString(OBJECT_ID, text);
}
const char *ObjectProp::GetId() { 
  return m_property_store.GetString(OBJECT_ID).c_str();
}


void ObjectProp::LoadAttributes(bool bReload) {
  Object& self = *static_cast<Object*>(this);


  if (!m_attribute_map_remaining)
    return;

  std::string strStyle;
  std::string strId;

  const char *szText = m_attribute_map_remaining->GetAttr(XML_STYLECLASS, false);
  if (szText)
    strStyle = szText;

  szText = m_attribute_map_remaining->GetAttr(XML_ID, false);
  if (szText)
    strId = szText;

  StyleRes &styleRes = self.m_resource->GetStyleRes();
  styleRes.LoadStyle(self.m_meta->Name(), strStyle.c_str(),
                     strId.c_str(), m_attribute_map_remaining.get());

  SerializeParam data = {0};
  data.resource = self.m_resource->GetIResource();
  data.attribute_map = m_attribute_map_remaining.get();
  data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
  if (bReload)
    data.nFlags |= SERIALIZEFLAG_RELOAD;

  SerializeMessage msg;
  msg.param = &data;
  self.RouteMessage(&msg);

  // 如果没有多余的属性，直接释放，节省内存
  if (m_attribute_map_remaining && 0 == m_attribute_map_remaining->GetAttrCount()) {
    m_attribute_map_remaining.reset();
  }
}

// 从xml获取自己的属性
void ObjectProp::LoadAttributeFromXml(UIElement *pElement, bool bReload) {
  if (!pElement)
    return;

  m_attribute_map_remaining.reset();
  m_attribute_map_remaining = UICreateIMapAttribute();
  pElement->GetAttribList(m_attribute_map_remaining.get());
  this->LoadAttributes(bReload); 

#if 0
  // 通知编辑器关联控件和xml结点.
  // 将通知放在这里，而不是layoutmanager中，是为了解决复合控件中的子控件加载问题
  // 这种子控件不会在layoutmanager中被加载属性，而是由复合控件自己解决，最终调用
  // 到子控件的SetAttributeFromXmlElement函数，因此将通知放在这里。
  IUIEditor *pEditor = GetUIApplication()->GetUIEditorPtr();
  if (pEditor) {
    pEditor->OnObjectAttributeLoad(m_pIObject, pElement->GetIUIElement());
  }
#endif
}

// 获取一个未解析的属性。如果bErase==true，则将返回一个临时的字符串指针，调用者应该尽快保存或者仅临时使用
const char *ObjectProp::GetAttribute(const char *szKey, bool bErase) {
  if (nullptr == szKey || nullptr == m_attribute_map_remaining)
    return nullptr;

  return m_attribute_map_remaining->GetAttr(szKey, bErase);
}
void ObjectProp::AddAttribute(const char *szKey, const char *szValue) {
  if (nullptr == m_attribute_map_remaining) {
    m_attribute_map_remaining = UICreateIMapAttribute();
  }
  m_attribute_map_remaining->AddAttr(szKey, szValue);
}
std::shared_ptr<IAttributeMap> ObjectProp::GetMapAttribute() {
  return m_attribute_map_remaining;
}
void ObjectProp::ClearMapAttribute() {
  m_attribute_map_remaining.reset();
}


void ObjectProp::SetBackRender(std::shared_ptr<IRenderBase> p) {
  m_back_render = p;
}

void ObjectProp::SetForegndRender(std::shared_ptr<IRenderBase> p) {
  m_fore_render = p;
}

void ObjectProp::SetTextRender(std::shared_ptr<ITextRenderBase> p) {
  m_text_render = p;
}

std::shared_ptr<ITextRenderBase> ObjectProp::GetTextRender() { return m_text_render; }
std::shared_ptr<IRenderBase> ObjectProp::GetBackRender() { return m_back_render; }
std::shared_ptr<IRenderBase> ObjectProp::GetForeRender() { return m_fore_render; }

//	获取自己的字体,这里返回的对象只是一个临时对象，
//	如果需要保存使用，则需要调用AddRef
IRenderFont *ObjectProp::GetRenderFont() {
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


void ObjectProp::LoadBkgndRender(const char *szName) {
  load_renderbase(szName, m_back_render);
}
void ObjectProp::LoadForegndRender(const char *szName) {
  load_renderbase(szName, m_fore_render);
}
void ObjectProp::LoadTextRender(const char *szName) {
  load_textrender(szName, m_text_render);
}
const char *ObjectProp::SaveBkgndRender() {
  return get_renderbase_name(m_back_render);
}
const char *ObjectProp::SaveForegndRender() {
  return get_renderbase_name(m_fore_render);
}
const char *ObjectProp::SaveTextRender() {
  return get_textrender_name(m_text_render);
}


const char *ObjectProp::get_renderbase_name(std::shared_ptr<IRenderBase> &pRender) {
  if (!pRender)
    return nullptr;
#if 0 // defined(OS_WIN)
  return GetUIApplication()->GetRenderBaseFactory().GetRenderBaseName(
      pRender->GetType());
#else
  return nullptr;
#endif
}
const char *ObjectProp::get_textrender_name(std::shared_ptr<ITextRenderBase> &pTextRender) {
  if (!pTextRender)
    return nullptr;
#if 0 // defined(OS_WIN)
  return GetUIApplication()->GetTextRenderFactroy().GetTextRenderBaseName(
      pTextRender->GetType());
#else
  return nullptr;
#endif
}


void ObjectProp::load_renderbase(const char *szName, std::shared_ptr<IRenderBase> &pRender) {
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

void ObjectProp::load_textrender(const char *szName,
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


} // namespace ui