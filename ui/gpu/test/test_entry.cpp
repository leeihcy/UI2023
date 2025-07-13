#if defined(UI_UNITTEST)
#include "gpu/include/api.h"
#include <iostream>

// 整个UISDK中的单元测试用例入口
// test_main.exe，将加载uisdk.dll中的UIUnitTest API

//
// https://vulkan-tutorial.com/en/Development_environment
//

void tutorial_01();
void tutorial_02();

void _GpuUnitTest() {
#if defined(OS_MAC)
  tutorial_02();
#endif
}

namespace ui {
extern "C" UIGPUAPI void GpuUnitTest() {
  std::cout << "gpu unit test begin!" << std::endl;
  _GpuUnitTest();
  std::cout << "gpu unit test end!" << std::endl;
}
}
#endif