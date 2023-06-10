#pragma  once
#include "UISDK\Kernel\Src\UIObject\Window\customwindow.h"

namespace UI
{
#if 0
//
// TODO: Œ¥ π”√
//
class TooltipWindow: public CustomWindow, public IToolTipUI
{
public:
	TooltipWindow();
	virtual ~TooltipWindow();

	UI_DECLARE_OBJECT_INFO(TooltipWindow, OBJ_WINDOW|WINDOW_TOOLTIP)

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
	UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

public:
	void   OnPaint(IRenderTarget* pRenderTarget);

	BOOL  PreCreateWindow(CREATESTRUCT* pcs);
	virtual void  OnFinalMessage();

	virtual bool  Create();
	virtual bool  Destroy();
	virtual bool  SetText(const String& strText);
	virtual bool  SetTitle(const String& strText);
	virtual bool  Show();
	virtual bool  Hide();
	virtual void  OnSerialize(SERIALIZEDATA* pData);
private:
	String   m_strText;
	int      m_nLimitWidth;
};
#endif

}