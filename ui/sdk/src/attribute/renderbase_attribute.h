#pragma once
#include "attributebase.h"

namespace ui {
class RenderBaseAttribute : public AttributeBase {
public:
  RenderBaseAttribute();
  ~RenderBaseAttribute();

  virtual const char *Get() override;
  virtual void Set(const char *) override;
  virtual void Reset() override;
  virtual bool IsDefaultValue() override;
  void SetBindValue(void *) override;

  virtual void Load(SerializeParam *) override;
  virtual void Save(SerializeParam *) override;
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *p,
                      EditorAttributeFlag e) override;

public:
  void SetObjectPtr(Object *p) { m_pObject = p; }
  void SetPrefix(const char *szPrefix) {
    if (szPrefix)
      m_strPrefix = szPrefix;
    else
      m_strPrefix.clear();
  }
  IRenderBaseAttribute *GetIRenderBaseAttribute();

private:
  void do_child_action(SerializeParam *);

private:
  IRenderBaseAttribute *m_pIRenderBaseAttribute;

  IRenderBase **m_ppBindValue;
  Object *m_pObject;

  std::string m_strPrefix;

  //     void*  _this;
  //     pfnStringSetter  _setter;
  //     pfnStringGetter  _getter;
};

AttributeBase *CreateRenderBaseAttribute();
} // namespace ui