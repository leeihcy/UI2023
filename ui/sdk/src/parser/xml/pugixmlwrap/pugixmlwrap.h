#pragma once
#include "../3rd/pugixml/pugixml.hpp"
#include "../xmlwrap.h"
#include "include/interface/ixmlwrap.h"
#include <functional>

namespace ui {

struct IAttributeMap;
struct IUIElement;

class PugiXmlElement : public UIElement {
public:
  PugiXmlElement(pugi::xml_node &node, UIDocument *pDoc);
  ~PugiXmlElement();

  virtual IUIElement *GetIUIElement() override;

public:
  virtual long AddRef() override;
  virtual long Release() override;

  virtual const char *GetTagName() override;
  virtual void SetTagName(const char *) override;
  virtual UIDocument *GetDocument() override;

  virtual const char *GetData() override;
  virtual bool SetData(const char *szText) override;

  virtual UIElementProxy AddChild(const char *szNodeName) override;
  virtual bool AddChild(UIElement *p) override;
  virtual bool AddChildBefore(UIElement *pElem,
                              UIElement *pInsertBefore) override;
  virtual bool AddChildAfter(UIElement *pElem,
                             UIElement *pInsertAfter) override;
  virtual UIElementProxy AddChildBefore(const char *szNodeName,
                                        UIElement *pInsertBefore) override;
  virtual UIElementProxy AddChildAfter(const char *szNodeName,
                                       UIElement *pInsertAfter) override;
  // virtual bool  MoveChildAfterChild(UIElement* pChild2Move, UIElement*
  // pChildInsertAfter) override;
  virtual bool MoveTo(UIElement *pNewParent,
                      UIElement *pChildInsertAfter) override;
  virtual bool RemoveChild(UIElement *) override;
  virtual bool RemoveSelfInParent() override;
  virtual UIElementProxy FirstChild() override;
  virtual UIElementProxy NextElement() override;
  virtual UIElementProxy FindChild(const char *szChildName) override;

  virtual void GetAttribList(IAttributeMap *ppMapAttrib) override;
  virtual void GetAttribList2(IAttributeList **ppListAttrib) override;
  virtual bool SetAttribList2(IAttributeList *attribute_list) override;
#if 0 // defined(OS_WIN)
  virtual bool GetAttrib(const char *szKey, BSTR *pbstrValue) override;
#endif
  virtual std::string GetAttrib(const char *szKey) override;

  virtual bool HasAttrib(const char *szKey) override;
  virtual bool AddAttrib(const char *szKey, const char *szValue) override;
  virtual bool SetAttrib(const char *szKey, const char *szValue) override;
  virtual bool RemoveAttrib(const char *szKey) override;
  virtual bool ModifyAttrib(const char *szKey, const char *szValue) override;
  virtual bool ClearAttrib() override;

  virtual bool GetAttribInt(const char *szKey, int *pInt) override;
  virtual bool AddAttribInt(const char *szKey, int nInt) override;

  void set_attr_by_prefix(IAttributeList *attribute_list, const char *szPrefix);
  void
  enum_attr(const std::function<void(const char *, const char *)> &callback);

private:
  long m_lRef;
  pugi::xml_node m_node;
  UIDocument *m_pDocument; // 没有引用计数

  IUIElement m_IUIElement;
};

class PugiXmlDocument : public UIDocument {
public:
  PugiXmlDocument();
  ~PugiXmlDocument();

  virtual long AddRef() override;
  virtual long Release() override;

  virtual IUIDocument *GetIUIDocument() override;

  virtual void SetSkinPath(const char *szPath) override;
  virtual const char *GetSkinPath() override;

  virtual bool LoadFile(const char *szFile) override;
  virtual bool LoadData(const byte *pData, unsigned int nDataSize) override;
  virtual UIElementProxy FindElem(const char *szText) override;
  virtual bool Save() override;
  virtual bool SaveAs(const char *szPath) override;
  virtual const char *GetPath() override;

  virtual void SetDirty(bool b) override;
  virtual bool IsDirty() override;

private:
  std::string m_strPath;     // 全路径
  std::string m_strSkinPath; // 在皮肤包中的路径

  long m_lRef;
  bool m_bDirty;

  pugi::xml_document m_doc;

  IUIDocument m_IUIDocument;
};

} // namespace ui