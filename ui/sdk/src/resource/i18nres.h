#pragma once
#include <map>

#include "include/interface/iuires.h"

namespace ui {
class Resource;
class I18nRes : public II18nCallback {
public:
  I18nRes(Resource *);
  ~I18nRes();

public:
  void Add(const char *szId, const char *szValue);

  const char *MapConfigValue(const char *configText);
  void Clear();

  II18nRes &GetII18nRes();

private:
  virtual const char *Map(const char *key) override;

private:
  Resource *m_pSkinRes;
  II18nRes *m_pII18nRes;

  std::map<std::string, std::string> m_dict;
};
} // namespace ui