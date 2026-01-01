#include "html/base/memory.h"
#include <assert.h>
#include <memory>

namespace {

int g_counter = 0;

class TestBase {
public:
  TestBase() {
    g_counter ++;
  }
  ~TestBase() {
    g_counter--;
  }

  void foo() { 
    int a = 0;
  }
};

class TestDeriver : public TestBase {

};

void test1() {
  g_counter = 0;
  {
    static TestDeriver s1;
    assert(g_counter == 1);
    auto o = html::memory::UniquePtr<TestBase>::from_managed(&s1);
    o->foo();
  }
  assert(g_counter == 1);


  g_counter = 0;
  {
    TestDeriver o1;
    assert(g_counter == 1);
    auto o = html::memory::UniquePtr<TestBase>::from_managed(&o1);
    o->foo();
  }
  assert(g_counter == 0);


  g_counter = 0;
  {
    TestDeriver* o1 = new TestDeriver();
    assert(g_counter == 1);
    auto o = html::memory::UniquePtr<TestBase>::take_new(o1);
    o->foo();
  }
  assert(g_counter == 0);

  g_counter = 0;
  {
    TestDeriver* o1 = new TestDeriver();
    assert(g_counter == 1);
    auto o = html::memory::UniquePtr<TestBase>::take_new(o1);
    assert(g_counter == 1);
    auto p = html::memory::UniquePtr<TestBase>::take_new(new TestDeriver());
    assert(g_counter == 2);
    assert(o1 == nullptr);
    o->foo();

    html::memory::UniquePtr<TestBase> q(std::move(p));
    assert(!p.get());
    html::memory::UniquePtr<TestBase> r(std::move(q));
    assert(!q.get());
    r->foo();
  }
  assert(g_counter == 0);

  g_counter = 0;
  {
    TestDeriver * p1 = new TestDeriver();
    assert(g_counter == 1);
    auto o = html::memory::UniquePtr<TestBase>::from_pool(p1);
    o->foo();
  }
  assert(g_counter == 1);


  // 指针转换
  g_counter = 0;
  {
    html::U<TestBase> o(html::U<TestDeriver>::take_new(new TestDeriver()));

    html::U<TestBase> p = html::U<TestDeriver>::take_new(new TestDeriver());
    o->foo();
  }
  assert(g_counter == 0);
}

}

void test_html_memory() {
  test1();
}
