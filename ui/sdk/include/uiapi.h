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
struct IResourceBundle;
struct IObject;
struct ILayout;
struct IApplication;

// UI对象创建函数
typedef void (*pfnUICreateRenderBasePtr)(IResourceBundle *resource_bundle, void **ppOut);
typedef void (*pfnUICreateTextRenderPtr)(IResourceBundle *resource_bundle,
                                             void **ppOut);
typedef int (*pfnParseResourceNode)(IUIElement *, IResourceBundle *resource_bundle);

// uiapplication中的枚举回调
typedef bool (*pfnEnumLayoutTypeCallback)(const char *, llong, llong);
typedef void (*pfnEnumRenderBaseNameCallback)(const char *, llong, llong);
typedef void (*pfnEnumTextRenderNameCallback)(const char *, llong, llong);

// 返回值:
enum PARSE_CONTROL_RETURN {
  ParseControl_Failed,       // 解析错误
  ParseControl_CreateObject, // 解析，并创建了该对象，但没有加载属性
  ParseControl_LoadObject, // 创建对象并加载其属性，但处没处理子结点
  ParseControl_LoadDescendants, // 加载自己和所有子结点的属性
};
typedef PARSE_CONTROL_RETURN (*pfnParseControlNode)(IUIElement *, IResourceBundle *,
                                                   IObject *pObjParent,
                                                   IObject **);

// 外部UI对象注册入口，由外部DLL实现该导出函数
#define FUN_RegisterUIObject_NAME "RegisterUIObject"
typedef int (*funRegisterUIObjectPtr)(IApplication *p);

struct UIMSG;
struct IMessage;

struct IAttributeMap;
struct IAttributeList;
UIAPI std::shared_ptr<IAttributeMap> UICreateIMapAttribute();
UIAPI int UICreateIListAttribute(IAttributeList **ppOut);

struct IRenderBitmap;
UIAPI std::shared_ptr<IRenderBitmap>
UICreateRenderBitmap(IApplication *pUIApp,
                     eGraphicsLibraryType eGraphicsRenderType,
                     IMAGE_ITEM_TYPE eType);

UIAPI int GetDpi();
UIAPI int ScaleByDpi(int x);
#define dpi(x) ui::ScaleByDpi(x)

UIAPI void UIUnitTest();
} // namespace ui
#endif