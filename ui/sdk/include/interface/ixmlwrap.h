#ifndef _UI_IXMLWRAP_H_
#define _UI_IXMLWRAP_H_
#include "include/macro/uidefine.h"

namespace ui
{
struct IMapAttribute;
struct IUIElement;
struct IUIDocument;

// 模拟 share_ptr 实现IUIElement生命周期管理。
// 调用者只需要直接拿到IUIElementProxy返回值进行调用即可，不用负责销毁
class UIAPI IUIElementProxy
{
public:
    IUIElementProxy();
    IUIElementProxy(IUIElement* pNew);
    ~IUIElementProxy();

    IUIElementProxy(const IUIElementProxy& o);
    IUIElementProxy& operator=(const IUIElementProxy& o);

    IUIElement* operator->();
    operator bool() const;
    IUIElement* get();

private:
    void  release();

private:
    IUIElement* m_ptr;
};

struct UIElement;
struct UIAPI IUIElement
{
public:
	IUIElement(UIElement*);
	UIElement*  GetImpl();

    long  AddRef();
    long  Release();

    IUIDocument*  GetDocument(); 

	const wchar_t*  GetData();
    const wchar_t*  GetTagName();
    bool  SetData(const wchar_t* szText);
    void  SetTagName(const wchar_t*);

    void  GetAttribList(IMapAttribute** ppMapAttrib);
    void  GetAttribList2(IListAttribute** ppListAttrib);
    bool  SetAttribList2(IListAttribute* pListAttrib);

#if defined(OS_WIN)
    bool  GetAttrib(const wchar_t* szKey, BSTR* pbstrValue);
#endif
    bool  HasAttrib(const wchar_t* szKey);
    bool  AddAttrib(const wchar_t* szKey, const wchar_t* szValue);
    bool  SetAttrib(const wchar_t* szKey, const wchar_t* szValue);
    bool  RemoveAttrib(const wchar_t* szKey);
    bool  ModifyAttrib(const wchar_t* szKey, const wchar_t* szValue);
    bool  ClearAttrib();

    IUIElementProxy  AddChild(const wchar_t* szNodeName);
    bool  RemoveChild(IUIElement*);
    bool  RemoveSelfInParent();

    bool  AddChildBefore(IUIElement*  pElem, IUIElement* pInsertBefore);
    bool  AddChildAfter(IUIElement*  pElem, IUIElement* pInsertAfter);
    IUIElementProxy  AddChildBefore(const wchar_t* szNodeName, IUIElement* pInsertBefore);
    IUIElementProxy  AddChildAfter(const wchar_t* szNodeName, IUIElement* pInsertAfter);
    //bool  MoveChildAfterChild(IUIElement* pChild2Move, IUIElement* pChildInsertAfter);
    bool  MoveTo(IUIElement* pNewParent, IUIElement* pChildInsertAfter);

    IUIElementProxy  FirstChild();
    IUIElementProxy  NextElement();
    IUIElementProxy  FindChild(const wchar_t* szChildName);

private:
	UIElement*  m_pImpl;
};

struct UIDocument;
struct UIAPI IUIDocument
{
public:
	IUIDocument(UIDocument*);

    long  AddRef();
    long  Release();

    void  SetDirty(bool b);
    bool  IsDirty();

    const wchar_t*  GetSkinPath();
    IUIElementProxy  FindElem(const wchar_t* szText);

private:
	UIDocument*  m_pImpl;
};

}

#endif // _UI_IXMLWRAP_H_