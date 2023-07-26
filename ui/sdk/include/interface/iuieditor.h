#pragma once
#include "include/common/guid/guid.h"

namespace ui
{
    struct IImageResItem;
    struct IGifResItem;
    struct ICursorResItem;
    struct IAttributeEditorProxy;
    struct IStringAttribute;
    struct IBoolAttribute;
    struct IColorAttribute;
    struct ILongAttribute;
	struct IEnumAttribute;
    struct IFlagsAttribute;
    struct IRectAttribute;
	struct ISizeAttribute;
	struct IRegion9Attribute;
    struct IStringEnumAttribute;
	struct IStyleResItem;
    struct IRenderBaseAttribute;
    struct ITextRenderBaseAttribute;
    struct IObject;
    struct IObjectDescription;
    struct IUIElement;

	struct  EditorAddObjectResData
	{
		IUIApplication*  pUIApp;
		IObject*  pParentObj;
		IUIElement*  pParentXml;
		bool  bNcChild;

		Guid  objiid;
		const wchar_t*  szId;
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

	struct IUIEditor
	{
		virtual ~IUIEditor() {};

        virtual bool  AddObjectRes(
                EditorAddObjectResData* pData) = 0;

		// 从配置文件中读取完一个对象的属性配置
		virtual void  OnObjectAttributeLoad(
                /*__in*/ IObject*  pObj, 
                /*__in*/ IUIElement* pXmlElem) = 0;
        virtual void  OnObjectDeleteInd(
                /*__in*/ IObject* pObj) = 0;
        virtual void  OnImageItemLoad(
                /*__in*/ IImageResItem*  pItem, 
                /*__in*/ IUIElement* pXmlElem) = 0;
        virtual void  OnGifItemLoad(
                /*__in*/ IGifResItem*  pItem, 
                /*__in*/ IUIElement* pXmlElem) = 0;
        virtual void  OnCursorItemLoad(
                /*__in*/ ICursorResItem*  pItem, 
                /*__in*/ IUIElement* pXmlElem) = 0;
        virtual void  OnImageItemDeleteInd(
                /*__in*/ IImageResItem*  pItem) = 0;
        virtual void  OnGifItemDeleteInd(
                /*__in*/ IGifResItem*  pItem) = 0;
        virtual void  OnCursorItemDeleteInd(
                /*__in*/ ICursorResItem*  pItem) = 0;

		virtual void  OnStyleChanged(
                IResBundle* pSkinRes, 
                IStyleResItem* p, 
                EditorOPType e) = 0;
		virtual void  OnStyleAttributeChanged(
                IResBundle* pSkinRes, 
                IStyleResItem* p, 
                const wchar_t* szKey, 
                EditorOPType e) = 0;
        virtual void  EditorStringAttribute(
                IStringAttribute*, 
                EditorAttributeFlag e) = 0;
        virtual void  EditorBoolAttribute(
                IBoolAttribute*, 
                EditorAttributeFlag e) = 0;
        virtual void  EditorLongAttribute(
                ILongAttribute*,
                EditorAttributeFlag e) = 0;
		virtual void  EditorEnumAttribute(
                IEnumAttribute*,
                EditorAttributeFlag e) = 0;
        virtual void  EditorFlagsAttribute(
                IFlagsAttribute*,
                EditorAttributeFlag e) = 0;
        virtual void  EditorRectAttribute(
                IRectAttribute*, 
                EditorAttributeFlag e) = 0;
		virtual void  EditorSizeAttribute(
				ISizeAttribute*,
				EditorAttributeFlag e) = 0;
		virtual void  EditorRegion9Attribute(
                IRegion9Attribute*, 
                EditorAttributeFlag e) = 0;
        virtual void  EditorStringEnumAttribute(
                IStringEnumAttribute*, 
                EditorAttributeFlag e) = 0;
		virtual void  EditorColorAttribute(
				IColorAttribute*,
				EditorAttributeFlag e) = 0;
        virtual void  EditorRenderBase(
                IRenderBaseAttribute* pAttribute, 
                EditorAttributeFlag e) = 0;
        virtual void  EditorTextRenderBase(
                ITextRenderBaseAttribute* pAttribute,
                EditorAttributeFlag e) = 0;

        // 获取对象列表，添加到工具箱中。
        // 调用LoadUIObjectListToUIEdirot时触发
        virtual void  OnToolBox_AddObject(
                IObjectDescription*) = 0;
	};

	
};

