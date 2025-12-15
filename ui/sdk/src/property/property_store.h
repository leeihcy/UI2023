#ifndef _UI_SDK_SRC_PROPERTY_PROPERTYSTORE_H_
#define _UI_SDK_SRC_PROPERTY_PROPERTYSTORE_H_

#include "include/interface/iattributemap.h"
#include "property.h"

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <assert.h>

namespace ui {
  
class PropertyStore;

// 外部需要提供给PropertyStore的数据
class PropertyStoreDelegate {
public:
  // 外部返回当前对象的父结点的属性库，用于实现属性继承。
  virtual PropertyStore* GetInheritPropertyStore() = 0;
};


// 所有属性的默认值数据库，一个模块中的所有对象共享一份。
class DefaultPropertyStore {
public:
  // 每个模块需要自己分配所有属性数组的内存，作为这里的初始化参数。
  DefaultPropertyStore(Property* property_register_buffer);

  PropertyValue *GetDefaultValue(int id) const { return m_properties[id].value; }
  Property& GetDefaultData(int id) const { return m_properties[id]; }

  IProperty& RegisterInt(int id, const std::string& key, int default_value = 0);
  IProperty& RegisterBool(int id, const std::string& key, bool default_value);
  IProperty& RegisterString(int id, const std::string &key,
                      const char *default_value = nullptr);
  IProperty& RegisterRect(int id, const std::string& key);

  static int MapKeyToId(const std::string& key);
private:
  template <class T>
  inline IProperty &Register(int id, const std::string &key, T *value) {
    return Register2(id, key, T::Type(), value);
  }
  IProperty &Register2(int id, const std::string &key, PropertyValueType type,
                       PropertyValue *value);

private:
  // 外部传递进来的属性数组内存。
  // 1. 由外部保证内存的生命周期。
  // 2. 数组的大小一定会大于所有属性ID。
  // 3. 直接使用属性ID作为数组索引获取数据。
  Property* m_properties;
};


// 每个对象单独配置的属性值数据库。
class SpecifiedPropertyStore {
public:
  ~SpecifiedPropertyStore();
  PropertyValue *GetConfigValue(int id) const;

  void SetInt(int id, int n);
  void SetBool(int id, bool b);
  void SetString(int id, const char* text);
  void SetRect(int id, const Rect& rect);

  void SetValue(int id, PropertyValue* value);

private:
  void setValue(int id, PropertyValue* value);

private:
  // TODO: 这里的每个value暂时规定得是malloc出来的，不使用static分配
  std::map<int, PropertyValue*> m_map;
};


// 每个对象从祖父结点中继承得到的属性数据库
class InheritPropertyStore {
public:
  PropertyValue *GetInheritValue(PropertyStoreDelegate* delegate, int id) const;
};


// 对外对象。
// 为了方便外部直接调用 Register/Get/Set 各种方法，这里使用多重继承的方式。
class PropertyStore:
  public SpecifiedPropertyStore,
  public InheritPropertyStore,
  public DefaultPropertyStore
{
public:
  PropertyStore(Property* default_buffer, PropertyStoreDelegate *delegate);

  int GetInt(int id) const;
  bool GetBool(int id) const;
  const std::string& GetString(int id) const;
  const Rect& GetRect(int id) const;

  void Serialize(IAttributeMap* attr_map);

protected:
  PropertyValue *GetValue(int id) const;

private:
  PropertyStoreDelegate* m_delegate = nullptr;
  friend class InheritPropertyStore;
};

}; // namespace ui

#endif