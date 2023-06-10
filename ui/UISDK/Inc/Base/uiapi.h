#ifndef _UI_API_H_
#define _UI_API_H_

namespace UI
{

	// 编译时版本定义
#define UISDK_VERSION_MAJOR   1    // 主版本
#define UISDK_VERSION_MINOR   0    // 次版本
#define UISDK_VERSION_PATCH   0    // 补丁版本

	interface IUIElement;
	interface ISkinRes;
	interface IObject;
	interface ILayout;
	interface IUIApplication;

	UIAPI bool  CreateUIApplication(IUIApplication** pp);

	// UI对象创建函数
	//typedef  HRESULT(*pfnUICreateRenderBasePtr)(ISkinRes* pSkinRes, void** ppOut);
	//typedef  HRESULT(*pfnUICreateTextRenderBasePtr)(ISkinRes* pSkinRes, void** ppOut);
	typedef  HRESULT(*pfnParseSkinTag)(IUIElement*, ISkinRes* pSkinRes);
	//typedef  HRESULT(*pfnUICreateLayoutPtr)(IObject* pObject, ILayout**  ppLayout);

	// UI对象创建函数
	typedef  void(*pfnUICreateRenderBasePtr)(ISkinRes* pSkinRes, void** ppOut);
	typedef  void(*pfnUICreateTextRenderBasePtr)(ISkinRes* pSkinRes, void** ppOut);
	typedef  HRESULT(*pfnParseSkinTag)(IUIElement*, ISkinRes* pSkinRes);
	typedef  HRESULT(*pfnUICreateLayoutPtr)(IObject* pObject, ILayout**  ppLayout);

	// uiapplication中的枚举回调
	typedef  bool(*pfnEnumLayoutTypeCallback)(long, LPCTSTR, WPARAM, LPARAM);
	typedef  void(*pfnEnumRenderBaseNameCallback)(LPCTSTR, WPARAM, LPARAM);
	typedef  void(*pfnEnumTextRenderBaseNameCallback)(LPCTSTR, WPARAM, LPARAM);


	// 返回值:
	enum  PARSE_CONTROL_RETURN
	{
		ParseControl_Failed,  // 解析错误
		ParseControl_CreateObject, // 解析，并创建了该对象，但没有加载属性
		ParseControl_LoadObject,   // 创建对象并加载其属性，但处没处理子结点
		ParseControl_LoadDescendants,  // 加载自己和所有子结点的属性
	};
	typedef  PARSE_CONTROL_RETURN(*pfnParseControlTag)(IUIElement*, ISkinRes*, IObject* pObjParent, IObject**);

	// 外部UI对象注册入口，由外部DLL实现该导出函数
#define FUN_RegisterUIObject_NAME  "RegisterUIObject"
	typedef  HRESULT(*funRegisterUIObjectPtr)(IUIApplication* p);


	struct UIMSG;
	interface IMessage;
 	UIAPI LONG_PTR  UIPostMessage(IUIApplication* pUIApp, UIMSG* pMsg, int nMsgMapID = 0);
	UIAPI LONG_PTR  UISendMessage(UIMSG* pMsg, int nMsgMapID = 0, BOOL* pbHandled = nullptr);
	UIAPI LONG_PTR  UISendMessage(IMessage* pMsgTo, UINT message, WPARAM wParam = 0, LPARAM lParam = 0,
						UINT nCode = 0, IMessage* pMsgFrom = nullptr, int nMsgMapID = 0, BOOL* pbHandled = nullptr);

	interface IMapAttribute;
	interface IListAttribute;
	UIAPI HRESULT  UICreateIMapAttribute(IMapAttribute** ppOut);
	UIAPI HRESULT  UICreateIListAttribute(IListAttribute** ppOut);

	enum GRAPHICS_RENDER_LIBRARY_TYPE;
	enum IMAGE_ITEM_TYPE;
	interface IRenderBitmap;
	UIAPI void  UICreateRenderBitmap(
                    IUIApplication*  pUIApp,
                    GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType, 
                    IMAGE_ITEM_TYPE eType, 
                    IRenderBitmap** ppOut);

    interface IRenderTarget;
    UIAPI IRenderTarget*  
        UICreateRenderTarget(
                    IUIApplication* pUIApp,
                    GRAPHICS_RENDER_LIBRARY_TYPE eType,
                    bool bNeedAlphaChannel);

    UIAPI long GetDpi();
    UIAPI long ScaleByDpi(long x);
#define dpi(x)  UI::ScaleByDpi(x)

}
#endif