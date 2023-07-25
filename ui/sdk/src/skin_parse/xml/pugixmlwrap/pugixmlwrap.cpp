#include "include/inc.h"
#include "pugixmlwrap.h"
#include "include/interface/imapattr.h"
#include "src/util/util.cpp"

namespace ui {
PugiXmlElement::PugiXmlElement(pugi::xml_node& node, UIDocument* pDoc) :
	m_IUIElement(this)
{
    m_lRef = 0;
    m_node = node;
    m_pDocument = pDoc;
}
PugiXmlElement::~PugiXmlElement()
{
    
}

IUIElement*  PugiXmlElement::GetIUIElement()
{
	return &m_IUIElement;
}

long  PugiXmlElement::AddRef()
{
    return ++m_lRef;
}
long  PugiXmlElement::Release()
{
    m_lRef--;

    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }

    return m_lRef;
}


const wchar_t*  PugiXmlElement::GetTagName()
{
    return m_node.name();
}

void  PugiXmlElement::SetTagName(const wchar_t* szTag)
{
	m_node.set_name(szTag);
}
UIDocument*  PugiXmlElement::GetDocument()
{
    return m_pDocument;
}

const wchar_t*  PugiXmlElement::GetData()
{
    return m_node.text().as_string();
}

bool  PugiXmlElement::SetData(const wchar_t* szText)
{
    m_node.text() = szText;
    return true;
}

UIElementProxy PugiXmlElement::AddChild(const wchar_t* szNodeName)
{
	if (!szNodeName)
		return UIElementProxy();

	pugi::xml_node node = m_node.append_child(szNodeName);
	if (node.empty())
		return UIElementProxy();

	return UIElementProxy(new PugiXmlElement(node, m_pDocument));
}

bool  PugiXmlElement::AddChild(UIElement* p)
{
    if (!p)
        return false;

    PugiXmlElement* pElement = static_cast<PugiXmlElement*>(p);
    return m_node.append_copy(pElement->m_node).empty() ? false:true;
}

bool  PugiXmlElement::RemoveChild(UIElement* p)
{
	if (!p)
		return false;

	PugiXmlElement* pElem = (PugiXmlElement*)p;
	return m_node.remove_child(pElem->m_node);
}

bool  PugiXmlElement::RemoveSelfInParent()
{
	if (m_node.parent().empty())
		return false;

	return m_node.parent().remove_child(m_node);
}

// ���Ϊ�գ����ʾ���������
bool  PugiXmlElement::AddChildBefore(UIElement* pElem, UIElement* pInsertBefore)
{
	if (!pElem)
		return false;

	if (!pInsertBefore)
		return AddChild(pElem);

	PugiXmlElement* pElement = static_cast<PugiXmlElement*>(pElem);
	PugiXmlElement* pElementBefore = static_cast<PugiXmlElement*>(pInsertBefore);

	return m_node.insert_copy_before(pElement->m_node, pElementBefore->m_node).empty() ? false:true;
}

bool  PugiXmlElement::AddChildAfter(UIElement*  pElem, UIElement* pInsertAfter)
{
	if (!pElem)
		return false;

	PugiXmlElement* pElement = static_cast<PugiXmlElement*>(pElem);

	// ������ǰ��
	if (!pInsertAfter)
	{
		return m_node.prepend_copy(pElement->m_node).empty() ? false:true;
	}

	// �鿴after����һ����������ǰ��
	PugiXmlElement* pElementInsertAfter = static_cast<PugiXmlElement*>(pInsertAfter);

	return m_node.insert_copy_after(pElement->m_node, pElementInsertAfter->m_node).empty() ? false:true;
}


// bool  PugiXmlElement::MoveChildAfterChild(UIElement* pChild2Move, UIElement* pChildInsertAfter)
// {
// 	if (!pChild2Move)
// 		return false;
// 
// 	PugiXmlElement* _pChild2Move = static_cast<PugiXmlElement*>(pChild2Move);
// 
// 	if (pChildInsertAfter)
// 	{
// 		PugiXmlElement* _pChildInsertAfter = static_cast<PugiXmlElement*>(pChildInsertAfter);
// 		pugi::xml_node new_node = m_node.insert_copy_after(_pChild2Move->m_node, _pChildInsertAfter->m_node);
// 		if (new_node.empty())
// 			return false;
// 
// 		m_node.remove_child(_pChild2Move->m_node);
// 		_pChild2Move->m_node = new_node;
// 	}
// 	else
// 	{
// 		pugi::xml_node new_node = m_node.prepend_copy(_pChild2Move->m_node);
// 		if (new_node.empty())
// 			return false;
// 
// 		m_node.remove_child(_pChild2Move->m_node);
// 		_pChild2Move->m_node = new_node;
// 	}
// 
// 	return true;
// }

bool  PugiXmlElement::MoveTo(UIElement* _pNewParent, UIElement* _pChildInsertAfter)
{
    if (!m_node)
        return false;

    PugiXmlElement* pNewParent = 
        static_cast<PugiXmlElement*>(_pNewParent);
    PugiXmlElement* pInsertAfter = 
        static_cast<PugiXmlElement*>(_pChildInsertAfter);

    if (!pNewParent)
    {
        UIASSERT(0); // TODO:
        return false;
    }
    pugi::xml_node new_node;

    if (pInsertAfter && pInsertAfter->m_node)
    {
        // λ��û�иı䣬ֱ�ӷ���
        if (m_node.previous_sibling() == pInsertAfter->m_node)
        {
            UIASSERT(0);
            return false;
        }

        new_node = pNewParent->m_node.
            insert_copy_after(m_node, pInsertAfter->m_node);
    }
    else
    {
        if (!m_node.previous_sibling())
        {
            // λ��û�иı䣬ֱ�ӷ���
            UIASSERT(0);
            return false;
        }
        new_node = pNewParent->m_node.prepend_copy(m_node);
    }

    if (new_node.empty())
        return false;

    // ���Լ��Ӹ�����Ƴ�
    if (m_node.parent())
    {
        m_node.parent().remove_child(m_node);
    }
    m_node = new_node;

    return true;
}


// ���Ϊ�գ����ʾ���������
UIElementProxy  PugiXmlElement::AddChildBefore(const wchar_t* szNodeName, UIElement* pInsertBefore)
{
	if (!szNodeName)
		return UIElementProxy();

	if (!pInsertBefore)
		return AddChild(szNodeName);

	PugiXmlElement* pElementInsertBefore = static_cast<PugiXmlElement*>(pInsertBefore);
	pugi::xml_node node = m_node.insert_child_before(szNodeName, pElementInsertBefore->m_node);
	if (node.empty())
		return UIElementProxy();

    return UIElementProxy(new PugiXmlElement(node, m_pDocument));
}

UIElementProxy  PugiXmlElement::AddChildAfter(const wchar_t* szNodeName, UIElement* pInsertAfter)
{
	if (!szNodeName)
		return UIElementProxy();

	pugi::xml_node node;

	// ������ǰ��
	if (!pInsertAfter)
	{
		node = m_node.prepend_child(szNodeName);
	}
	else
	{
		PugiXmlElement* pElementInsertAfter = static_cast<PugiXmlElement*>(pInsertAfter);
		node = m_node.insert_child_after(szNodeName, pElementInsertAfter->m_node);
	}
	
	if (node.empty())
		return UIElementProxy();

	return UIElementProxy(new PugiXmlElement(node, m_pDocument));
}

UIElementProxy PugiXmlElement::FirstChild()
{
    pugi::xml_node nodeChild = m_node.first_child();
	if (!nodeChild)
		return UIElementProxy();

    return UIElementProxy(new PugiXmlElement(nodeChild, m_pDocument));
}

UIElementProxy  PugiXmlElement::NextElement()
{
    pugi::xml_node nodeNext = m_node;
    while ((nodeNext = nodeNext.next_sibling()))
    {
        if (nodeNext.type() == pugi::node_comment)
            continue;

        break;
    }

	if (!nodeNext)
		return UIElementProxy();
    
	return UIElementProxy(new PugiXmlElement(nodeNext, m_pDocument));
}

UIElementProxy PugiXmlElement::FindChild(const wchar_t* szChildName)
{
    if (!szChildName)
        return UIElementProxy();

    pugi::xml_node nodeChild = m_node.child(szChildName);
    return UIElementProxy(new PugiXmlElement(nodeChild, m_pDocument));
}


void  PugiXmlElement::enum_attr(
        const std::function<void(const wchar_t*, const wchar_t*)>& callback)
{

    for (pugi::xml_attribute attr = m_node.first_attribute();
        !attr.empty();
        attr = attr.next_attribute())
    {
        callback(attr.name(), attr.as_string());
    }

    // �����ӽ���е�����
    pugi::xml_node nodeProp = m_node.child(XML_PROP);
    if (!nodeProp)
        return;
    /*
    bkgnd.render.type = "color" bkgnd.render.color = "0xFFF" ��Ч���������ʽ��

    <prop key="bkgnd.render" type="color" color = "0xFFF"/>  // render tag����δʹ��
    */

    while (nodeProp)
    {
        String strKey;

        // ���û������key������keyΪ�գ��򲻼�ǰ׺
        pugi::xml_attribute attr = nodeProp.attribute(XML_KEY);
        if (!attr.empty())
        {
            strKey = attr.as_string();
        }

        attr = nodeProp.first_attribute();
        for (; !attr.empty(); attr = attr.next_attribute())
        {
            if (0 == wcscmp(attr.name(), XML_KEY))
                continue;

            if (strKey.empty())
            {
                callback(attr.name(), attr.as_string());
            }
            else
            {
                String strName = strKey;
                strName.append(TEXT("."));
                strName.append(attr.name());
                callback(strName.c_str(), attr.as_string());
            }
        }

        nodeProp = nodeProp.next_sibling(XML_PROP);
    }
}

void  PugiXmlElement::GetAttribList(IMapAttribute** ppMapAttrib)
{
    // �п�����������ȡ
    if (*ppMapAttrib == nullptr)
        UICreateIMapAttribute(ppMapAttrib);

    //(*ppMapAttrib)->SetTag(m_node.name());

    enum_attr([ppMapAttrib](const wchar_t* k, const wchar_t* v){
        (*ppMapAttrib)->AddAttr(k, v);
    });
}
void  PugiXmlElement::GetAttribList2(IListAttribute** ppListAttrib)
{
    UICreateIListAttribute(ppListAttrib);

    //(*ppListAttrib)->SetTag(m_node.name());

    enum_attr([ppListAttrib](const wchar_t* k, const wchar_t* v){
        (*ppListAttrib)->AddAttr(k, v);
    });
}



void  PugiXmlElement::set_attr_by_prefix(IListAttribute* pListAttrib, const wchar_t* szPrefix)
{
	UIASSERT(pListAttrib);
	UIASSERT(szPrefix);

	int nLength = (int)wcslen(szPrefix);

	pListAttrib->BeginEnum();

	const wchar_t* szKey = nullptr;
	const wchar_t* szValue = nullptr;
	pugi::xml_node propnode;
	while (pListAttrib->EnumNext(&szKey, &szValue))
	{
		if (wcsstr(szKey, szPrefix) == szKey)
		{
			if (!propnode)
			{
				propnode = m_node.prepend_child(XML_PROP);
				propnode.append_attribute(XML_KEY).set_value(szPrefix);
			}

			// +1 ���Ϻ����һ�� . 
			propnode.append_attribute(szKey+nLength+1).set_value(szValue);
			pListAttrib->EraseAttr(szKey);
		}
	}
	pListAttrib->EndEnum();
}

bool  PugiXmlElement::SetAttribList2(IListAttribute* pListAttrib)
{
	if (!pListAttrib)
		return false;

	ClearAttrib();

	const wchar_t* szPrefixArray[] = { 
		XML_BACKGND_RENDER_PREFIX XML_RENDER, 
		XML_FOREGND_RENDER_PREFIX XML_RENDER,
        XML_MASK_RENDER_PREFIX    XML_RENDER,
		XML_TEXTRENDER,
		XML_LAYOUT,        
	};

	int nCount = sizeof(szPrefixArray) / sizeof(const wchar_t*);
	for (int i = 0; i < nCount; i++)
	{
		set_attr_by_prefix(pListAttrib, szPrefixArray[i]);
	}

	pListAttrib->BeginEnum();
	const wchar_t* szKey = nullptr;
	const wchar_t* szValue = nullptr;
	while (pListAttrib->EnumNext(&szKey, &szValue))
	{
		AddAttrib(szKey, szValue);
	}
	pListAttrib->EndEnum();

	return true;
}
#if defined(OS_WIN)
bool  PugiXmlElement::GetAttrib(const wchar_t* szKey, BSTR* pbstrValue)
{
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

std::wstring PugiXmlElement::GetAttrib(const wchar_t* szKey) 
{
    if (nullptr == szKey)
        return std::wstring();

    pugi::xml_attribute attr = m_node.attribute(szKey);
    if (attr.empty())
        return std::wstring();

    return std::wstring(attr.as_string());
}

bool  PugiXmlElement::HasAttrib(const wchar_t* szKey)
{
    if (nullptr == szKey)
        return false;

    if (0 == wcslen(szKey))
        return false;

    return (!m_node.attribute(szKey).empty());
}
bool  PugiXmlElement::AddAttrib(const wchar_t* szKey, const wchar_t* szValue)
{
    if (nullptr == szKey || nullptr == szValue)
        return false;

    m_node.append_attribute(szKey) = szValue;

    return true;
}

bool  PugiXmlElement::SetAttrib(const wchar_t* szKey, const wchar_t* szValue)
{
	if (nullptr == szKey || nullptr == szValue)
		return false;

	pugi::xml_attribute attr = m_node.attribute(szKey);
	if (attr.empty())
		return false;

	return attr.set_value(szValue);
}

bool  PugiXmlElement::RemoveAttrib(const wchar_t* szKey)
{
    if (nullptr == szKey)
        return false;

    return m_node.remove_attribute(szKey);
}
bool  PugiXmlElement::ModifyAttrib(const wchar_t* szKey, const wchar_t* szValue)
{
    return AddAttrib(szKey, szValue);
}
bool  PugiXmlElement::ClearAttrib()
{
	pugi::xml_attribute_iterator iter = m_node.attributes_begin();
	for (; iter != m_node.attributes_end(); ++iter)
	{
		m_node.remove_attribute(*iter);
	}

    // �Ƴ����е�prop�ӽ��
    pugi::xml_node nodeProp;
    while ((nodeProp = m_node.child(XML_PROP)))
    {
        m_node.remove_child(nodeProp);
    }
    return true;
}

bool  PugiXmlElement::GetAttribInt(const wchar_t* szKey, int* pInt)
{
	if (nullptr == szKey || nullptr == pInt)
		return false;

	std::wstring  bstrValue = GetAttrib(szKey);
	if (bstrValue.empty())
		return false;

	*pInt = util::wtoi(bstrValue.c_str());
	return true;
}
bool  PugiXmlElement::AddAttribInt(const wchar_t* szKey, int nInt)
{
	wchar szValue[16] = {0};
	wprintf(szValue, TEXT("%d"), nInt);

	return AddAttrib(szKey, szValue);
}
//////////////////////////////////////////////////////////////////////////


PugiXmlDocument::PugiXmlDocument() : m_IUIDocument(this)
{
    m_bDirty = false;
	m_lRef = 0;
}
PugiXmlDocument::~PugiXmlDocument()
{
}

IUIDocument*  PugiXmlDocument::GetIUIDocument()
{
	return &m_IUIDocument;
}

void  PugiXmlDocument::SetSkinPath(const wchar_t* szPath)
{
	if (szPath)
        m_strSkinPath = szPath;
	else
        m_strSkinPath.clear();
}
const wchar_t*  PugiXmlDocument::GetSkinPath()
{
    return m_strSkinPath.c_str();
}

bool  PugiXmlDocument::LoadFile(const wchar_t* szFile)
{
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

bool  PugiXmlDocument::LoadData(const byte*  pData, int nDataSize)
{
    if (!pData || nDataSize <= 0)
        return false;

    m_strPath.clear();
    if (pugi::status_ok != m_doc.load_buffer((void*)pData, nDataSize).status)
        return false;
    
    return true;
}
UIElementProxy PugiXmlDocument::FindElem(const wchar_t* szText)
{
    if (!szText)
        return UIElementProxy();

    pugi::xml_node node = m_doc.child(szText);
    if (node.empty())
    {
        return UIElementProxy();
    }

    return UIElementProxy(new PugiXmlElement(node, this));
}

bool  PugiXmlDocument::Save()
{
    return m_doc.save_file(m_strPath.c_str(), PUGIXML_TEXT("\t"), 
            pugi::format_default | pugi::format_write_bom, pugi::encoding_auto);
}

bool  PugiXmlDocument::SaveAs(const wchar_t* szPath)
{
    return m_doc.save_file(szPath, PUGIXML_TEXT("\t"), 
            pugi::format_default | pugi::format_write_bom, pugi::encoding_auto);
}

long  PugiXmlDocument::Release()
{
    m_lRef --;
    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }
    return m_lRef;
}
long  PugiXmlDocument::AddRef()
{
    return ++m_lRef;
}

void  PugiXmlDocument::SetDirty(bool b)
{
    m_bDirty = b;
}
bool  PugiXmlDocument::IsDirty()
{
    return m_bDirty;
}

const wchar_t*  PugiXmlDocument::GetPath()
{ 
    return m_strPath.c_str();
}
}