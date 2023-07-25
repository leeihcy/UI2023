#pragma once
#include "../3rd/pugixml/pugixml.hpp"
#include "../xmlwrap.h"
#include "include/interface/ixmlwrap.h"

namespace ui
{

struct IMapAttribute;
struct IUIElement;

class PugiXmlElement : public UIElement
{
public:
    PugiXmlElement(pugi::xml_node& node, UIDocument* pDoc);
    ~PugiXmlElement();

	virtual IUIElement*  GetIUIElement() override;

public:
    virtual long  AddRef() override;
    virtual long  Release() override;

    virtual const wchar_t*  GetTagName() override;
	virtual void  SetTagName(const wchar_t*) override;
    virtual UIDocument*  GetDocument() override; 

    virtual const wchar_t*  GetData() override;
    virtual bool  SetData(const wchar_t* szText) override;

    virtual UIElementProxy  AddChild(const wchar_t* szNodeName) override;
    virtual bool  AddChild(UIElement* p) override;
    virtual bool  AddChildBefore(UIElement*  pElem, UIElement* pInsertBefore) override;
    virtual bool  AddChildAfter(UIElement*  pElem, UIElement* pInsertAfter) override;
    virtual UIElementProxy  AddChildBefore(const wchar_t* szNodeName, UIElement* pInsertBefore) override;
    virtual UIElementProxy  AddChildAfter(const wchar_t* szNodeName, UIElement* pInsertAfter) override;
    // virtual bool  MoveChildAfterChild(UIElement* pChild2Move, UIElement* pChildInsertAfter) override;
    virtual bool  MoveTo(UIElement* pNewParent, UIElement* pChildInsertAfter) override;
    virtual bool  RemoveChild(UIElement*) override;
    virtual bool  RemoveSelfInParent() override;
	virtual UIElementProxy  FirstChild() override;
	virtual UIElementProxy  NextElement() override;
	virtual UIElementProxy  FindChild(const wchar_t* szChildName) override;

    virtual void  GetAttribList(IMapAttribute** ppMapAttrib) override;
    virtual void  GetAttribList2(IListAttribute** ppListAttrib) override;
    virtual bool  SetAttribList2(IListAttribute* pListAttrib) override;
#if defined(OS_WIN)
    virtual bool  GetAttrib(const wchar_t* szKey, BSTR* pbstrValue) override;
#endif
    virtual std::wstring GetAttrib(const wchar_t* szKey) override;

    virtual bool  HasAttrib(const wchar_t* szKey) override;
    virtual bool  AddAttrib(const wchar_t* szKey, const wchar_t* szValue) override;
	virtual bool  SetAttrib(const wchar_t* szKey, const wchar_t* szValue) override;
    virtual bool  RemoveAttrib(const wchar_t* szKey) override;
    virtual bool  ModifyAttrib(const wchar_t* szKey, const wchar_t* szValue) override;
    virtual bool  ClearAttrib() override;

    virtual bool  GetAttribInt(const wchar_t* szKey, int* pInt) override;
    virtual bool  AddAttribInt(const wchar_t* szKey, int nInt) override;

	void  set_attr_by_prefix(IListAttribute* pListAttrib, const wchar_t* szPrefix);
    void  enum_attr(const std::function<void(const wchar_t*, const wchar_t*)>& callback);

private:
    long  m_lRef;
    pugi::xml_node  m_node;
    UIDocument*  m_pDocument;  // 没有引用计数

	IUIElement  m_IUIElement;
};

class PugiXmlDocument : public UIDocument
{
public:
    PugiXmlDocument();
    ~PugiXmlDocument();

    virtual long  AddRef() override;
    virtual long  Release() override;

	virtual IUIDocument*  GetIUIDocument() override;

	virtual void  SetSkinPath(const wchar_t* szPath) override;
	virtual const wchar_t*  GetSkinPath() override;

    virtual bool  LoadFile(const wchar_t* szFile) override;
    virtual bool  LoadData(const byte*  pData, int nDataSize) override;
    virtual UIElementProxy  FindElem(const wchar_t* szText) override;
    virtual bool  Save() override;
    virtual bool  SaveAs(const wchar_t* szPath) override;
    virtual const wchar_t*  GetPath() override;

    virtual void  SetDirty(bool b) override;
    virtual bool  IsDirty() override;

private:
    String  m_strPath;  // 全路径
	String  m_strSkinPath;   // 在皮肤包中的路径

    long    m_lRef;
    bool    m_bDirty;

    pugi::xml_document m_doc;

	IUIDocument  m_IUIDocument;
};

}