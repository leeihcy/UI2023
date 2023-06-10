#ifndef _UI_IXMLWRAP_H_
#define _UI_IXMLWRAP_H_

namespace UI
{
interface IMapAttribute;
interface IUIElement;
interface IUIDocument;

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

interface UIElement;
interface UIAPI IUIElement
{
public:
	IUIElement(UIElement*);
	UIElement*  GetImpl();

    long  AddRef();
    long  Release();

    IUIDocument*  GetDocument(); 

	LPCTSTR  GetData();
    LPCTSTR  GetTagName();
    bool  SetData(LPCTSTR szText);
    void  SetTagName(LPCTSTR);

    void  GetAttribList(IMapAttribute** ppMapAttrib);
    void  GetAttribList2(IListAttribute** ppListAttrib);
    bool  SetAttribList2(IListAttribute* pListAttrib);

    bool  GetAttrib(LPCTSTR szKey, BSTR* pbstrValue);
    bool  HasAttrib(LPCTSTR szKey);
    bool  AddAttrib(LPCTSTR szKey, LPCTSTR szValue);
    bool  SetAttrib(LPCTSTR szKey, LPCTSTR szValue);
    bool  RemoveAttrib(LPCTSTR szKey);
    bool  ModifyAttrib(LPCTSTR szKey, LPCTSTR szValue);
    bool  ClearAttrib();

    IUIElementProxy  AddChild(LPCTSTR szNodeName);
    bool  RemoveChild(IUIElement*);
    bool  RemoveSelfInParent();

    bool  AddChildBefore(IUIElement*  pElem, IUIElement* pInsertBefore);
    bool  AddChildAfter(IUIElement*  pElem, IUIElement* pInsertAfter);
    IUIElementProxy  AddChildBefore(LPCTSTR szNodeName, IUIElement* pInsertBefore);
    IUIElementProxy  AddChildAfter(LPCTSTR szNodeName, IUIElement* pInsertAfter);
    //bool  MoveChildAfterChild(IUIElement* pChild2Move, IUIElement* pChildInsertAfter);
    bool  MoveTo(IUIElement* pNewParent, IUIElement* pChildInsertAfter);

    IUIElementProxy  FirstChild();
    IUIElementProxy  NextElement();
    IUIElementProxy  FindChild(LPCTSTR szChildName);

private:
	UIElement*  m_pImpl;
};

interface UIDocument;
interface UIAPI IUIDocument
{
public:
	IUIDocument(UIDocument*);

    long  AddRef();
    long  Release();

    void  SetDirty(bool b);
    bool  IsDirty();

    LPCTSTR  GetSkinPath();
    IUIElementProxy  FindElem(LPCTSTR szText);

private:
	UIDocument*  m_pImpl;
};

}

#endif // _UI_IXMLWRAP_H_