#include "property_store.h"
#include "property_id.h"
#include "include/interface/iproperty.h"

using namespace ui;

Property g_Property[UI_SDK_PROPERTY_COUNT];

class TestObject : public PropertyStoreDelegate {
public:
  TestObject() : m_property_store(g_Property, this) {
    RegisterProperties(); 
  }
  void RegisterProperties() {
    m_property_store.RegisterString(OBJECT_ID, "id", nullptr);
    m_property_store.RegisterString(OBJECT_CLASS, "class", nullptr);
    m_property_store.RegisterString(OBJECT_STYLE, "style", nullptr);

    m_property_store.RegisterInt(OBJECT_WIDTH, "width", 0);
    m_property_store.RegisterInt(OBJECT_WIDTH, "height", 0);

    m_property_store.RegisterString(OBJECT_FONT_NAME, "font-name", nullptr, PF_Inherit);
    m_property_store.RegisterInt(OBJECT_FONT_SIZE, "font-size", 10, PF_Inherit);
  }

  std::string GetId() { return m_property_store.GetString(OBJECT_ID); }
  void SetId(const char* id) { return m_property_store.SetString(OBJECT_ID, id); }

  int GetWidth() { return m_property_store.GetInt(OBJECT_WIDTH); }
  void SetWidth(int n) { m_property_store.SetInt(OBJECT_WIDTH, n); }

  int GetFontSize() { return m_property_store.GetInt(OBJECT_FONT_SIZE); }
  void SetFontSize(int n) { m_property_store.SetInt(OBJECT_FONT_SIZE, n); }

protected:
  PropertyStore* GetInheritStore() override {
    if (m_parent) {
      return &m_parent->m_property_store;
    }
    return nullptr;
  }
public:
  TestObject* m_parent = nullptr;
  PropertyStore m_property_store;
};
class TestControl : public TestObject {};


void test1_basic() {
  TestControl obj1;
  TestControl obj2;
  obj2.m_parent = &obj1;

  assert(obj1.GetWidth() == obj2.GetWidth());

  // 修改一个对象的属性，不影响其它对象
  obj1.SetWidth(100);
  assert(obj1.GetWidth() != obj2.GetWidth());

  // 继承属性验证
  assert(obj1.GetFontSize() == obj2.GetFontSize());
  obj1.SetFontSize(99);
  assert(obj1.GetFontSize() == obj2.GetFontSize());
  assert(99 == obj2.GetFontSize());

  // string属性验证
  assert(obj1.GetId() == "");
  obj1.SetId("obj1");
  assert(obj1.GetId() == "obj1");
}


// -------------------------------------------


// 第三方只能调用wrap
enum ThirdPartyPropertyId {
  MODULE_PROPERTY_FOO = 0,
  MODULE_PROPERTY_COUNT,
};
void* s_property_register = nullptr;

class Test3rdModuleObject {
public:
  Test3rdModuleObject() : m_property_store(s_property_register) {
    RegisterProperties();
  }
  void RegisterProperties() {
    m_property_store.RegisterString(0, "foo", "bar");
  }
  std::string GetFoo() { return m_property_store.GetString(0); }
  void SetFoo(const char* id) { return m_property_store.SetString(0, id); }

public:
  PropertyStoreWrap m_property_store;
};
// uisdk之外的模块属性功能验证
void test2_3rd_module() {
  s_property_register = PropertyStoreWrap::AllocRegister(MODULE_PROPERTY_COUNT);
  {
    TestObject obj1;
    Test3rdModuleObject _3rd_module;

    // 都是0号索引，但value是不同的。
    obj1.SetId("myid");

    assert(obj1.GetId() == "myid");
    assert(_3rd_module.GetFoo() == "bar");
  }
  PropertyStoreWrap::FreeRegister(s_property_register);
}


// -------------------------------------------


void property_unittest() {
  test1_basic();  
  test2_3rd_module();
}