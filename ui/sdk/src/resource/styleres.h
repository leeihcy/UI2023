#pragma once
#include "inc.h"

namespace ui
{
	class ResBundle;
	struct IStyleRes;

// 1. �ؼ�����һ��class��ʽ��
//    styleclass="imagebtn;aaa" 
// 
// 2. Tag ��ʽ
//       <CustomWindow font="normal" transparentrgn.type="4corner" transparentrgn.9region="5"/>
// 
// 3. Id ��ʽ 
//       <id id="imagebtn" xxx="yyy"/>
//
// 4. class ��ʽ
//       <class id="imagebtn" xxx="yyy"/>
//

struct IStyleResItem;
struct UIElement;

class StyleResItem
{
public:
	StyleResItem();
	~StyleResItem();
    IStyleResItem*  GetIStyleResItem();

    const wchar_t*  GetId() { 
        return m_strID.c_str();
    }
    void  SetId(const wchar_t* szId){
        if (szId)
            m_strID = szId;
        else
            m_strID.clear();
    }

	STYLE_SELECTOR_TYPE  GetSelectorType() { return m_eSelectorType; }
	void  SetSelectorType(STYLE_SELECTOR_TYPE type) { m_eSelectorType = type; }
	
	const wchar_t*  GetInherits() { return m_strInherits.c_str(); }
	void  SetInherits(const wchar_t* sz);
	int   GetInheritCount();
	bool  GetInheritItem(int i, String& str);
	bool  RemoveInheritItem(const wchar_t* sz);

	void  SetXmlElement(UIElement* p);
	UIElement*  GetXmlElement();
	bool  GetXmlElement2(UIElement** pp);

    void  SetAttributeMap(IMapAttribute* pMapAttrib);
	void  GetAttributeMap(IMapAttribute** ppMapAttrib);
	void  SetAttribute( const wchar_t* key, const wchar_t* value);
	const wchar_t*  GetAttribute(const wchar_t* key);
	bool  ModifyAttribute(const wchar_t* key, const wchar_t* value);
	bool  RemoveAttribute(const wchar_t* key);

	bool  InheritAttribute(const wchar_t* key, const wchar_t* value);
	bool  InheritMyAttributesToAnother(StyleResItem* pChild);
	bool  Apply(IMapAttribute* pMapAttrib, bool bOverwrite);

private:
    IStyleResItem*  m_pIStyleResItem;

	STYLE_SELECTOR_TYPE  m_eSelectorType;
	String  m_strID;          // ��ѡ������ID
	String  m_strInherits;    // �̳��б�
    IMapAttribute*  m_pMapAttrib;  // ��ѡ���������Լ�

	// �ǳ־�����
	std::vector<String>  m_vInheritList;   // �̳��б������н���m_strInherits

	// editor����
	UIElement*  m_pXmlElement;
};

class StyleRes
{
public:
    StyleRes(ResBundle* p);
	~StyleRes();
    IStyleRes&  GetIStyleRes();
	void  Clear();

public:
	StyleResItem*  Add(STYLE_SELECTOR_TYPE type, const wchar_t* szId, const wchar_t* szInherit);
	StyleResItem*  Insert(STYLE_SELECTOR_TYPE type, const wchar_t* szId, long lPos);
	bool  Modify(StyleResItem* pItem, STYLE_SELECTOR_TYPE type, const wchar_t* szId, const wchar_t* szInherit);
	bool  Remove(STYLE_SELECTOR_TYPE type, const wchar_t* szId);

	bool  AddAttribute(StyleResItem* pItem, const wchar_t* szKey, const wchar_t* szValue);
	bool  ModifyAttribute(StyleResItem* pItem, const wchar_t* szKey, const wchar_t* szValue);
	bool  RemoveAttribute(StyleResItem* pItem, const wchar_t* szKey);

	bool  Add(StyleResItem* pNewItem);
	bool  Remove(StyleResItem* pNewItem);

    long  GetCount();
	StyleResItem* GetItem(long nIndex);
	long  GetItemPos(StyleResItem* p);
	StyleResItem* GetItem(STYLE_SELECTOR_TYPE type, const wchar_t* szID);

	bool  LoadStyle(const wchar_t* szTagName, const wchar_t* szStyleClass, const wchar_t* szID, IMapAttribute* pMapAttribute);
    bool  UnloadStyle(const wchar_t* szTagName, const wchar_t* szStyleClass, const wchar_t* szID, IListAttribute* pListAttribte);

private:
    IStyleRes*  m_pIStyleRes;

	ResBundle*  m_pSkinRes;
	std::vector<StyleResItem*>  m_vStyles;
};
}