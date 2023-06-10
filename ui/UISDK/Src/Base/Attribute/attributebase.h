#pragma once
#include "Inc\Interface\iattribute.h"
#include "Src\Resource\skinres.h"

namespace UI
{
    class AttributeSerializer;
    class AttributeEditorProxy;
	class UIApplication;

    // 属性基类
    class AttributeBase
    {
    public:
		AttributeBase();

        virtual ~AttributeBase() {};
        virtual void  Release() { delete this; };

        virtual LPCTSTR  Get() PURE; 
        virtual void  Set(LPCTSTR) PURE;
        // reload时，如果在xml中没有显式配置该属性，则调用Reset，而不是Set
		virtual void  Reset() PURE;   
        virtual bool  IsDefaultValue() PURE;

		// 加载、保存、编辑器显示
		virtual void  Load(SERIALIZEDATA*);
		virtual void  Save(SERIALIZEDATA*);
		virtual void  Editor(
						SERIALIZEDATA* pData, 
						AttributeEditorProxy* p,
						EditorAttributeFlag e) PURE;

        // 已过期，将删除。修改为各个类自己的方法来设置。参照StringAttribute.
        virtual void  SetBindValue(void*) {};
        virtual void  SetBindFuction(void* _this, void* _setter, void* _getter) {};

    public:
		void  SetKey(LPCTSTR szKey);
		LPCTSTR  GetKey();
		AttributeBase*  SetCompatibleKey(LPCTSTR szKey);
		LPCTSTR  GetCompatibleKey();
		bool  IsKey(LPCTSTR szKey);

		LPCTSTR  GetDesc();
		AttributeBase*  AsData();
		bool  IsData();
		AttributeBase*  ReloadOnChanged();
		bool  IsReloadOnChanged();

		AttributeBase*  SetParentKey(LPCTSTR szParent);
		LPCTSTR  GetParentKey();

		void  SetGroupName(LPCTSTR szGroupName);
		LPCTSTR  GetGroupName();
		void  SetUIApplication(UIApplication* p);
		void  SetSkinRes(SkinRes* p);
		UIApplication*  GetUIApplication();

    protected:
        LPCTSTR  ConstructTempLPCTSTR(long lValue);

    private:
        String  m_strKey;
		String  m_strKeyCompatible; // 兼容key。例如：width="100"，也可以写成 layout.width="100"
		String  m_strDesc;

		// 例如back.render.image，它的groupname是ImageRender，它的
		// parentkey是back.render.type

		// 属性所属的对象名，例如"Object" "ImageRender"
        String  m_strGroupName;  
		// 父结点fullkey。例如bkg.render.type
        String  m_strParentKey;  
        bool  m_bParentKeyIsNull;

        // 该属性是否是一个控件data。data在换肤的时候不用再加载一次
        bool  m_bData;  

        // 对于bkg.render.type属性修改时，在editor中重新加载这个控件的所有属性，来切换具体的render属性
        // 目前没想到一个较好的办法来只加载/删除相应的属性。
        bool  m_bReloadOnChanged;  

    protected:
        UIApplication*  m_pUIApplication;
		SkinRes*  m_pSkinRes;
    };
}
