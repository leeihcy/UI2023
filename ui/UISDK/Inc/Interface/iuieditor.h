#pragma once

namespace UI
{
    interface IImageResItem;
    interface IGifResItem;
    interface ICursorResItem;
    interface IAttributeEditorProxy;
    interface IStringAttribute;
    interface IBoolAttribute;
    interface ILongAttribute;
	interface IEnumAttribute;
    interface IFlagsAttribute;
    interface IRectAttribute;
	interface ISizeAttribute;
	interface IRegion9Attribute;
    interface IStringEnumAttribute;
	interface IStyleResItem;

	struct  EditorAddObjectResData
	{
		IUIApplication*  pUIApp;
		IObject*  pParentObj;
		IUIElement*  pParentXml;
		bool  bNcChild;

		GUID  objiid;
		LPCTSTR  szId;
		void** ppCreateObj;
		IUIElement**  ppCreateXml;
	};
	
	//  外部的编辑器指针，用于消息通知和数据获取
	enum EditorAttributeFlag
	{
		EDITOR_ATTRIBUTE_ADD,
		EDITOR_ATTRIBUTE_UPDATE,
	};

	enum EditorOPType
	{
		Editor_None,
		Editor_Add,
		Editor_Remove,
		Editor_Modify,
		// Editor_Clear,
	};

	interface IUIEditor
	{
		virtual ~IUIEditor() = 0{};

        virtual bool  AddObjectRes(
                EditorAddObjectResData* pData) = 0;

		// 从配置文件中读取完一个对象的属性配置
		virtual void  OnObjectAttributeLoad(
                __in IObject*  pObj, 
                __in IUIElement* pXmlElem) = 0;
        virtual void  OnObjectDeleteInd(
                __in IObject* pObj) = 0;
        virtual void  OnImageItemLoad(
                __in IImageResItem*  pItem, 
                __in IUIElement* pXmlElem) = 0;
        virtual void  OnGifItemLoad(
                __in IGifResItem*  pItem, 
                __in IUIElement* pXmlElem) = 0;
        virtual void  OnCursorItemLoad(
                __in ICursorResItem*  pItem, 
                __in IUIElement* pXmlElem) = 0;
        virtual void  OnImageItemDeleteInd(
                __in IImageResItem*  pItem) = 0;
        virtual void  OnGifItemDeleteInd(
                __in IGifResItem*  pItem) = 0;
        virtual void  OnCursorItemDeleteInd(
                __in ICursorResItem*  pItem) = 0;

		virtual void  OnStyleChanged(
                ISkinRes* pSkinRes, 
                IStyleResItem* p, 
                EditorOPType e) PURE;
		virtual void  OnStyleAttributeChanged(
                ISkinRes* pSkinRes, 
                IStyleResItem* p, 
                LPCTSTR szKey, 
                EditorOPType e) PURE;
        virtual void  EditorStringAttribute(
                IStringAttribute*, 
                EditorAttributeFlag e) PURE;
        virtual void  EditorBoolAttribute(
                IBoolAttribute*, 
                EditorAttributeFlag e) PURE;
        virtual void  EditorLongAttribute(
                ILongAttribute*,
                EditorAttributeFlag e) PURE;
		virtual void  EditorEnumAttribute(
                IEnumAttribute*,
                EditorAttributeFlag e) PURE;
        virtual void  EditorFlagsAttribute(
                IFlagsAttribute*,
                EditorAttributeFlag e) PURE;
        virtual void  EditorRectAttribute(
                IRectAttribute*, 
                EditorAttributeFlag e) PURE;
		virtual void  EditorSizeAttribute(
				ISizeAttribute*,
				EditorAttributeFlag e) PURE;
		virtual void  EditorRegion9Attribute(
                IRegion9Attribute*, 
                EditorAttributeFlag e) PURE;
        virtual void  EditorStringEnumAttribute(
                IStringEnumAttribute*, 
                EditorAttributeFlag e) PURE;
		virtual void  EditorColorAttribute(
				IColorAttribute*,
				EditorAttributeFlag e) PURE;
        virtual void  EditorRenderBase(
                IRenderBaseAttribute* pAttribute, 
                EditorAttributeFlag e) PURE;
        virtual void  EditorTextRenderBase(
                ITextRenderBaseAttribute* pAttribute,
                EditorAttributeFlag e) PURE;

        // 获取对象列表，添加到工具箱中。
        // 调用LoadUIObjectListToUIEdirot时触发
        virtual void  OnToolBox_AddObject(
                IObjectDescription*) = 0;
	};

	
};

