#ifndef _UI_SDK_COMMON_H_
#define _UI_SDK_COMMON_H_

// 这个文件里放所有 Ixxx 都依赖的内容。

#include "sdk/include/common/ptr/unique_ptr.h"
#include "sdk/include/common/ptr/weak_ptr.h"
#include "sdk/include/common/uuid/uuid.h"

namespace ui {

// 注：long不是跨平台的，win32/win64/linux32下是4 bytes，但linux64(mac)下是8bytes
// 尽量不要使用long作为变量类型。long long固定是8 bytes，可以使用。
using byte = unsigned char;
using wchar = wchar_t;
using ushort = unsigned short;
using uint = unsigned int;
using llong = long long;
using ullong = unsigned long long;

// 导入导出定义
// linux默认所有函数都导出，添加编译参数 -fvisibility=hidden 控制默认不导出
#if defined(OS_WIN)
#ifdef UI_EXPORTS
#define UIAPI __declspec(dllexport)
#else
#define UIAPI __declspec(dllimport)
#endif
#else
#ifdef UI_EXPORTS
#define UIAPI __attribute__((visibility("default")))
#else
#define UIAPI
#endif
#endif

struct IApplication;
struct IMessage;
struct IObject;
struct IPanel;
struct IWindow;


// 定义快速
// struct IWindow;
// using IWindowPtr = std::unique_ptr<IWindow, void (*)(IWindow*)>;
// struct WindowPtr : public IWindowPtr {
//     WindowPtr(IResource *res) : IWindowPtr(IWindow::create(res)){ }
// };
#define UI_DEFINE_PTR(Xxx)                                                     \
  struct I##Xxx;                                                               \
  using I##Xxx##Ptr = std::unique_ptr<I##Xxx, void (*)(I##Xxx *)>;             \
  struct Xxx##Ptr : public I##Xxx##Ptr {                                       \
    Xxx##Ptr(IResource *res) : I##Xxx##Ptr(I##Xxx::create(res)) {}             \
  };

} // namespace ui
#endif