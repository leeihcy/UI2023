#ifndef _UI_API_H_
#define _UI_API_H_
#include "sdk/include/interface.h"

#include "sdk/include/macro/xmldefine.h"
#include "sdk/include/macro/uidefine.h"

namespace ui {

// 编译时版本定义
#define UISDK_VERSION_MAJOR 2 // 主版本
#define UISDK_VERSION_MINOR 0 // 次版本
#define UISDK_VERSION_PATCH 1 // 补丁版本

struct UIAPI SDKVersion {
  static int GetMajor();
  static int GetMinor();
  static int GetPatch();
  static int GetVersionText(char *szText, int nTextSize);
};

struct IUIElement;
struct IResource;
struct IObject;
struct ILayout;
struct IApplication;

// UI对象创建函数
// typedef  int(*pfnUICreateRenderBasePtr)(IResource* pSkinRes, void** ppOut);
// typedef  int(*pfnUICreateTextRenderBasePtr)(IResource* pSkinRes, void**
// ppOut);
typedef int (*pfnParseSkinTag)(IUIElement *, IResource *pSkinRes);
// typedef  int(*pfnUICreateLayoutPtr)(IObject* pObject, ILayout**  ppLayout);

// UI对象创建函数
typedef void (*pfnUICreateRenderBasePtr)(IResource *pSkinRes, void **ppOut);
typedef void (*pfnUICreateTextRenderBasePtr)(IResource *pSkinRes,
                                             void **ppOut);
typedef int (*pfnParseSkinTag)(IUIElement *, IResource *pSkinRes);

// uiapplication中的枚举回调
typedef bool (*pfnEnumLayoutTypeCallback)(const wchar_t *, llong, llong);
typedef void (*pfnEnumRenderBaseNameCallback)(const wchar_t *, llong, llong);
typedef void (*pfnEnumTextRenderBaseNameCallback)(const wchar_t *, llong, llong);

// 返回值:
enum PARSE_CONTROL_RETURN {
  ParseControl_Failed,       // 解析错误
  ParseControl_CreateObject, // 解析，并创建了该对象，但没有加载属性
  ParseControl_LoadObject, // 创建对象并加载其属性，但处没处理子结点
  ParseControl_LoadDescendants, // 加载自己和所有子结点的属性
};
typedef PARSE_CONTROL_RETURN (*pfnParseControlTag)(IUIElement *, IResource *,
                                                   IObject *pObjParent,
                                                   IObject **);

// 外部UI对象注册入口，由外部DLL实现该导出函数
#define FUN_RegisterUIObject_NAME "RegisterUIObject"
typedef int (*funRegisterUIObjectPtr)(IApplication *p);

struct UIMSG;
struct IMessage;

struct IMapAttribute;
struct IListAttribute;
UIAPI IMapAttribute* UICreateIMapAttribute();
UIAPI int UICreateIListAttribute(IListAttribute **ppOut);

struct IRenderBitmap;
UIAPI void
UICreateRenderBitmap(IApplication *pUIApp,
                     GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType,
                     IMAGE_ITEM_TYPE eType, IRenderBitmap **ppOut);

struct IRenderTarget;
UIAPI IRenderTarget *UICreateRenderTarget(IApplication *pUIApp,
                                          GRAPHICS_RENDER_LIBRARY_TYPE eType,
                                          bool bNeedAlphaChannel);

UIAPI int GetDpi();
UIAPI int ScaleByDpi(int x);
#define dpi(x) ui::ScaleByDpi(x)

UIAPI void UIUnitTest();
} // namespace ui
#endif