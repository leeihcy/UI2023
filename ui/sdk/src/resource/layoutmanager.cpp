#include "layoutmanager.h"
#include "src/private_inc.h"

#include "include/interface/iuiapplication.h"
#include "include/interface/iuires.h"
#include "include/interface/ixmlwrap.h"
#include "src/application/uiapplication.h"
#include "src/object/object.h"
#include "src/window/window.h"
#include "src/resource/res_bundle.h"
#include "src/resource/stylemanager.h"
#include "src/resource/uiresource.h"
#include "src/parser/skinparseengine.h"
#include "src/parser/xml/xmlwrap.h"
#include "include/interface/ilayout.h"

namespace ui {

LayoutManager::LayoutManager(ResourceBundle *p) {
  m_resource_bundle = p;
  m_pILayoutManager = nullptr;
}

LayoutManager::~LayoutManager(void) {
  m_listUIElement.RELEASE_ALL();
  m_listLayoutConfig.DELETE_ALL();
  SAFE_DELETE(m_pILayoutManager);
}

ILayoutManager &LayoutManager::GetILayoutManager() {
  if (nullptr == m_pILayoutManager)
    m_pILayoutManager = new ILayoutManager(this);

  return *m_pILayoutManager;
}

// 用于实现界面插件化，将一部分控件放在另外一个窗口配置文件中。
// LoadControlLayout将找到指定的窗口，将窗口下面的所有控件加载到指定的父控件下面
Object *LayoutManager::LoadPluginLayout(const char *szWndId,
                                        Object *pNewParent,
                                        IMessage *pNotifyTarget) {
  if (!pNewParent)
    return nullptr;

  UIElementProxy objUIElement = this->FindWindowElement(XML_WINDOW_, szWndId);
  if (!objUIElement) {
    UI_LOG_FATAL("未找到要加载的对象：name=%s, id=%s", XML_WINDOW_,
                 szWndId);
    return nullptr;
  }

  Object *pFirstChild =
      this->ParseChildElement(objUIElement.get(), pNewParent, pNotifyTarget);
  if (!pFirstChild)
    return nullptr;

  GetLayoutMessage msg;
  pNewParent->RouteMessage(&msg);
  ILayout *pLayout = msg.layout;
  if (pLayout) {
    pLayout->SetDirty(true);

    float scale = 1.0f;
    auto* window = pNewParent->GetWindow();
    if (window) {
      scale = window->m_dpi.GetScaleFactor();
    }
    ArrangeParam param = {
      pFirstChild->GetIObject(), 
      ArrangeReason::VisibleChanged,
    };
    pLayout->Arrange(param);
  }

  // 发送初始化通知( TBD:
  // 这里只实现了给第一个对象发送通知，因此插件也只能有一个根结点）
  Object *p = pFirstChild;
  {
    p->RouteMessage(UI_MSG_INITIALIZE);
    p->ForwardInitializeMessageToDecendant();
    p->RouteMessage(UI_MSG_INITIALIZE2);
  }

  pNewParent->Invalidate();
  return pFirstChild;
}

// 外部只知道一个窗口id，相拿到该窗口的tagname好调用uiapplication->createinstancebyname
// bool  LayoutManager::GetWindowNameById(const char* szId, BSTR* bstrName)
// {
//     UIElement* pElement = nullptr;
//     if (!FindWindowElement(nullptr, szId, &pElement))
//         return false;
//
//     pElement->GetTagName(bstrName);
//     return true;
// }

// 1. 从缓存m_listUIElement中查找结点，直接配置在skin.xml中
// 2. 用szId.xml在皮肤根目录中查找，例如加载maindlg，会匹配maindlg.xml
// 3. 在layoutconfig中的配置加载，例如<layoutconfig> <item id="" path=""/>
// </layoutconfig>
UIElementProxy LayoutManager::FindWindowElement(const char *szTagName,
                                                const char *szId) {
  UIElementProxy proxy;

  if (!m_resource_bundle || !szId)
    return proxy;

  // 1. 从缓存m_listUIElement中查找结点
  proxy = find_element_from_cache(szTagName, szId);
  if (proxy)
    return proxy;

    // UIEditor不好支持这种形式，还得去遍历目录。先不支持了
#if 0
	// 2. 用szId.xml在皮肤根目录中查找
	if (loadWindowById(szTagName, szId, ppElem))
		return true;
#endif

  // 3. 从配置中加载指定窗口id
  proxy = load_element_from_layout_config(szTagName, szId);
  if (proxy)
    return proxy;

  ResourceBundle *pParentRes = m_resource_bundle->GetParentSkinRes();
  if (pParentRes) {
    return pParentRes->GetLayoutManager().FindWindowElement(szTagName, szId);
  }
  return proxy;
}

UIElementProxy LayoutManager::FindListItemElement(const char *szId) {
  UIElementProxy proxy;

  if (!m_resource_bundle || !szId)
    return proxy;

  // 1. 从缓存m_listUIElement中查找结点
  proxy = find_element_from_cache(XML_LISTITEM_, szId);
  if (proxy)
    return proxy;

  // 2. 从配置中加载指定窗口id
  proxy = load_element_from_layout_config(XML_LISTITEM_, szId);
  if (proxy)
    return proxy;

  return proxy;
}

UIElementProxy LayoutManager::load_window_by_id(const char *szTagName,
                                                const char *szId) {
  UIElementProxy proxy;
  BundleSource *pDataSource = m_resource_bundle->GetSource();
  if (!pDataSource)
    return proxy;

  std::string strFile(szId);
  strFile.append("." XML_XML);

  SkinParseEngine engine(m_resource_bundle);
  if (engine.Parse(pDataSource, strFile.c_str())) {
    // TODO: parse只是将文件添加到列表中，还得重新从列表中拿到xml结点
    // 有待优化
    proxy = find_element_from_cache(szTagName, szId);
    if (proxy)
      return proxy;
  }

  return proxy;
}

// 在<layout>下面查找指定的tag + id，返回UIElement
UIElementProxy LayoutManager::find_element_from_cache(const char *szTagName,
                                                      const char *szId) {
  // 允许szTagName为空，只匹配szId
  if (!szId)
    return UIElementProxy();

  UIListItem<UIElement *> *pUIElementItem = m_listUIElement.GetFirst();
  while (pUIElementItem) {
    UIElementProxy childElem = (**pUIElementItem)->FirstChild();
    while (childElem) {
      if (testUIElement(childElem.get(), szTagName, szId)) {
        return childElem;
      }

      childElem = childElem->NextElement();
    }
    pUIElementItem = pUIElementItem->GetNext();
  }

  return UIElementProxy();
}

UIElementProxy
LayoutManager::load_element_from_layout_config(const char *szTagName,
                                               const char *szId) {
  if (!szId)
    return UIElementProxy();

  BundleSource *pDataSource = m_resource_bundle->GetSource();
  if (!pDataSource)
    return UIElementProxy();

  UIListItem<LayoutConfigItem *> *pListItem = m_listLayoutConfig.GetFirst();
  while (pListItem) {
    LayoutConfigItem *pConfigItem = **pListItem;
    if (!pConfigItem->Find(szTagName, szId)) {
      pListItem = pListItem->GetNext();
      continue;
    }

    SkinParseEngine engine(m_resource_bundle);
    if (engine.Parse(pDataSource, pConfigItem->GetPath())) {
      // TODO: parse只是将文件添加到列表中，还得重新从列
      // 表中拿到xml结点。有待优化
      UIElementProxy proxy = find_element_from_cache(szTagName, szId);
      if (proxy) {
        m_listLayoutConfig.Remove(pConfigItem);
        SAFE_DELETE(pConfigItem);
        return proxy;
      }
    }

    pListItem = pListItem->GetNext();
  }

  return UIElementProxy();
}

// 测试一个结点的tag和id
// 可以不指定szTagName
bool LayoutManager::testUIElement(UIElement *pElem, const char *szTagName,
                                  const char *szId) {
  if (!pElem || !szId)
    return false;

  const char *szTagName2 = pElem->GetTagName();
  if (szTagName && szTagName2 && 0 != strcmp(szTagName2, szTagName))
    return false;

  std::string bstrId = pElem->GetAttrib(XML_ID);
  if (bstrId != szId)
    return false;

  return true;
}

// 递归，加载所有子对象及子对象的属性
// 返回第一个子结点
Object *LayoutManager::ParseChildElement(UIElement *pParentElement,
                                         Object *pParent,
                                         IMessage *pNotifyTarget, int flags) {
  if (!pParent || !pParentElement)
    return nullptr;

  Object *pFirstChild = nullptr;

  UIElementProxy child = pParentElement->FirstChild();
  while (child) {
    Object *pRet = ParseElement(child.get(), pParent, pNotifyTarget, flags);
    if (!pFirstChild)
      pFirstChild = pRet;

    child = child->NextElement();
  }

  return pFirstChild;
}

bool IsTrue(const char *szValue) {
  if (0 == strcmp(szValue, XML_BOOL_VALUE_TRUE) ||
      0 == strcmp(szValue, XML_BOOL_VALUE_1) ||
      0 == strcmp(szValue, XML_BOOL_VALUE_YES)) {
    return true;
  }
  return false;
}

// 得到一个元素指针，创建该元素及其子结点，并返回对象指针
Object *LayoutManager::ParseElement(UIElement *pUIElement, Object *pParent,
                                    IMessage *pNotifyTarget, int flags) {
  Application *pUIApp = m_resource_bundle->GetUIApplication();

  Object *pObj = nullptr;
  std::string bstrTagName = pUIElement->GetTagName();

  // 将标签名转化为类
  PARSE_CONTROL_RETURN eParseRet = ParseControl_CreateObject;

  // 特殊字段，跳过。
  if (0 == strcmp(XML_PROP, bstrTagName.c_str()))
    return nullptr;

  IObject *pIObject = pUIApp->CreateUIObjectByName(bstrTagName.c_str(),
                                                   m_resource_bundle->GetIResource());
  if (nullptr == pIObject) {
    // 尝试寻找该Tag是否被注册了
    pfnParseControlNode func = nullptr;
    pUIApp->GetControlTagParseFunc(bstrTagName.c_str(), &func);
    if (func) {
      eParseRet = func(pUIElement->GetIUIElement(), m_resource_bundle->GetIResource(),
                       pParent ? pParent->GetIObject() : nullptr, &pIObject);

      if (eParseRet == ParseControl_Failed) {
        UI_LOG_ERROR(L"Parse Object:  %s Failed.", bstrTagName.c_str());
        return nullptr;
      }
    } else {
      UI_LOG_ERROR("CreateObject Failed. name=%s", bstrTagName.c_str());
      return nullptr;
    }
  }

  if (nullptr == pIObject)
    return nullptr;

  pObj = pIObject->GetImpl();

  bool bReload = flags & PARSEFLAG_RELOAD ? true : false;

  if (eParseRet < ParseControl_LoadObject) {
    // 先单方面设置下父对象，用于font/bitmap创建时，获取到window的graphics type
    pObj->SetParentObjectDirect(pParent);
    {
      // 自己的属性
      pObj->LoadAttributeFromXml(pUIElement, bReload);
    }
    pObj->SetParentObjectDirect(nullptr);

    // 先加载属性，获取到zorder、ncchild属性之后，再addchild
    if (pParent) {
      OBJSTYLE s = {0};
      s.ncobject = 1;
      if (pObj->TestObjectStyle(s)) {
        pParent->AddNcChild(pObj);
      } else {
        pParent->AddChild(pObj);
      }
    }
  }

  if (eParseRet < ParseControl_LoadDescendants) {
    if (flags & PARSEFLAG_LOADCHILD) {
      // 遍历子对象
      ParseChildElement(pUIElement, pIObject->GetImpl(), pNotifyTarget, flags);
    }
  }

#if 0 // 废弃，使用RouteMessage代替。 
  // 默认将该控件添加一个notify object为窗口对象，并将msgmapid设置为0
  if (pNotifyTarget == NOTIFY_TARGET_ROOT) {
    Window *pWindowBase = pObj->GetWindow();
    if (pObj != pWindowBase && pWindowBase) {
      pObj->SetNotify(pWindowBase->GetIMessage(), 0);
    }
  } else if (pNotifyTarget == NOTIFY_TARGET_NULL) {

  } else {
    pObj->SetNotify(pNotifyTarget, 0);
  }
#endif

  return pObj;
}

//
//	更新指定的对象的布局数据，在xml中将根据pRootObj的object
//name和id来查找原有对象的新属性和布局
//
// TODO: 新增的控件，没有机会收到INITIALIZE消息？
bool LayoutManager::ReLoadLayout(Object *pRootObj, const char *szNewLayoutId,
                                 std::map<std::string, Object *> &mapNamedChildren) {
  const char *szObjName = "";
  if (pRootObj->GetMeta())
    szObjName = pRootObj->GetMeta()->Name();

  UIElementProxy objUIElem = this->FindWindowElement(szObjName, szNewLayoutId);

  if (!objUIElem) {
    UI_LOG_FATAL("未找到要加载的对象：name=%s, id=%s", szObjName,
                 szNewLayoutId);
    return false;
  }

  pRootObj->LoadAttributeFromXml(objUIElem.get(), true);
  ReloadChildObjects(pRootObj, objUIElem.get(), mapNamedChildren);
  return true;
}

void LayoutManager::ReloadChildObjects(
    Object *pObjParent, UIElement *pObjElement,
    std::map<std::string, Object *> &mapNamedChildren) {
  Application *pUIApp = m_resource_bundle->GetUIApplication();
  UIElementProxy childElem = pObjElement->FirstChild();

  // 遍历所有子对象
  while (childElem) {
    Object *pObj = nullptr;

    const char *szTagName = childElem->GetTagName();
    if (!szTagName || !szTagName[0]) {
      UI_LOG_WARN("xml invalid tag name.");

      childElem = childElem->NextElement();
      continue;
    }

    // 特殊字段，跳过。
    if (0 == strcmp(XML_PROP, szTagName)) {
      childElem = childElem->NextElement();
      continue;
    }

    std::string bstrId = childElem->GetAttrib(XML_ID);
    if (!bstrId.empty()) {
      // 根据 tagName + id 从listAllChild中查找该对象
      auto iter = mapNamedChildren.find(bstrId);
      if (iter != mapNamedChildren.end()) {
        Object *pTempObj = iter->second;

        const char *szObjName = "";
        if (pTempObj->GetMeta())
          szObjName = pTempObj->GetMeta()->Name();

        if (szObjName && 0 == strcmp(szObjName, szTagName)) {
          pObj = pTempObj;
          mapNamedChildren.erase(iter);
        }
      }
    }

    if (nullptr == pObj) {
      pObj = ParseElement(childElem.get(), pObjParent, NOTIFY_TARGET_ROOT, 0);

      // 递归，加载这个子对象的子对象
      this->ReloadChildObjects(pObj, childElem.get(), mapNamedChildren);
    } else {
      // 关联
      if (pObj->IsNcObject())
        pObjParent->AddNcChild(pObj);
      else
        pObjParent->AddChild(pObj);

      // 加载属性
      pObj->LoadAttributeFromXml(childElem.get(), true);

      // 递归，加载这个子对象的子对象
      this->ReloadChildObjects(pObj, childElem.get(), mapNamedChildren);
    }

    childElem = childElem->NextElement();
  }
}

//////////////////////////////////////////////////////////////////////////

int LayoutManager::UIParseLayoutTagCallback(IUIElement *pElem,
                                             IResourceBundle *resource_bundle) {
  ILayoutManager &pLayoutMgr = resource_bundle->GetLayoutManager();
  pLayoutMgr.GetImpl()->ParseNewElement(pElem->GetImpl());
  return true;
}
int LayoutManager::UIParseLayoutConfigTagCallback(IUIElement *pElem,
                                                   IResourceBundle *resource_bundle) {
  ILayoutManager &pLayoutMgr = resource_bundle->GetLayoutManager();
  pLayoutMgr.GetImpl()->ParseLayoutConfigTag(pElem->GetImpl());
  return 0;
}

void LayoutManager::ParseNewElement(UIElement *pElem) {
  pElem->AddRef();
  m_listUIElement.Add(pElem);

  // 由窗口自己调用LoadLayout再解析
}

//
// 解析
//
//  <layoutconfig>
//      <file path="login_dlg.xml">
//        <Window name="login.dlg"/>
//        <Window name="login.userlist"/>
//        <ListItem name="login.userlist.item"/>
//      </file>
//  </layoutconfig>
//
void LayoutManager::ParseLayoutConfigTag(UIElement *pElem) {
  if (!pElem)
    return;

  // 遍历其子元素

  std::string bstrTagName;

  UIElementProxy childElement = pElem->FirstChild();
  while (childElement) {
    bstrTagName = childElement->GetTagName();
    if (bstrTagName == XML_FILE) {
      ParseLayoutConfigFileTag(childElement.get());
    }

    childElement = childElement->NextElement();
  }
}

bool LayoutManager::ParseLayoutConfigFileTag(UIElement *pElem) {
  if (!pElem)
    return false;

  std::string bstrPath = pElem->GetAttrib(XML_PATH);
  if (bstrPath.empty())
    return false;

  LayoutConfigItem *pFileitem = new LayoutConfigItem;
  pFileitem->SetPath(bstrPath.c_str());

  UIElementProxy childElement = pElem->FirstChild();
  while (childElement) {
    const char *szTagName = childElement->GetTagName();

    if (0 == strcmp(szTagName, XML_WINDOW)) {
      std::string bstrWindowName = childElement->GetAttrib(XML_NAME);
      if (!bstrWindowName.empty()) {
        pFileitem->AddWindow(bstrWindowName.c_str());
      }
    } else if (0 == strcmp(szTagName, XML_LISTITEM)) {
      std::string bstrWindowName = childElement->GetAttrib(XML_NAME);
      if (!bstrWindowName.empty()) {
        pFileitem->AddListItem(szTagName);
      }
    }

    childElement = childElement->NextElement();
  }

  m_listLayoutConfig.Add(pFileitem);
  return true;
}

//////////////////////////////////////////////////////////////////////////

void LayoutConfigItem::AddWindow(const char *szName) {
  if (szName) {
    m_vecWindow.push_back(std::string(szName));
  }
}
bool LayoutConfigItem::FindWindow(const char *szName) {
  if (!szName)
    return false;

  std::vector<std::string>::iterator iter =
      std::find(m_vecWindow.begin(), m_vecWindow.end(), std::string(szName));

  if (iter == m_vecWindow.end())
    return false;

  return true;
}

void LayoutConfigItem::AddListItem(const char *szName) {
  if (szName) {
    m_vecListItem.push_back(std::string(szName));
  }
}
bool LayoutConfigItem::FindListItem(const char *szName) {
  if (!szName)
    return false;

  std::vector<std::string>::iterator iter =
      std::find(m_vecListItem.begin(), m_vecListItem.end(), std::string(szName));

  if (iter == m_vecListItem.end())
    return false;

  return true;
}

// 目前只有这两种类型
bool LayoutConfigItem::Find(const char *szTagName, const char *szName) {
  if (!szTagName || !szName)
    return false;

  if (0 == strcmp(szTagName, XML_LISTITEM_))
    return FindListItem(szName);
  else
    return FindWindow(szName);
}

unsigned int LayoutConfigItem::GetWindowCount() {
  return (unsigned int)m_vecWindow.size();
}
const char *LayoutConfigItem::GetWindowName(unsigned int index) {
  if (index <= m_vecWindow.size())
    return m_vecWindow[index].c_str();

  return nullptr;
}

void LayoutConfigItem::SetPath(const char *szPath) {
  SETSTRING(m_strPath, szPath);
}
const char *LayoutConfigItem::GetPath() { return m_strPath.c_str(); }

//////////////////////////////////////////////////////////////////////////

class LayoutWindowNodeList : public ILayoutWindowNodeList {
public:
  static LayoutWindowNodeList *Create() { return new LayoutWindowNodeList; }
  virtual void Release() override { delete this; }
  virtual unsigned int GetCount() override { return (int)m_array.size(); }
  virtual const char *GetWindowName(unsigned int index) override {
    if (index >= m_array.size())
      return nullptr;

    return m_array[index].strName.c_str();
  }
  virtual const char *GetWindowId(unsigned int index) override {
    if (index >= m_array.size())
      return nullptr;

    return m_array[index].strId.c_str();
  }
  virtual const char *GetWindowPath(unsigned int index) override {
    if (index >= m_array.size())
      return nullptr;

    return m_array[index].strPath.c_str();
  }

public:
  void AddWindow(const char *szPath, const char *szName,
                 const char *szId) {
    if (szId && szPath) {
      WindowInfo info;
      SETSTRING(info.strName, szName);
      SETSTRING(info.strId, szId);
      SETSTRING(info.strPath, szPath);
      m_array.push_back(info);
    }
  }

private:
  struct WindowInfo {
    std::string strPath;
    std::string strName;
    std::string strId;
  };
  std::vector<WindowInfo> m_array;
};

bool LayoutManager::LoadWindowNodeList(ILayoutWindowNodeList **ppInfo) {
  if (!ppInfo)
    return false;

  LayoutWindowNodeList *pResult = LayoutWindowNodeList::Create();
  *ppInfo = pResult;

  // 从缓存中读取
  UIListItem<UIElement *> *pUIElementItem = m_listUIElement.GetFirst();
  while (pUIElementItem) {
    UIElement *pLayoutElem = **pUIElementItem;

    // 获取该结点所在的xml名称
    const char *szDocName = nullptr;
    UIDocument *pDoc = pLayoutElem->GetDocument();
    if (pDoc) {
      szDocName = pDoc->GetSkinPath();
    }

    //  遍历访<layout>下面的窗口结点
    std::string bstrTagName;
    std::string bstrId;

    UIElementProxy childElem = pLayoutElem->FirstChild();
    while (childElem) {
      bstrId = childElem->GetAttrib(XML_ID);
      pResult->AddWindow(szDocName, childElem->GetTagName(), bstrId.c_str());

      childElem = childElem->NextElement();
    }
    pUIElementItem = pUIElementItem->GetNext();
  }

  // 2. 从layoutconfig中读取
  UIListItem<LayoutConfigItem *> *pListItem = m_listLayoutConfig.GetFirst();
  while (pListItem) {
    LayoutConfigItem *pConfigItem = **pListItem;

    unsigned int nWindowCount = pConfigItem->GetWindowCount();
    for (unsigned int i = 0; i < nWindowCount; i++) {
      pResult->AddWindow(pConfigItem->GetPath(), nullptr,
                         pConfigItem->GetWindowName(i));
    }
    pListItem = pListItem->GetNext();
  }
  return true;
}

} // namespace ui