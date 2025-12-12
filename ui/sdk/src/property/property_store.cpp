#include "src/property/property_store.h"

namespace ui {

DefaultPropertyStore::DefaultPropertyStore(Property *property_register_buffer)
    : m_properties(property_register_buffer) {}

bool DefaultPropertyStore::Register(int id, std::string key,
                                    PropertyValue *value, int flags) {
  if (!value) {
    return false;
  }

  Property &detail = m_properties[id];
  detail.id = id;
  detail.key = key;
  detail.value = value;
  detail.flags = flags;
  return true;
}

bool DefaultPropertyStore::RegisterInt(int id, const std::string &key,
                                       int default_value, int flags) {
  if (default_value == 0) {
    return Register(id, key, IntValue::s_0(), flags);
  } else if (default_value == -1) {
    return Register(id, key, IntValue::s_minus1(), flags);
  } else if (default_value == 1) {
    return Register(id, key, IntValue::s_1(), flags);
  }
  return Register(id, key, mallocValue<IntValue>(default_value),
                  flags | PF_MallocValue);
}

bool DefaultPropertyStore::RegisterBool(int id, const std::string &key,
                                        bool default_value, int flags) {
  if (default_value) {
    return Register(id, key, BoolValue::s_true(), flags);
  } else {
    return Register(id, key, BoolValue::s_false(), flags);
  }
}

bool DefaultPropertyStore::RegisterString(int id, const std::string &key,
                                          const char *default_value,
                                          int flags) {
  if (default_value) {
    return Register(id, key, mallocValue<StringValue>(default_value),
                    flags | PF_MallocValue);
  } else {
    return Register(id, key, StringValue::s_empty(), flags);
  }
}

// --------------------------------------------------------------------------------

ConfigPropertyStore::~ConfigPropertyStore() {
  auto iter = m_map.begin();
  for (; iter != m_map.end(); ++iter) {
    free(iter->second);
  }
}

PropertyValue *ConfigPropertyStore::GetConfigValue(int id) const {
  auto iter = m_map.find(id);
  if (iter == m_map.end()) {
    return nullptr;
  }
  return (*iter).second;
}
void ConfigPropertyStore::SetValue(int id, PropertyValue *value) {
  m_map[id] = value;
}

void ConfigPropertyStore::SetInt(int id, int n) {
  PropertyValue *cur = GetConfigValue(id);
  if (!cur) {
    SetValue(id, mallocValue<IntValue>(n));
  } else {
    static_cast<IntValue *>(cur)->value = n;
  }
}

void ConfigPropertyStore::SetBool(int id, bool b) {
  PropertyValue *cur = GetConfigValue(id);
  if (!cur) {
    SetValue(id, mallocValue<BoolValue>(b));
  } else {
    static_cast<BoolValue *>(cur)->value = b;
  }
}

void ConfigPropertyStore::SetString(int id, const char *text) {
  PropertyValue *cur = GetConfigValue(id);
  if (!cur) {
    SetValue(id, mallocValue<StringValue>(text));
  } else {
    static_cast<StringValue *>(cur)->Set(text);
  }
}

// --------------------------------------------------------------------------------

PropertyValue *
InheritPropertyStore::GetInheritValue(PropertyStoreDelegate *delegate,
                                      int id) const {
  if (!delegate) {
    return nullptr;
  }
  PropertyStore *store = delegate->GetInheritStore();
  if (!store) {
    return nullptr;
  }
  return store->GetValue(id);
}

// --------------------------------------------------------------------------------

PropertyStore::PropertyStore(Property *default_buffer,
                             PropertyStoreDelegate *delegate)
    : DefaultPropertyStore(default_buffer), m_delegate(delegate) {}

PropertyValue *PropertyStore::GetValue(int id) const {
  // 优先级： Config > Inherit > Default

  PropertyValue *value = GetConfigValue(id);
  if (!value) {
    Property &default_data = GetDefaultData(id);

    if (default_data.flags & PF_Inherit) {
      value = GetInheritValue(m_delegate, id);
    }

    if (!value) {
      value = default_data.value;
    }
  }
  return value;
}

int PropertyStore::GetInt(int id) const {
  return static_cast<IntValue *>(GetValue(id))->value;
}
bool PropertyStore::GetBool(int id) const {
  return static_cast<BoolValue *>(GetValue(id))->value;
}
std::string PropertyStore::GetString(int id) const {
  return static_cast<StringValue *>(GetValue(id))->value;
}

} // namespace ui