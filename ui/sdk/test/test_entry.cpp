#include "include/interface.h"
#include <iostream>

// 整个UISDK中的单元测试用例入口
// test_main.exe，将加载uisdk.dll中的UIUnitTest API

void signalslot_unittest();
void weakptr_unittest();
void rect_region_unittest();
void uuid_unittest();
void color_unittest();
void property_unittest();
void html_unittest();

namespace ui {

void _UIUnitTest() {
#if 0
  signalslot_unittest();
  weakptr_unittest();
  rect_region_unittest();
  uuid_unittest();
  color_unittest();
  property_unittest();
#endif
  html_unittest();
}

UIAPI void UIUnitTest() {
#if defined(UI_UNITTEST)
  std::cout << "ui unit test begin!" << std::endl;
  _UIUnitTest();
  std::cout << "ui unit test end!" << std::endl;
#endif
}

}