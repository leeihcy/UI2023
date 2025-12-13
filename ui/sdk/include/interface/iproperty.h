#ifndef _UI_IPROPERTY_H_
#define _UI_IPROPERTY_H_

#include "sdk/include/interface.h"
#include <string>

namespace ui {

struct IProperty {
  // flags相关操作
  virtual IProperty& AsData() = 0;
  virtual IProperty& Inheritable() = 0;
  virtual IProperty& ToFree() = 0;

  virtual bool IsData() = 0;
  virtual bool CanInherit() = 0;
  virtual bool NeedsFree() = 0;

  // virtual 
};

class PropertyStore;
struct UIAPI PropertyStoreWrap {
  PropertyStoreWrap(void* property_register_buffer);
  ~PropertyStoreWrap();

  static void* AllocRegister(int property_count);
  static void FreeRegister(void* register_buffer);

  // Register
  IProperty& RegisterInt(int id, const std::string& key, int default_value = 0);
  IProperty& RegisterBool(int id, const std::string& key, bool default_value);
  IProperty& RegisterString(int id, const std::string &key,
                      const char *default_value = nullptr);
  // Get
  int GetInt(int id) const;
  bool GetBool(int id) const;
  std::string GetString(int id) const;

  // Set
  void SetInt(int id, int n);
  void SetBool(int id, bool b);
  void SetString(int id, const char* text);


private:
  PropertyStore *m_pImpl;
};



}

#endif