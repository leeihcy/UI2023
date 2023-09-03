#pragma once
// UISDK 内部接口。对外使用IUIElement/IUIDocument
#include <string>

namespace ui {

struct UIDocument;
struct IUIDocument;
struct IUIElement;
struct UIElement;
struct IMapAttribute;
struct IListAttribute;

// 模拟 share_ptr 实现UIElement生命周期管理。
// 调用者只需要直接拿到UIElementProxy返回值进行调用即可，不用负责销毁

class UIElementProxy {
public:
  UIElementProxy();
  UIElementProxy(UIElement *pNew);
  ~UIElementProxy();

  UIElementProxy(const UIElementProxy &o);
  UIElementProxy &operator=(const UIElementProxy &o);

  UIElement *operator->();
  operator bool() const;
  UIElement *get();
  UIElement *detach();

private:
  void release();

private:
  UIElement *m_ptr;
};

struct UIElement {
  virtual ~UIElement(){};

  virtual long AddRef() = 0;
  virtual long Release() = 0;

  virtual IUIElement *GetIUIElement() = 0;

  virtual const char *GetTagName() = 0;
  virtual void SetTagName(const char *) = 0;
  virtual UIDocument *GetDocument() = 0;

  virtual const char *GetData() = 0;
  virtual bool SetData(const char *szText) = 0;

  virtual UIElementProxy AddChild(const char *szNodeName) = 0;
  virtual bool AddChild(UIElement *p) = 0;
  virtual bool AddChildBefore(UIElement *pElem, UIElement *pInsertBefore) = 0;
  virtual bool AddChildAfter(UIElement *pElem, UIElement *pInsertAfter) = 0;
  virtual UIElementProxy AddChildBefore(const char *szNodeName,
                                        UIElement *pInsertBefore) = 0;
  virtual UIElementProxy AddChildAfter(const char *szNodeName,
                                       UIElement *pInsertAfter) = 0;
  virtual bool MoveTo(UIElement *pNewParent, UIElement *pChildInsertAfter) = 0;
  virtual bool RemoveChild(UIElement *) = 0;
  virtual bool RemoveSelfInParent() = 0;
  virtual UIElementProxy FirstChild() = 0;
  virtual UIElementProxy NextElement() = 0;
  virtual UIElementProxy FindChild(const char *szChildName) = 0;

  virtual void GetAttribList(IMapAttribute *) = 0;
  virtual void GetAttribList2(IListAttribute **ppListAttrib) = 0;
  virtual bool SetAttribList2(IListAttribute *pListAttrib) = 0;
#if 0 // defined(OS_WIN)
  virtual bool GetAttrib(const char *szKey, BSTR *pbstrValue) = 0;
#endif
  virtual std::string GetAttrib(const char *szKey) = 0;

  virtual bool HasAttrib(const char *szKey) = 0;
  virtual bool AddAttrib(const char *szKey, const char *szValue) = 0;
  virtual bool SetAttrib(const char *szKey, const char *szValue) = 0;
  virtual bool RemoveAttrib(const char *szKey) = 0;
  virtual bool ModifyAttrib(const char *szKey, const char *szValue) = 0;
  virtual bool ClearAttrib() = 0;

  virtual bool GetAttribInt(const char *szKey, int *pInt) = 0;
  virtual bool AddAttribInt(const char *szKey, int nInt) = 0;
};

struct UIDocument {
  virtual long AddRef() = 0;
  virtual long Release() = 0;

  virtual IUIDocument *GetIUIDocument() = 0;

  virtual void SetSkinPath(const char *szPath) = 0;
  virtual const char *GetSkinPath() = 0;

  virtual bool LoadFile(const char *szFile) = 0;
  virtual bool LoadData(const unsigned char *pData, int nDataSize) = 0;
  virtual UIElementProxy FindElem(const char *szText) = 0;
  virtual bool Save() = 0;
  virtual bool SaveAs(const char *szPath) = 0;
  virtual const char *GetPath() = 0;

  virtual void SetDirty(bool b) = 0;
  virtual bool IsDirty() = 0;
};

enum XML_ENGINE {
  XML_ENGINE_DEFAULT,
  // MSXML, 放弃。 1. com方式调用太费劲。2.
  // 有些用户电脑上注册表一出问题，就会创建失败
  PUGIXML,
};
bool CreateXmlDocument(XML_ENGINE e, UIDocument **);

} // namespace ui