#pragma once
// UISDK 内部接口。对外使用IUIElement/IUIDocument
#include <string>

namespace ui
{

struct UIDocument;
struct IUIDocument;
struct IUIElement;
struct UIElement;
struct IMapAttribute;
struct IListAttribute;

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

struct UIElement
{
	virtual ~UIElement() {};

	virtual long  AddRef() = 0;
	virtual long  Release() = 0;

	virtual IUIElement*  GetIUIElement() = 0;

	virtual const wchar_t*  GetTagName() = 0;
	virtual void  SetTagName(const wchar_t*) = 0;
	virtual UIDocument*  GetDocument() = 0; 

	virtual const wchar_t*  GetData() = 0;
	virtual bool  SetData(const wchar_t* szText) = 0;

	virtual UIElementProxy  AddChild(const wchar_t* szNodeName) = 0;
    virtual bool  AddChild(UIElement* p) = 0;
	virtual bool  AddChildBefore(UIElement*  pElem, UIElement* pInsertBefore) = 0;
	virtual bool  AddChildAfter(UIElement*  pElem, UIElement* pInsertAfter) = 0;
    virtual UIElementProxy  AddChildBefore(const wchar_t* szNodeName, UIElement* pInsertBefore) = 0;
    virtual UIElementProxy  AddChildAfter(const wchar_t* szNodeName, UIElement* pInsertAfter) = 0;
    virtual bool  MoveTo(UIElement* pNewParent, UIElement* pChildInsertAfter) = 0;
	virtual bool  RemoveChild(UIElement*) = 0;
	virtual bool  RemoveSelfInParent() = 0;
	virtual UIElementProxy  FirstChild() = 0;
	virtual UIElementProxy  NextElement() = 0;
	virtual UIElementProxy  FindChild(const wchar_t* szChildName) = 0;

	virtual void GetAttribList(IMapAttribute*) = 0;
	virtual void  GetAttribList2(IListAttribute** ppListAttrib) = 0;
	virtual bool  SetAttribList2(IListAttribute* pListAttrib) = 0;
#if defined(OS_WIN)
	virtual bool  GetAttrib(const wchar_t* szKey, BSTR* pbstrValue) = 0;
#endif
    virtual std::wstring GetAttrib(const wchar_t* szKey) = 0;
    
	virtual bool  HasAttrib(const wchar_t* szKey) = 0;
	virtual bool  AddAttrib(const wchar_t* szKey, const wchar_t* szValue) = 0;
	virtual bool  SetAttrib(const wchar_t* szKey, const wchar_t* szValue) = 0;
	virtual bool  RemoveAttrib(const wchar_t* szKey) = 0;
	virtual bool  ModifyAttrib(const wchar_t* szKey, const wchar_t* szValue) = 0;
	virtual bool  ClearAttrib() = 0;

	virtual bool  GetAttribInt(const wchar_t* szKey, int* pInt) = 0;
	virtual bool  AddAttribInt(const wchar_t* szKey, int nInt) = 0;
};


struct UIDocument
{
	virtual long  AddRef() = 0;
	virtual long  Release() = 0;

	virtual IUIDocument*  GetIUIDocument() = 0;

	virtual void  SetSkinPath(const wchar_t* szPath) = 0;
	virtual const wchar_t*  GetSkinPath() = 0;

	virtual bool  LoadFile(const wchar_t* szFile) = 0;
	virtual bool  LoadData(const unsigned char*  pData, int nDataSize) = 0;
	virtual UIElementProxy  FindElem(const wchar_t* szText) = 0;
	virtual bool  Save() = 0;
	virtual bool  SaveAs(const wchar_t* szPath) = 0;
	virtual const wchar_t*  GetPath() = 0;

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