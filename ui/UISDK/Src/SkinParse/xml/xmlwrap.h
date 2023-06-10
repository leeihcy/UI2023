#pragma once
// UISDK 内部接口。对外使用IUIElement/IUIDocument

namespace UI
{

interface UIDocument;
interface IUIDocument;
interface IUIElement;
interface UIElement;
interface IMapAttribute;
interface IListAttribute;

// 模拟 share_ptr 实现UIElement生命周期管理。
// 调用者只需要直接拿到UIElementProxy返回值进行调用即可，不用负责销毁

class UIElementProxy
{
public:
	UIElementProxy();
	UIElementProxy(UIElement* pNew);
	~UIElementProxy();

	UIElementProxy(const UIElementProxy& o);
	UIElementProxy& operator=(const UIElementProxy& o);

	UIElement* operator->();
	operator bool() const;
	UIElement* get();
    UIElement* detach();

private:
	void  release();

private:
    UIElement* m_ptr;
};

interface UIElement
{
	virtual ~UIElement() {};

	virtual long  AddRef() = 0;
	virtual long  Release() = 0;

	virtual IUIElement*  GetIUIElement() = 0;

	virtual LPCTSTR  GetTagName() = 0;
	virtual void  SetTagName(LPCTSTR) = 0;
	virtual UIDocument*  GetDocument() = 0; 

	virtual LPCTSTR  GetData() = 0;
	virtual bool  SetData(LPCTSTR szText) = 0;

	virtual UIElementProxy  AddChild(LPCTSTR szNodeName) = 0;
    virtual bool  AddChild(UIElement* p) = 0;
	virtual bool  AddChildBefore(UIElement*  pElem, UIElement* pInsertBefore) = 0;
	virtual bool  AddChildAfter(UIElement*  pElem, UIElement* pInsertAfter) = 0;
    virtual UIElementProxy  AddChildBefore(LPCTSTR szNodeName, UIElement* pInsertBefore) = 0;
    virtual UIElementProxy  AddChildAfter(LPCTSTR szNodeName, UIElement* pInsertAfter) = 0;
    virtual bool  MoveTo(UIElement* pNewParent, UIElement* pChildInsertAfter) = 0;
	virtual bool  RemoveChild(UIElement*) = 0;
	virtual bool  RemoveSelfInParent() = 0;
	virtual UIElementProxy  FirstChild() = 0;
	virtual UIElementProxy  NextElement() = 0;
	virtual UIElementProxy  FindChild(LPCTSTR szChildName) = 0;

	virtual void  GetAttribList(IMapAttribute** ppMapAttrib) = 0;
	virtual void  GetAttribList2(IListAttribute** ppListAttrib) = 0;
	virtual bool  SetAttribList2(IListAttribute* pListAttrib) = 0;
	virtual bool  GetAttrib(LPCTSTR szKey, BSTR* pbstrValue) = 0;
	virtual bool  HasAttrib(LPCTSTR szKey) = 0;
	virtual bool  AddAttrib(LPCTSTR szKey, LPCTSTR szValue) = 0;
	virtual bool  SetAttrib(LPCTSTR szKey, LPCTSTR szValue) = 0;
	virtual bool  RemoveAttrib(LPCTSTR szKey) = 0;
	virtual bool  ModifyAttrib(LPCTSTR szKey, LPCTSTR szValue) = 0;
	virtual bool  ClearAttrib() = 0;

	virtual bool  GetAttribInt(LPCTSTR szKey, int* pInt) = 0;
	virtual bool  AddAttribInt(LPCTSTR szKey, int nInt) = 0;
};


interface UIDocument
{
	virtual long  AddRef() = 0;
	virtual long  Release() = 0;

	virtual IUIDocument*  GetIUIDocument() = 0;

	virtual void  SetSkinPath(LPCTSTR szPath) = 0;
	virtual LPCTSTR  GetSkinPath() = 0;

	virtual bool  LoadFile(LPCTSTR szFile) = 0;
	virtual bool  LoadData(const byte*  pData, int nDataSize) = 0;
	virtual UIElementProxy  FindElem(LPCTSTR szText) = 0;
	virtual bool  Save() = 0;
	virtual bool  SaveAs(LPCTSTR szPath) = 0;
	virtual LPCTSTR  GetPath() = 0;

	virtual void  SetDirty(bool b) = 0;
	virtual bool  IsDirty() = 0;
};

enum XML_ENGINE
{
	XML_ENGINE_DEFAULT,
	// MSXML, 放弃。 1. com方式调用太费劲。2. 有些用户电脑上注册表一出问题，就会创建失败
	PUGIXML,
};
bool  CreateXmlDocument(XML_ENGINE e, UIDocument**);


}