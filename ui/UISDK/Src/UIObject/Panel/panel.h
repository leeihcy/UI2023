#pragma once

#include "include/interface/ipanel.h"
#include "src/object/object.h"
#include "panel_desc.h"


namespace ui
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
	virtual void  virtualOnSize(unsigned int nType, unsigned int nWidth, unsigned int nHeight) override;

	void  OnEraseBkgnd(IRenderTarget*);
	void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnPostPaint(IRenderTarget* pRenderTarget);
	long  OnGetLayoutPtr(unsigned int uMsg, WPARAM wParam, LPARAM lParam);
	void  OnSerialize(SERIALIZEDATA* pData);
	void  OnGetDesiredSize(SIZE* pSize);  // ����ΪOn,������object::GetDesiredSize��ͻ 
private:
    
protected:
    IPanel*    m_pIPanel;
	ILayout*   m_pLayout;
	CRegion4   m_rcBkgndRenderRegion;
	CRegion4   m_rcForegndRenderRegion;

    // ����֧�ֻ������ܵ�Ƥ��ͼƬ
    IRenderBase*   m_pTextureRender;
    CRegion4       m_rcTextureRenderRegion;

    // ����������ֲ�
    IRenderBase*   m_pMaskRender;  
    CRegion4       m_rcMaskRenderRegion;
};

}

