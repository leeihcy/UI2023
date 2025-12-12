#ifndef _UI_IPROPERTY_H_
#define _UI_IPROPERTY_H_

#include "sdk/include/interface.h"
#include <string>

namespace ui {

class PropertyStore;
struct UIAPI PropertyStoreWrap {
  PropertyStoreWrap(void* property_register_buffer);
  ~PropertyStoreWrap();

  static void* AllocRegister(int property_count);
  static void FreeRegister(void* register_buffer);

  // Register
  bool RegisterInt(int id, const std::string& key, int default_value = 0, int flags = 0);
  bool RegisterBool(int id, const std::string& key, bool default_value, int flags = 0);
  bool RegisterString(int id, const std::string &key,
                      const char *default_value = nullptr, int flags = 0);
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