#include "sdk/include/util/color.h"
#include <assert.h>

namespace {
void test1() {
  {
    ui::Color c1 = ui::Color::Make(0x01020304);
    ui::Color c2 = ui::Color::MakeRGBA(0x02, 0x03, 0x04, 0x01);
    assert(c2.value == c1.value);
  }
  {
    ui::Color c1 = ui::Color::Make(0x12345678);
    ui::Color c2 = ui::Color::MakeRGBA(0x34, 0x56, 0x78, 0x12);
    ui::Color c3 = ui::Color::MakeARGB(0x12, 0x34, 0x56, 0x78);
    assert(c2.value == c1.value);
    assert(c3.a = 0x12);
    assert(c3.r = 0x34);
    assert(c3.g = 0x56);
    assert(c3.b = 0x78);
  }
}

}

void color_unittest() {
  test1();
}
