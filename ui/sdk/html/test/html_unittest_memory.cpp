#include "html/base/memory.h"
#include "html/base/atomic_string.h"
#include <assert.h>
#include <memory>

using html::A;

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
    auto o = html::memory::AutoPtr<TestBase>::from_managed(&s1);
    o->foo();
  }
  assert(g_counter == 1);


  g_counter = 0;
  {
    TestDeriver o1;
    assert(g_counter == 1);
    auto o = html::memory::AutoPtr<TestBase>::from_managed(&o1);
    o->foo();
  }
  assert(g_counter == 0);


  g_counter = 0;
  {
    TestDeriver* o1 = new TestDeriver();
    assert(g_counter == 1);
    auto o = html::memory::AutoPtr<TestBase>::take_new(o1);
    o->foo();
  }
  assert(g_counter == 0);

  g_counter = 0;
  {
    TestDeriver* o1 = new TestDeriver();
    assert(g_counter == 1);
    auto o = html::memory::AutoPtr<TestBase>::take_new(o1);
    assert(g_counter == 1);
    auto p = html::memory::AutoPtr<TestBase>::take_new(new TestDeriver());
    assert(g_counter == 2);
    assert(o1 == nullptr);
    o->foo();

    html::memory::AutoPtr<TestBase> q(std::move(p));
    assert(!p.get());
    html::memory::AutoPtr<TestBase> r(std::move(q));
    assert(!q.get());
    r->foo();
  }
  assert(g_counter == 0);

  g_counter = 0;
  {
    TestDeriver * p1 = new TestDeriver();
    assert(g_counter == 1);
    auto o = html::memory::AutoPtr<TestBase>::from_pool(p1);
    o->foo();
  }
  assert(g_counter == 1);


  // 指针转换
  g_counter = 0;
  {
    A<TestBase> o(A<TestDeriver>::take_new(new TestDeriver()));

    A<TestBase> p = A<TestDeriver>::take_new(new TestDeriver());
    o->foo();
  }
  assert(g_counter == 0);
}

void test2_share() {
  g_counter = 0;
  {
    A<TestBase> o1 = A<TestBase>::take_new(new TestDeriver());
    assert(g_counter == 1);

    A<TestBase> o2 = nullptr;
    o2.share(o1);
    assert(g_counter == 1);

    o1.destroy();
    assert(g_counter == 1);

    o2.destroy();
    assert(g_counter == 0);
  }

  g_counter = 0;
  {
    {
      TestDeriver s1;
      A<TestBase> o1 = A<TestBase>::from_managed(&s1);
      assert(g_counter == 1);

      A<TestBase> o2 = nullptr;
      o2.share(o1);
      assert(g_counter == 1);

      o1.destroy();
      assert(g_counter == 1);

      o2.destroy();
      assert(g_counter == 1);
    }
    assert(g_counter == 0);
  }
}

void test3_atomic_string() {
  {
    html::AtomicString as1;
    html::AtomicString as2;
    html::AtomicString as3(u"");
    html::AtomicString as4(u"*");
    assert(as1 == as2);
    assert(as2 != as3);
    assert(as3 == u"");
    assert(as3 != as4);

    assert(as1 == html::g_null_atom);
    assert(as2 == html::g_null_atom);
    assert(as3 == html::g_empty_atom);
    assert(as4 == html::g_star_atom);
  }
  {
     html::AtomicString as1 = u"div";
     html::AtomicString as2 = u"div";
     assert(as1 == as2);
  }
}

}

void test_html_memory() {
  // test1();
  // test2_share();
  test3_atomic_string();
}
