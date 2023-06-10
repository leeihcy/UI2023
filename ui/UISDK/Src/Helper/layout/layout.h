#ifndef LAYOUT_H_70402CBF_A4C2_4a35_AA14_829F26BA9A5C
#define LAYOUT_H_70402CBF_A4C2_4a35_AA14_829F26BA9A5C

#include "Inc\Interface\ilayout.h"
#include "Src\Base\Object\object.h"
#include "Inc\Interface\imapattr.h"

namespace UI
{
class Panel;
class WindowBase;

#if 0

/*
**	注意：DockLayout只允许有一个center对象，但该对象可以是一个Panel、Object
**		  如果在配置文件中有多个center对象，则只取第一个对象
*/
class DockLayout : public Layout
{
public:
	virtual SIZE  MeasureChildObject();
	virtual void  ArrangeChildObject(Object* pObjToArrage = nullptr);
};


class LayoutManagerFactory 
{
public:
	static Layout* GetLayout(const TCHAR* szType, Object* pPanel);
	static Layout* GetLayoutFromAttribute(IMapAttribute* pMapAttrib, Object* pObject, bool bCreateDefaultCanvasLayout=true);
};
#endif

template <class TLayout, class TLayoutInterface, class TLayoutParam, LAYOUTTYPE tLayoutType>
class LayoutImpl : public TLayoutInterface
{
public:
    LayoutImpl()
    {
        m_pPanel = nullptr;
        m_bDirty = true;
    }
    virtual ~LayoutImpl()
    {
        if (m_pPanel)
        {
            Object*  pChild = nullptr;
            while (pChild = this->m_pPanel->EnumChildObject(pChild))
            {
                pChild->SetLayoutParam(nullptr);
            }
        }
    }
    void  Release()
    {
        delete this;    
    }

    virtual LAYOUTTYPE  GetLayoutType() 
    { 
        return tLayoutType; 
    }

    static HRESULT  CreateInstance(IObject* p, ILayout** ppOut)
    {
        if (nullptr == ppOut || nullptr == p)
            return E_INVALIDARG;

        TLayout*  pLayout = new TLayout;
        pLayout->m_pPanel = p->GetImpl();
        *ppOut = pLayout;
        return S_OK;
    }
    ILayoutParam*  CreateLayoutParam(IObject* pObj)
    {
        return s_CreateLayoutParam(pObj);
    }

    static ILayoutParam*  s_CreateLayoutParam(IObject* pObj)
    {
        if (!pObj)
            return nullptr;

        TLayoutParam* p = new TLayoutParam();
        p->SetObject(pObj->GetImpl());
        IMapAttribute* pMapAttr = nullptr;
        pObj->GetImpl()->GetMapAttribute(&pMapAttr);
        
		if (!pMapAttr)
		{
			UICreateIMapAttribute(&pMapAttr);
		}
        if (pMapAttr)
        {
            SERIALIZEDATA data = {0};
            data.pUIApplication = pObj->GetUIApplication();
			data.pSkinRes = pObj->GetSkinRes();
            data.nFlags = SERIALIZEFLAG_LOAD | SERIALIZEFLAG_LOAD_ERASEATTR;
            data.pMapAttrib = pMapAttr;

            p->Serialize(&data);

			// 节省内存
			if (pMapAttr->GetAttrCount() == 0)
			{
				pObj->GetImpl()->ClearMapAttribute();
			}
            SAFE_RELEASE(pMapAttr);
        }

        return static_cast<ILayoutParam*>(p);
    }

    // 自己在布局的时候调用
    static TLayoutParam*  s_GetObjectLayoutParam(Object* pObj)
    {
        if (!pObj)
            return nullptr;

        ILayoutParam* pParam = pObj->GetLayoutParam();
        if (pParam && pParam->GetLayoutType() == tLayoutType)
        {
            return static_cast<TLayoutParam*>(pParam);
        }

        pParam = s_CreateLayoutParam(pObj->GetIObject());
        pObj->SetLayoutParam(pParam);
        return static_cast<TLayoutParam*>(pParam);
    }

    // Arrage由基类统管，子类实现DoArrage
    virtual void  DoArrage(IObject* pObjToArrage = nullptr) = 0;
    virtual void  Arrange(IObject* pObjToArrage = nullptr) override
    {
        m_bDirty = false;
        DoArrage(pObjToArrage);
    }

    virtual bool  IsDirty() override
    {
        return m_bDirty;
    }
    void  SetDirty(bool b)
    {
        m_bDirty = b;
    }

    virtual void  ChildObjectContentSizeChanged(IObject* pObj) override
    {

    }

protected:
    Object*   m_pPanel;   // 与该布局关联的panel
    bool  m_bDirty;   // 是否需要布局
};

template<class T>
class LayoutParamImpl : public T
{
public:
    LayoutParamImpl()
    {
        m_pObj = nullptr;
    }
    virtual ~LayoutParamImpl()
    {

    }
    virtual void  Release()
    {
        delete this;
    }

    void  SetObject(Object* p)
    {
        m_pObj = p;
    }
    virtual void  UpdateByRect()
    {

    }
    virtual void  Serialize(SERIALIZEDATA* pData)
    {

    }

	virtual SIZE  CalcDesiredSize()
    {
        SIZE s = {0, 0};

        if (m_pObj)
        {
            // 计算 margin 的大小
            s.cx += m_pObj->GetMarginW();
            s.cy += m_pObj->GetMarginH();
        }

        return s;
    }
    

protected:
    Object*  m_pObj;
};

// 桌面布局管理器，专门用于设置顶层窗口在屏幕中的大小与位置
// 桌面布局有些特殊，它直接管理窗口的位置，但窗口和控件、Panel有很多不一样的地方，
// 如窗口分客户区和非客户区。而且DesktopLayout只需要管理一个对象，即Window，没有其他
// 子对象，因此这里不将DesktopLayout派生自LayoutManager
class DesktopLayout   
{
public:
    void  Arrange(WindowBase*  pWindow);
};


void  LoadConfigWH(LPCTSTR szText, long& wh, long& whtype);
LPCTSTR  SaveConfigWH(long wh, long whtype);
}

#endif // LAYOUT_H_70402CBF_A4C2_4a35_AA14_829F26BA9A5C