#ifndef _UI_NET_INCLUDE_NETAPI_H_
#define _UI_NET_INCLUDE_NETAPI_H_

// 导入导出定义
// linux默认所有函数都导出，添加编译参数 -fvisibility=hidden 控制默认不导出
#if defined(OS_WIN)
#ifdef NET_EXPORTS
#define NETAPI __declspec(dllexport)
#else
#define NETAPI __declspec(dllimport)
#endif
#else
#ifdef NET_EXPORTS
#define NETAPI __attribute__((visibility("default")))
#else
#define NETAPI
#endif
#endif

namespace net {
  
}

#endif