#pragma once
#include <map>
#include <string>
#include "include/interface/iuires.h"

namespace ui {
class ResourceBundle;
class I18nRes : public II18nCallback {
public:
  I18nRes(ResourceBundle *);
  ~I18nRes();

public:
  void Add(const char *szId, const char *szValue);

  const char *MapConfigValue(const char *configText);
  void Clear();

  II18nRes &GetII18nRes();

private:
  virtual const char *Map(const char *key) override;

private:
  ResourceBundle *m_resource_bundle;
  II18nRes *m_pII18nRes;

  std::map<std::string, std::string> m_dict;
};
} // namespace ui