#ifndef _UI_SDK_COMMON_H_
#define _UI_SDK_COMMON_H_

// 这个文件里放所有 Ixxx 都依赖的内容。

#include "sdk/include/common/ptr/unique_ptr.h"
#include "sdk/include/common/ptr/weak_ptr.h"
#include "sdk/include/common/uuid/uuid.h"
#include "sdk/include/uicreator.h"

namespace ui {

using byte = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;
using wchar = wchar_t;

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


} // namespace ui
#endif