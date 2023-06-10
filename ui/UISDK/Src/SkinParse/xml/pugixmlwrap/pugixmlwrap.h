#pragma once
#include "..\3rd\pugixml\pugixml.hpp"
#include "..\xmlwrap.h"
#include "Inc\Interface\ixmlwrap.h"

namespace UI
{

interface IMapAttribute;
interface IUIElement;

class PugiXmlElement : public UIElement
{
public:
    PugiXmlElement(pugi::xml_node& node, UIDocument* pDoc);
    ~PugiXmlElement();

	virtual IUIElement*  GetIUIElement() override;

public:
    virtual long  AddRef() override;
    virtual long  Release() override;

    virtual LPCTSTR  GetTagName() override;
	virtual void  SetTagName(LPCTSTR) override;
    virtual UIDocument*  GetDocument() override; 

    virtual LPCTSTR  GetData() override;
    virtual bool  SetData(LPCTSTR szText) override;

    virtual UIElementProxy  AddChild(LPCTSTR szNodeName) override;
    virtual bool  AddChild(UIElement* p) override;
    virtual bool  AddChildBefore(UIElement*  pElem, UIElement* pInsertBefore) override;
    virtual bool  AddChildAfter(UIElement*  pElem, UIElement* pInsertAfter) override;
    virtual UIElementProxy  AddChildBefore(LPCTSTR szNodeName, UIElement* pInsertBefore) override;
    virtual UIElementProxy  AddChildAfter(LPCTSTR szNodeName, UIElement* pInsertAfter) override;
    // virtual bool  MoveChildAfterChild(UIElement* pChild2Move, UIElement* pChildInsertAfter) override;
    virtual bool  MoveTo(UIElement* pNewParent, UIElement* pChildInsertAfter) override;
    virtual bool  RemoveChild(UIElement*) override;
    virtual bool  RemoveSelfInParent() override;
	virtual UIElementProxy  FirstChild() override;
	virtual UIElementProxy  NextElement() override;
	virtual UIElementProxy  FindChild(LPCTSTR szChildName) override;

    virtual void  GetAttribList(IMapAttribute** ppMapAttrib) override;
    virtual void  GetAttribList2(IListAttribute** ppListAttrib) override;
    virtual bool  SetAttribList2(IListAttribute* pListAttrib) override;
    virtual bool  GetAttrib(LPCTSTR szKey, BSTR* pbstrValue) override;
    virtual bool  HasAttrib(LPCTSTR szKey) override;
    virtual bool  AddAttrib(LPCTSTR szKey, LPCTSTR szValue) override;
	virtual bool  SetAttrib(LPCTSTR szKey, LPCTSTR szValue) override;
    virtual bool  RemoveAttrib(LPCTSTR szKey) override;
    virtual bool  ModifyAttrib(LPCTSTR szKey, LPCTSTR szValue) override;
    virtual bool  ClearAttrib() override;

    virtual bool  GetAttribInt(LPCTSTR szKey, int* pInt) override;
    virtual bool  AddAttribInt(LPCTSTR szKey, int nInt) override;

	void  set_attr_by_prefix(IListAttribute* pListAttrib, LPCTSTR szPrefix);
    void  enum_attr(const std::function<void(LPCTSTR, LPCTSTR)>& callback);

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

	virtual void  SetSkinPath(LPCTSTR szPath) override;
	virtual LPCTSTR  GetSkinPath() override;

    virtual bool  LoadFile(LPCTSTR szFile) override;
    virtual bool  LoadData(const byte*  pData, int nDataSize) override;
    virtual UIElementProxy  FindElem(LPCTSTR szText) override;
    virtual bool  Save() override;
    virtual bool  SaveAs(LPCTSTR szPath) override;
    virtual LPCTSTR  GetPath() override;

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