#include "pugixmlwrap.h"
#include "include/inc.h"
#include "include/interface/imapattr.h"
#include "include/util/util.h"

namespace ui {
PugiXmlElement::PugiXmlElement(pugi::xml_node &node, UIDocument *pDoc)
    : m_IUIElement(this) {
  m_lRef = 0;
  m_node = node;
  m_pDocument = pDoc;
}
PugiXmlElement::~PugiXmlElement() {}

IUIElement *PugiXmlElement::GetIUIElement() { return &m_IUIElement; }

long PugiXmlElement::AddRef() { return ++m_lRef; }
long PugiXmlElement::Release() {
  m_lRef--;

  if (0 == m_lRef) {
    delete this;
    return 0;
  }

  return m_lRef;
}

const char *PugiXmlElement::GetTagName() { return m_node.name(); }

void PugiXmlElement::SetTagName(const char *szTag) {
  m_node.set_name(szTag);
}
UIDocument *PugiXmlElement::GetDocument() { return m_pDocument; }

const char *PugiXmlElement::GetData() { return m_node.text().as_string(); }

bool PugiXmlElement::SetData(const char *szText) {
  m_node.text() = szText;
  return true;
}

UIElementProxy PugiXmlElement::AddChild(const char *szNodeName) {
  if (!szNodeName)
    return UIElementProxy();

  pugi::xml_node node = m_node.append_child(szNodeName);
  if (node.empty())
    return UIElementProxy();

  return UIElementProxy(new PugiXmlElement(node, m_pDocument));
}

bool PugiXmlElement::AddChild(UIElement *p) {
  if (!p)
    return false;

  PugiXmlElement *pElement = static_cast<PugiXmlElement *>(p);
  return m_node.append_copy(pElement->m_node).empty() ? false : true;
}

bool PugiXmlElement::RemoveChild(UIElement *p) {
  if (!p)
    return false;

  PugiXmlElement *pElem = (PugiXmlElement *)p;
  return m_node.remove_child(pElem->m_node);
}

bool PugiXmlElement::RemoveSelfInParent() {
  if (m_node.parent().empty())
    return false;

  return m_node.parent().remove_child(m_node);
}

// 如果为空，则表示插在最后面
bool PugiXmlElement::AddChildBefore(UIElement *pElem,
                                    UIElement *pInsertBefore) {
  if (!pElem)
    return false;

  if (!pInsertBefore)
    return AddChild(pElem);

  PugiXmlElement *pElement = static_cast<PugiXmlElement *>(pElem);
  PugiXmlElement *pElementBefore = static_cast<PugiXmlElement *>(pInsertBefore);

  return m_node.insert_copy_before(pElement->m_node, pElementBefore->m_node)
                 .empty()
             ? false
             : true;
}

bool PugiXmlElement::AddChildAfter(UIElement *pElem, UIElement *pInsertAfter) {
  if (!pElem)
    return false;

  PugiXmlElement *pElement = static_cast<PugiXmlElement *>(pElem);

  // 插在最前面
  if (!pInsertAfter) {
    return m_node.prepend_copy(pElement->m_node).empty() ? false : true;
  }

  // 查看after的下一个，插在其前面
  PugiXmlElement *pElementInsertAfter =
      static_cast<PugiXmlElement *>(pInsertAfter);

  return m_node.insert_copy_after(pElement->m_node, pElementInsertAfter->m_node)
                 .empty()
             ? false
             : true;
}

// bool  PugiXmlElement::MoveChildAfterChild(UIElement* pChild2Move, UIElement*
// pChildInsertAfter)
// {
// 	if (!pChild2Move)
// 		return false;
//
// 	PugiXmlElement* _pChild2Move =
// static_cast<PugiXmlElement*>(pChild2Move);
//
// 	if (pChildInsertAfter)
// 	{
// 		PugiXmlElement* _pChildInsertAfter =
// static_cast<PugiXmlElement*>(pChildInsertAfter); 		pugi::xml_node new_node =
// m_node.insert_copy_after(_pChild2Move->m_node, _pChildInsertAfter->m_node);
// 		if (new_node.empty())
// 			return false;
//
// 		m_node.remove_child(_pChild2Move->m_node);
// 		_pChild2Move->m_node = new_node;
// 	}
// 	else
// 	{
// 		pugi::xml_node new_node =
// m_node.prepend_copy(_pChild2Move->m_node); 		if (new_node.empty()) 			return
// false;
//
// 		m_node.remove_child(_pChild2Move->m_node);
// 		_pChild2Move->m_node = new_node;
// 	}
//
// 	return true;
// }

bool PugiXmlElement::MoveTo(UIElement *_pNewParent,
                            UIElement *_pChildInsertAfter) {
  if (!m_node)
    return false;

  PugiXmlElement *pNewParent = static_cast<PugiXmlElement *>(_pNewParent);
  PugiXmlElement *pInsertAfter =
      static_cast<PugiXmlElement *>(_pChildInsertAfter);

  if (!pNewParent) {
    UIASSERT(0); // TODO:
    return false;
  }
  pugi::xml_node new_node;

  if (pInsertAfter && pInsertAfter->m_node) {
    // 位置没有改变，直接返回
    if (m_node.previous_sibling() == pInsertAfter->m_node) {
      UIASSERT(0);
      return false;
    }

    new_node =
        pNewParent->m_node.insert_copy_after(m_node, pInsertAfter->m_node);
  } else {
    if (!m_node.previous_sibling()) {
      // 位置没有改变，直接返回
      UIASSERT(0);
      return false;
    }
    new_node = pNewParent->m_node.prepend_copy(m_node);
  }

  if (new_node.empty())
    return false;

  // 将自己从父结点移除
  if (m_node.parent()) {
    m_node.parent().remove_child(m_node);
  }
  m_node = new_node;

  return true;
}

// 如果为空，则表示插在最后面
UIElementProxy PugiXmlElement::AddChildBefore(const char *szNodeName,
                                              UIElement *pInsertBefore) {
  if (!szNodeName)
    return UIElementProxy();

  if (!pInsertBefore)
    return AddChild(szNodeName);

  PugiXmlElement *pElementInsertBefore =
      static_cast<PugiXmlElement *>(pInsertBefore);
  pugi::xml_node node =
      m_node.insert_child_before(szNodeName, pElementInsertBefore->m_node);
  if (node.empty())
    return UIElementProxy();

  return UIElementProxy(new PugiXmlElement(node, m_pDocument));
}

UIElementProxy PugiXmlElement::AddChildAfter(const char *szNodeName,
                                             UIElement *pInsertAfter) {
  if (!szNodeName)
    return UIElementProxy();

  pugi::xml_node node;

  // 插在最前面
  if (!pInsertAfter) {
    node = m_node.prepend_child(szNodeName);
  } else {
    PugiXmlElement *pElementInsertAfter =
        static_cast<PugiXmlElement *>(pInsertAfter);
    node = m_node.insert_child_after(szNodeName, pElementInsertAfter->m_node);
  }

  if (node.empty())
    return UIElementProxy();

  return UIElementProxy(new PugiXmlElement(node, m_pDocument));
}

UIElementProxy PugiXmlElement::FirstChild() {
  pugi::xml_node nodeChild = m_node.first_child();
  if (!nodeChild)
    return UIElementProxy();

  return UIElementProxy(new PugiXmlElement(nodeChild, m_pDocument));
}

UIElementProxy PugiXmlElement::NextElement() {
  pugi::xml_node nodeNext = m_node;
  while ((nodeNext = nodeNext.next_sibling())) {
    if (nodeNext.type() == pugi::node_comment)
      continue;

    break;
  }

  if (!nodeNext)
    return UIElementProxy();

  return UIElementProxy(new PugiXmlElement(nodeNext, m_pDocument));
}

UIElementProxy PugiXmlElement::FindChild(const char *szChildName) {
  if (!szChildName)
    return UIElementProxy();

  pugi::xml_node nodeChild = m_node.child(szChildName);
  return UIElementProxy(new PugiXmlElement(nodeChild, m_pDocument));
}

void PugiXmlElement::enum_attr(
    const std::function<void(const char *, const char *)> &callback) {

  for (pugi::xml_attribute attr = m_node.first_attribute(); !attr.empty();
       attr = attr.next_attribute()) {
    callback(attr.name(), attr.as_string());
  }

  // 加载子结点中的属性
  pugi::xml_node nodeProp = m_node.child(XML_PROP);
  if (!nodeProp)
    return;
  /*
  bkgnd.render.type = "color" bkgnd.render.color = "0xFFF" 等效于下面的形式：

  <prop key="bkgnd.render" type="color" color = "0xFFF"/>  // render
  tag名称未使用
  */

  while (nodeProp) {
    std::string strKey;

    // 如果没有配置key，或者key为空，则不加前缀
    pugi::xml_attribute attr = nodeProp.attribute(XML_KEY);
    if (!attr.empty()) {
      strKey = attr.as_string();
    }

    attr = nodeProp.first_attribute();
    for (; !attr.empty(); attr = attr.next_attribute()) {
      if (0 == strcmp(attr.name(), XML_KEY))
        continue;

      if (strKey.empty()) {
        callback(attr.name(), attr.as_string());
      } else {
        std::string strName = strKey;
        strName.append(".");
        strName.append(attr.name());
        callback(strName.c_str(), attr.as_string());
      }
    }

    nodeProp = nodeProp.next_sibling(XML_PROP);
  }
}

void PugiXmlElement::GetAttribList(IMapAttribute *ppMapAttrib) {
  //(*ppMapAttrib)->SetTag(m_node.name());

  enum_attr([ppMapAttrib](const char *k, const char *v) {
    ppMapAttrib->AddAttr(k, v);
  });
}
void PugiXmlElement::GetAttribList2(IListAttribute **ppListAttrib) {
  UICreateIListAttribute(ppListAttrib);

  //(*ppListAttrib)->SetTag(m_node.name());

  enum_attr([ppListAttrib](const char *k, const char *v) {
    (*ppListAttrib)->AddAttr(k, v);
  });
}

void PugiXmlElement::set_attr_by_prefix(IListAttribute *pListAttrib,
                                        const char *szPrefix) {
  UIASSERT(pListAttrib);
  UIASSERT(szPrefix);

  int nLength = (int)strlen(szPrefix);

  pListAttrib->BeginEnum();

  const char *szKey = nullptr;
  const char *szValue = nullptr;
  pugi::xml_node propnode;
  while (pListAttrib->EnumNext(&szKey, &szValue)) {
    if (strstr(szKey, szPrefix) == szKey) {
      if (!propnode) {
        propnode = m_node.prepend_child(XML_PROP);
        propnode.append_attribute(XML_KEY).set_value(szPrefix);
      }

      // +1 补上后面的一个 .
      propnode.append_attribute(szKey + nLength + 1).set_value(szValue);
      pListAttrib->EraseAttr(szKey);
    }
  }
  pListAttrib->EndEnum();
}

bool PugiXmlElement::SetAttribList2(IListAttribute *pListAttrib) {
  if (!pListAttrib)
    return false;

  ClearAttrib();

  const char *szPrefixArray[] = {
      XML_BACKGND_RENDER_PREFIX XML_RENDER,
      XML_FOREGND_RENDER_PREFIX XML_RENDER,
      XML_MASK_RENDER_PREFIX XML_RENDER,
      XML_TEXTRENDER,
      XML_LAYOUT,
  };

  int nCount = sizeof(szPrefixArray) / sizeof(const char *);
  for (int i = 0; i < nCount; i++) {
    set_attr_by_prefix(pListAttrib, szPrefixArray[i]);
  }

  pListAttrib->BeginEnum();
  const char *szKey = nullptr;
  const char *szValue = nullptr;
  while (pListAttrib->EnumNext(&szKey, &szValue)) {
    AddAttrib(szKey, szValue);
  }
  pListAttrib->EndEnum();

  return true;
}
#if 0 // defined(OS_WIN)
bool PugiXmlElement::GetAttrib(const char *szKey, BSTR *pbstrValue) {
  if (nullptr == szKey)
    return false;

  pugi::xml_attribute attr = m_node.attribute(szKey);
  if (attr.empty())
    return false;

  if (nullptr != pbstrValue)
    *pbstrValue = ::SysAllocString(attr.as_string());

  return true;
}
#endif

std::string PugiXmlElement::GetAttrib(const char *szKey) {
  if (nullptr == szKey)
    return std::string();

  pugi::xml_attribute attr = m_node.attribute(szKey);
  if (attr.empty())
    return std::string();

  return std::string(attr.as_string());
}

bool PugiXmlElement::HasAttrib(const char *szKey) {
  if (nullptr == szKey)
    return false;

  if (0 == strlen(szKey))
    return false;

  return (!m_node.attribute(szKey).empty());
}
bool PugiXmlElement::AddAttrib(const char *szKey, const char *szValue) {
  if (nullptr == szKey || nullptr == szValue)
    return false;

  m_node.append_attribute(szKey) = szValue;

  return true;
}

bool PugiXmlElement::SetAttrib(const char *szKey, const char *szValue) {
  if (nullptr == szKey || nullptr == szValue)
    return false;

  pugi::xml_attribute attr = m_node.attribute(szKey);
  if (attr.empty())
    return false;

  return attr.set_value(szValue);
}

bool PugiXmlElement::RemoveAttrib(const char *szKey) {
  if (nullptr == szKey)
    return false;

  return m_node.remove_attribute(szKey);
}
bool PugiXmlElement::ModifyAttrib(const char *szKey, const char *szValue) {
  return AddAttrib(szKey, szValue);
}
bool PugiXmlElement::ClearAttrib() {
  pugi::xml_attribute_iterator iter = m_node.attributes_begin();
  for (; iter != m_node.attributes_end(); ++iter) {
    m_node.remove_attribute(*iter);
  }

  // 移除所有的prop子结点
  pugi::xml_node nodeProp;
  while ((nodeProp = m_node.child(XML_PROP))) {
    m_node.remove_child(nodeProp);
  }
  return true;
}

bool PugiXmlElement::GetAttribInt(const char *szKey, int *pInt) {
  if (nullptr == szKey || nullptr == pInt)
    return false;

  std::string bstrValue = GetAttrib(szKey);
  if (bstrValue.empty())
    return false;

  *pInt = atoi(bstrValue.c_str());
  return true;
}
bool PugiXmlElement::AddAttribInt(const char *szKey, int nInt) {
  char szValue[16] = {0};
  sprintf(szValue, "%d", nInt);

  return AddAttrib(szKey, szValue);
}
//////////////////////////////////////////////////////////////////////////

PugiXmlDocument::PugiXmlDocument() : m_IUIDocument(this) {
  m_bDirty = false;
  m_lRef = 0;
}
PugiXmlDocument::~PugiXmlDocument() {}

IUIDocument *PugiXmlDocument::GetIUIDocument() { return &m_IUIDocument; }

void PugiXmlDocument::SetSkinPath(const char *szPath) {
  if (szPath)
    m_strSkinPath = szPath;
  else
    m_strSkinPath.clear();
}
const char *PugiXmlDocument::GetSkinPath() { return m_strSkinPath.c_str(); }

bool PugiXmlDocument::LoadFile(const char *szFile) {
  if (!szFile)
    return false;

  if (!util::PathFileExists(szFile))
    return false;

  m_strPath.clear();
  if (pugi::status_ok != m_doc.load_file(szFile, pugi::parse_full).status)
    return false;

  m_strPath = szFile;
  return true;
}

bool PugiXmlDocument::LoadData(const byte *pData, int nDataSize) {
  if (!pData || nDataSize <= 0)
    return false;

  m_strPath.clear();
  if (pugi::status_ok != m_doc.load_buffer((void *)pData, nDataSize).status)
    return false;

  return true;
}
UIElementProxy PugiXmlDocument::FindElem(const char *szText) {
  if (!szText)
    return UIElementProxy();

  pugi::xml_node node = m_doc.child(szText);
  if (node.empty()) {
    return UIElementProxy();
  }

  return UIElementProxy(new PugiXmlElement(node, this));
}

bool PugiXmlDocument::Save() {
  return m_doc.save_file(m_strPath.c_str(), PUGIXML_TEXT("\t"),
                         pugi::format_default | pugi::format_write_bom,
                         pugi::encoding_auto);
}

bool PugiXmlDocument::SaveAs(const char *szPath) {
  return m_doc.save_file(szPath, PUGIXML_TEXT("\t"),
                         pugi::format_default | pugi::format_write_bom,
                         pugi::encoding_auto);
}

long PugiXmlDocument::Release() {
  m_lRef--;
  if (0 == m_lRef) {
    delete this;
    return 0;
  }
  return m_lRef;
}
long PugiXmlDocument::AddRef() { return ++m_lRef; }

void PugiXmlDocument::SetDirty(bool b) { m_bDirty = b; }
bool PugiXmlDocument::IsDirty() { return m_bDirty; }

const char *PugiXmlDocument::GetPath() { return m_strPath.c_str(); }
} // namespace ui