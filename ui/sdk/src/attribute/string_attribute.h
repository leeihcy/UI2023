#pragma once
#include "attributebase.h"
#include <functional>

namespace ui {

class StringAttribute : public AttributeBase {
public:
  StringAttribute();
  ~StringAttribute();
  IStringAttribute *GetIStringAttribute();

  virtual const char *Get() override;
  virtual void Set(const char *) override;
  virtual void Reset() override;
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *p,
                      EditorAttributeFlag e) override;
  virtual bool IsDefaultValue() override;

public:
  StringAttribute *SetDefault(const char *);
  const char *GetDefault();
  // void  Internationalization();

  void BindReference(std::string &);
  // void  BindFunction(void* _this, pfnStringSetter s, pfnStringGetter g);
  void Bind(slot<void(const char *)> &&s, slot<const char *()> &&g);
  // void  BindFunction(
  //     const std::function<void(const char*)>& s,
  //     const std::function<const char*()>& g);

protected:
  IStringAttribute *m_pIStringAttribute;
  std::string m_strDefault;
  std::string *m_pBindValue;

  // void*  _this;
  // pfnStringSetter  _setter;
  // pfnStringGetter  _getter;

  signal<void(const char *)> m_setter;
  signal<const char *()> m_getter;
};

class I18nStringAttribute : public StringAttribute {
public:
  virtual const char *Get() override;
  virtual void Set(const char *) override;

  void BindEx(slot<void(const char *, int)> &&s,
              slot<const char *()> &&g);
  // void  BindFunctionEx(void* _this, pfnStringExSetter s, pfnStringGetter g);

  // pfnStringExSetter  _setter_ex;
  signal<void(const char *, int)> m_setter_ex;
};

AttributeBase *CreateStringAttribute();
AttributeBase *CreateI18nStringAttribute();
} // namespace ui