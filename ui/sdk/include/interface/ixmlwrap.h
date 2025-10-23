#ifndef _UI_IXMLWRAP_H_
#define _UI_IXMLWRAP_H_
#include "sdk/include/interface.h"
#include "sdk/include/macro/uidefine.h"

namespace ui {
struct IAttributeMap;
struct IUIElement;
struct IUIDocument;

// 模拟 share_ptr 实现IUIElement生命周期管理。
// 调用者只需要直接拿到IUIElementProxy返回值进行调用即可，不用负责销毁
class UIAPI IUIElementProxy {
public:
  IUIElementProxy();
  IUIElementProxy(IUIElement *pNew);
  ~IUIElementProxy();

  IUIElementProxy(const IUIElementProxy &o);
  IUIElementProxy &operator=(const IUIElementProxy &o);

  IUIElement *operator->();
  operator bool() const;
  IUIElement *get();

private:
  void release();

private:
  IUIElement *m_ptr;
};

struct UIElement;
struct UIAPI IUIElement {
public:
  IUIElement(UIElement *);
  UIElement *GetImpl();

  long AddRef();
  long Release();

  IUIDocument *GetDocument();

  const char *GetData();
  const char *GetTagName();
  bool SetData(const char *szText);
  void SetTagName(const char *);

  void GetAttribList(IAttributeMap *ppMapAttrib);
  void GetAttribList2(IAttributeList **ppListAttrib);
  bool SetAttribList2(IAttributeList *attribute_list);

#if 0 // defined(OS_WIN)
  bool GetAttrib(const char *szKey, BSTR *pbstrValue);
#endif
  bool HasAttrib(const char *szKey);
  bool AddAttrib(const char *szKey, const char *szValue);
  bool SetAttrib(const char *szKey, const char *szValue);
  bool RemoveAttrib(const char *szKey);
  bool ModifyAttrib(const char *szKey, const char *szValue);
  bool ClearAttrib();

  IUIElementProxy AddChild(const char *szNodeName);
  bool RemoveChild(IUIElement *);
  bool RemoveSelfInParent();

  bool AddChildBefore(IUIElement *pElem, IUIElement *pInsertBefore);
  bool AddChildAfter(IUIElement *pElem, IUIElement *pInsertAfter);
  IUIElementProxy AddChildBefore(const char *szNodeName,
                                 IUIElement *pInsertBefore);
  IUIElementProxy AddChildAfter(const char *szNodeName,
                                IUIElement *pInsertAfter);
  // bool  MoveChildAfterChild(IUIElement* pChild2Move, IUIElement*
  // pChildInsertAfter);
  bool MoveTo(IUIElement *pNewParent, IUIElement *pChildInsertAfter);

  IUIElementProxy FirstChild();
  IUIElementProxy NextElement();
  IUIElementProxy FindChild(const char *szChildName);

private:
  UIElement *m_pImpl;
};

struct UIDocument;
struct UIAPI IUIDocument {
public:
  IUIDocument(UIDocument *);

  long AddRef();
  long Release();

  void SetDirty(bool b);
  bool IsDirty();

  const char *GetSkinPath();
  IUIElementProxy FindElem(const char *szText);

private:
  UIDocument *m_pImpl;
};

} // namespace ui

#endif // _UI_IXMLWRAP_H_