#pragma once

#include "Src\Base\Object\object.h"
#include "Inc\Interface\icontrol.h"

namespace UI
{
	interface IToolTipUI;

class Control : public Object
{
public:
    Control(IControl*);

	UI_BEGIN_MSG_MAP()
		UIMSG_GET_TOOLTIPINFO(OnGetToolTipInfo)
        UIMSG_QUERYINTERFACE(Control)
        UIMSG_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT(Object)

    IControl*  GetIControl() { return m_pIControl; }

    void  OnSerialize(SERIALIZEDATA* pData);

	void  ModifyControlStyle(ControlStyle* add, ControlStyle* remove);
	bool  TestControlStyle(ControlStyle* test);

	bool  IsGroup();
	void  SetGroup(bool b);

    ITextRenderBase*  GetTextRenderDefault();
    ITextRenderBase*  CreateDefaultTextRender();
	void TryUpdateLayoutOnContentChanged();

public:
	LPCTSTR  GetToolTipText();
	void  SetToolTipText(LPCTSTR szText);

protected:
	BOOL  OnGetToolTipInfo(TOOLTIPITEM* pItem, IToolTipUI* pUI);

protected:
    IControl*  m_pIControl;

	ControlStyle  m_controlStyle;
};

}

