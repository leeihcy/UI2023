#pragma once

#include "Inc\Interface\ipanel.h"
#include "Src\Base\Object\object.h"
#include "panel_desc.h"


namespace UI
{

class Panel : public Object
{
public:
	Panel(IPanel* p);
	~Panel();

	UI_BEGIN_MSG_MAP()
		UIMSG_ERASEBKGND(OnEraseBkgnd)
		UIMSG_PAINT(OnPaint)
        UIMSG_POSTPAINT(OnPostPaint)
        UIMSG_GETDESIREDSIZE(OnGetDesiredSize)
		UIMSG_HANDLER_EX(UI_MSG_GETLAYOUT, OnGetLayoutPtr)
        UIMSG_QUERYINTERFACE(Panel)
		UIMSG_SERIALIZE(OnSerialize)
	UI_END_MSG_MAP_CHAIN_PARENT(Object)

    IPanel* GetIPanel() { return m_pIPanel; }

public:
    ILayout*  GetLayout();
    LAYOUTTYPE  GetLayoutType();
    void  SetLayoutType(LAYOUTTYPE eLayoutType);

    void  SetTextureRender(IRenderBase* p);
    IRenderBase*  GetTextureRender();

	IObjectDescription*  GetObjectDescription() {
		return PanelDescription::Get();
	}

protected:
	virtual void  virtualOnSize(UINT nType, UINT nWidth, UINT nHeight) override;

	void  OnEraseBkgnd(IRenderTarget*);
	void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnPostPaint(IRenderTarget* pRenderTarget);
	LRESULT  OnGetLayoutPtr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void  OnSerialize(SERIALIZEDATA* pData);
	void  OnGetDesiredSize(SIZE* pSize);  // 命名为On,避免与object::GetDesiredSize冲突 
private:
    
protected:
    IPanel*    m_pIPanel;
	ILayout*   m_pLayout;
	CRegion4   m_rcBkgndRenderRegion;
	CRegion4   m_rcForegndRenderRegion;

    // 用于支持换肤功能的皮肤图片
    IRenderBase*   m_pTextureRender;
    CRegion4       m_rcTextureRenderRegion;

    // 最上面的遮罩层
    IRenderBase*   m_pMaskRender;  
    CRegion4       m_rcMaskRenderRegion;
};

}

