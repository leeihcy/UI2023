#ifndef _UI_GPU_INCLUDE_INTERFACE_H_
#define _UI_GPU_INCLUDE_INTERFACE_H_


// 导入导出定义
// linux默认所有函数都导出，添加编译参数 -fvisibility=hidden 控制默认不导出
#if defined(OS_WIN)
#ifdef UI_EXPORTS
#define UIGPUAPI __declspec(dllexport)
#else
#define UIGPUAPI __declspec(dllimport)
#endif
#else
#ifdef UI_EXPORTS
#define UIGPUAPI __attribute__((visibility("default")))
#else
#define UIGPUAPI
#endif
#endif


#endif