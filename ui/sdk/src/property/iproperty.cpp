#include "include/interface/iproperty.h"
#include "src/property/property_store.h"

namespace ui {

PropertyStoreWrap::PropertyStoreWrap(void* register_buffer) {
  // 第三方模块，看起来不需要支持inherit属性，没想好delegate要如何跨模块，先不支持。
  m_pImpl = new PropertyStore((Property*)register_buffer, nullptr);
}
PropertyStoreWrap::~PropertyStoreWrap() {
  delete m_pImpl;
}

void* PropertyStoreWrap::AllocRegister(int property_count) {
  return (void*)new Property[property_count];
}
void PropertyStoreWrap::FreeRegister(void* register_buffer) {
  delete[] (Property*)register_buffer;
}

IProperty& PropertyStoreWrap::RegisterInt(int id, const std::string& key, int default_value) {
  return m_pImpl->RegisterInt(id, key, default_value);
}
IProperty& PropertyStoreWrap::RegisterBool(int id, const std::string& key, bool default_value) {
  return m_pImpl->RegisterBool(id, key, default_value);
}
IProperty& PropertyStoreWrap::RegisterString(int id, const std::string &key,
                    const char *default_value) {
  return m_pImpl->RegisterString(id, key, default_value);
}

int PropertyStoreWrap::GetInt(int id) const {
  return m_pImpl->GetInt(id);
}
bool PropertyStoreWrap::GetBool(int id) const {
  return m_pImpl->GetBool(id);
}
std::string PropertyStoreWrap::GetString(int id) const {
  return m_pImpl->GetString(id);
}

void PropertyStoreWrap::SetInt(int id, int n) {
  m_pImpl->SetInt(id, n);
}
void PropertyStoreWrap::SetBool(int id, bool b) {
  m_pImpl->SetBool(id, b);
}
void PropertyStoreWrap::SetString(int id, const char* text) {
  m_pImpl->SetString(id, text);
}

}