
#ifndef _UI_SVG_INCLUDE_COMMON_H_
#define _UI_SVG_INCLUDE_COMMON_H_

// 所有CPP文件会都依赖该文件，定义所有最通用的内容

#if defined(OS_WIN)
    #ifdef UI_SVG_EXPORTS
    #define UISVGAPI __declspec(dllexport)
    #else
    #define UISVGAPI __declspec(dllimport)
    #endif
#else
    #ifdef UI_SVG_EXPORTS
    #define UISVGAPI __attribute__((visibility("default")))
    #else
    #define UISVGAPI  
    #endif
#endif



#endif