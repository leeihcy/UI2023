#ifndef _UI_API_H_
#define _UI_API_H_

#include "include/base/xmldefine.h"
#include "include/common.h"

namespace ui {

// 编译时版本定义
#define UISDK_VERSION_MAJOR 2 // 主版本
#define UISDK_VERSION_MINOR 0 // 次版本
#define UISDK_VERSION_PATCH 1 // 补丁版本

struct IUIElement;
struct IResBundle;
struct IObject;
struct ILayout;
struct IApplication;

UIAPI bool CreateUIApplication(IApplication **pp);

// UI对象创建函数
// typedef  long(*pfnUICreateRenderBasePtr)(IResBundle* pSkinRes, void** ppOut);
// typedef  long(*pfnUICreateTextRenderBasePtr)(IResBundle* pSkinRes, void**
// ppOut);
typedef long (*pfnParseSkinTag)(IUIElement *, IResBundle *pSkinRes);
// typedef  long(*pfnUICreateLayoutPtr)(IObject* pObject, ILayout**  ppLayout);

// UI对象创建函数
typedef void (*pfnUICreateRenderBasePtr)(IResBundle *pSkinRes, void **ppOut);
typedef void (*pfnUICreateTextRenderBasePtr)(IResBundle *pSkinRes,
                                             void **ppOut);
typedef long (*pfnParseSkinTag)(IUIElement *, IResBundle *pSkinRes);
typedef long (*pfnUICreateLayoutPtr)(IObject *pObject, ILayout **ppLayout);

// uiapplication中的枚举回调
typedef bool (*pfnEnumLayoutTypeCallback)(long, const wchar_t *, long, long);
typedef void (*pfnEnumRenderBaseNameCallback)(const wchar_t *, long, long);
typedef void (*pfnEnumTextRenderBaseNameCallback)(const wchar_t *, long, long);

// 返回值:
enum PARSE_CONTROL_RETURN {
  ParseControl_Failed,       // 解析错误
  ParseControl_CreateObject, // 解析，并创建了该对象，但没有加载属性
  ParseControl_LoadObject, // 创建对象并加载其属性，但处没处理子结点
  ParseControl_LoadDescendants, // 加载自己和所有子结点的属性
};
typedef PARSE_CONTROL_RETURN (*pfnParseControlTag)(IUIElement *, IResBundle *,
                                                   IObject *pObjParent,
                                                   IObject **);

// 外部UI对象注册入口，由外部DLL实现该导出函数
#define FUN_RegisterUIObject_NAME "RegisterUIObject"
typedef long (*funRegisterUIObjectPtr)(IApplication *p);

struct UIMSG;
struct IMessage;
UIAPI long UIPostMessage(IApplication *pUIApp, UIMSG *pMsg, int nMsgMapID = 0);
UIAPI long UISendMessage(UIMSG *pMsg, int nMsgMapID = 0,
                         bool *pbHandled = nullptr);
UIAPI long UISendMessage(IMessage *pMsgTo, uint message, long wParam = 0,
                         long lParam = 0, uint nCode = 0,
                         IMessage *pMsgFrom = nullptr, int nMsgMapID = 0,
                         bool *pbHandled = nullptr);

struct IMapAttribute;
struct IListAttribute;
UIAPI long UICreateIMapAttribute(IMapAttribute **ppOut);
UIAPI long UICreateIListAttribute(IListAttribute **ppOut);

struct IRenderBitmap;
UIAPI void
UICreateRenderBitmap(IApplication *pUIApp,
                     GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType,
                     IMAGE_ITEM_TYPE eType, IRenderBitmap **ppOut);

struct IRenderTarget;
UIAPI IRenderTarget *UICreateRenderTarget(IApplication *pUIApp,
                                          GRAPHICS_RENDER_LIBRARY_TYPE eType,
                                          bool bNeedAlphaChannel);

UIAPI long GetDpi();
UIAPI long ScaleByDpi(long x);
#define dpi(x) ui::ScaleByDpi(x)

} // namespace ui
#endif