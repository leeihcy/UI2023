#include "src/metal2/metal2_app.h"

namespace ui {

Metal2Application &Metal2Application::GetInstance() {
  static Metal2Application s_app;
  return s_app;
}

bool Metal2Application::Startup() {
  return true;
}

//
// Metal Document:
// selecting-device-objects-for-graphics-rendering
//
// MacOS每个屏幕会对应一张显卡。App在哪个屏幕上，我们就应该选取哪张显卡。
// 如果选错了，将浪费资源，在显卡之间传递数据。
// 
// MacBookPro的集成显卡，在某些条件下会成为内置屏幕(built-in display)的显卡，例如
// 发热状态、电池状态或应用程序的需要。
//
// 如何查看获取app在使用哪张显卡？
// 通过view.window.screen获取对应设备属性。
//
bool Metal2Application::createDevice() {
  // On macOS device that have multiple GPUs, such as MacBook Pro, 
  // the system default is the discrete GPU. （独显）
  // 调用这个函数，可能会导致系统强制将当前显卡提升为独显
  // m_device = MTLCreateSystemDefaultDevice();

  // 不能在app中创建设备，而是由每个view/compositor对应它所在的设备。
  return true;
}

void Metal2Application::Shutdown() {
  // if (m_device) {
  //   [m_device release];
  //   m_device = nil;
  // }
}

} // namespace ui