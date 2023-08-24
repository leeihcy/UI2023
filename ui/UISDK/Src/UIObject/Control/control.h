#pragma once

#include "src/object/object.h"
#include "include/interface/icontrol.h"

namespace ui
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

    void  OnSerialize(SerializeParam* pData);

	void  ModifyControlStyle(ControlStyle* add, ControlStyle* remove);
	bool  TestControlStyle(ControlStyle* test);

	bool  IsGroup();
	void  SetGroup(bool b);

    ITextRenderBase*  GetTextRenderDefault();
    ITextRenderBase*  CreateDefaultTextRender();
	void TryUpdateLayoutOnContentChanged();

public:
	const wchar_t*  GetToolTipText();
	void  SetToolTipText(const wchar_t* szText);

protected:
	BOOL  OnGetToolTipInfo(TOOLTIPITEM* pItem, IToolTipUI* pUI);

protected:
    IControl*  m_pIControl;

	ControlStyle  m_controlStyle;
};

}

