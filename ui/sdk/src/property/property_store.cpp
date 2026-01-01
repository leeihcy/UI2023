#include "src/property/property_store.h"
#include "include/util/log.h"
#include "src/property/property.h"

namespace ui {

static std::map<std::string, int> g_key_id_map;

DefaultPropertyStore::DefaultPropertyStore(Property *property_register_buffer)
    : m_properties(property_register_buffer) {}


int DefaultPropertyStore::MapKeyToId(const std::string& key) {
  auto iter = g_key_id_map.find(key);
  if (iter == g_key_id_map.end()) {
    UI_LOG_WARN("[Property] MapKeyToId failed, key is not registered: %s", key.c_str());
    return -1;
  }
  return iter->second;
}

void DefaultPropertyStore::onRegister(int id, const std::string &key) {
  if (!key.empty()) {
    g_key_id_map[key] = id;
  }
}

IProperty& DefaultPropertyStore::RegisterInt(int id, const std::string &key,
                                       int default_value) {
  return Register(id, key, IntValue::Create(default_value));
}

IProperty& DefaultPropertyStore::RegisterBool(int id, const std::string &key,
                                        bool default_value) {
  return Register(id, key, BoolValue::Create(default_value));
}

IProperty& DefaultPropertyStore::RegisterString(int id, const std::string &key,
                                          const char *default_value) {
  return Register(id, key, StringValue::Create(default_value));
}

IProperty& DefaultPropertyStore::RegisterRect(int id, const std::string& key) {
  return Register(id, key, RectValue::s_empty()); 
}

// --------------------------------------------------------------------------------

SpecifiedPropertyStore::~SpecifiedPropertyStore() {
  auto iter = m_map.begin();
  for (; iter != m_map.end(); ++iter) {
    free(iter->second);
  }
}

PropertyValue *SpecifiedPropertyStore::GetConfigValue(int id) const {
  auto iter = m_map.find(id);
  if (iter == m_map.end()) {
    return nullptr;
  }
  return (*iter).second;
}

void SpecifiedPropertyStore::SetValue(int id, PropertyValue* value) {
  if (!value) {
    return;
  }

  auto iter = m_map.find(id);
  if (iter == m_map.end()) {
    m_map[id] = value;
    return;
  }
  free((*iter).second);
  iter->second = value;
}

void SpecifiedPropertyStore::setValue(int id, PropertyValue *value) {
  m_map[id] = value;
}

void SpecifiedPropertyStore::SetInt(int id, int n) {
  PropertyValue *cur = GetConfigValue(id);
  if (!cur || !cur->IsAlloc()) {
    setValue(id, IntValue::Create(n));
    return;
  } else {
    static_cast<IntValue *>(cur)->value = n;
  }
}

void SpecifiedPropertyStore::SetBool(int id, bool b) {
  PropertyValue *cur = GetConfigValue(id);
  if (!cur || !cur->IsAlloc()) {
    setValue(id, BoolValue::Create(b));
  } else {
    static_cast<BoolValue *>(cur)->value = b;
  }
}

void SpecifiedPropertyStore::SetString(int id, const char *text) {
  PropertyValue *cur = GetConfigValue(id);
  if (!cur || !cur->IsAlloc()) {
    setValue(id, StringValue::Create(text));
  } else {
    static_cast<StringValue *>(cur)->Set(text);
  }
}

void SpecifiedPropertyStore::SetRect(int id, const Rect& rect) {
  PropertyValue *cur = GetConfigValue(id);
  if (!cur || !cur->IsAlloc()) {
    setValue(id, RectValue::Create(rect));
  } else {
    static_cast<RectValue *>(cur)->value = rect;
  }
}

// --------------------------------------------------------------------------------

PropertyValue *
InheritPropertyStore::GetInheritValue(PropertyStoreDelegate *delegate,
                                      int id) const {
  if (!delegate) {
    return nullptr;
  }
  PropertyStore *store = delegate->GetInheritPropertyStore();
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

    if (default_data.CanInherit()) {
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
const std::string& PropertyStore::GetString(int id) const {
  return static_cast<StringValue *>(GetValue(id))->value;
}
const Rect& PropertyStore::GetRect(int id) const {
  return static_cast<RectValue *>(GetValue(id))->value;
}

void PropertyStore::Serialize(IAttributeMap* attr_map) {
  // 需要将 attribute key --> property id
  attr_map->BeginEnum();

  const char* key = nullptr;
  const char* value = nullptr;
  while (attr_map->EnumNext(&key, &value)) {
    if (!value) {
      return;
    }

    int id = MapKeyToId(key);
    if (id < 0) {
      continue;
    }
    Property &default_data = GetDefaultData(id);
    SpecifiedPropertyStore::SetValue(
        id, PropertyValue::Parse(default_data.Type(), value));
  }
  attr_map->EndEnum();
}

} // namespace ui