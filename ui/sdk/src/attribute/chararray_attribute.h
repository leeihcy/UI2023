#pragma once
#include "attributebase.h"

namespace ui {

class CharArrayAttribute : public AttributeBase {
public:
  CharArrayAttribute();
  ~CharArrayAttribute();
  ICharArrayAttribute *GetICharArrayAttribute();

  virtual const char *Get() override;
  virtual void Set(const char *) override;
  virtual void Reset() override;
  virtual bool IsDefaultValue() override;
  void SetBindValue(void *);
  void SetBindFuction(void *_this, void *_setter, void *_getter);
  virtual void Editor(SerializeParam *pData, AttributeEditorProxy *,
                      EditorAttributeFlag) override;
  void SetBindValueSize(long lSize);

public:
  CharArrayAttribute *SetDefault(const char *);
  const char *GetDefault();

protected:
  ICharArrayAttribute *m_pICharArrayAttribute;

  char *m_pBindValue;
  long m_lBindValueSize;

  std::string m_strDefault;
  bool m_bDefaultIsNULL; // 默认是nullptr，而不是""

  void *_this;
  pfnStringSetter _setter;
  pfnStringGetter _getter;
};

AttributeBase *CreateCharArrayAttribute();

} // namespace ui