#include "stdafx.h"
#include "control.h"
#include "Src\Base\Attribute\attribute.h"
#include "Src\Base\Attribute\bool_attribute.h"
#include "Src\Base\Application\uiapplication.h"
#include "Src\Base\Attribute\stringselect_attribute.h"
#include "Inc\Base\uidefine.h"
#include "Inc\Interface\itextrenderbase.h"
#include "Inc\Interface\iuiapplication.h"
#include "Src\Resource\skinres.h"
#include "Inc\Interface\ilayout.h"

using namespace UI;

Control::Control(IControl* p) : Object(p)
{
    m_pIControl = p;
	memset(&m_controlStyle, 0, sizeof(m_controlStyle));
	m_objStyle.tabstop = 1;
    m_objStyle.default_tabstop = 1;
}

void  Control::OnSerialize(SERIALIZEDATA* pData)
{
    __super::OnSerialize(pData);

	{
		AttributeSerializer s(pData, TEXT("Control"));

		s.AddBool(XML_CONTROL_STYLE_TABABLE, this, 
			memfun_cast<pfnBoolSetter>(&Object::SetTabstop),
			memfun_cast<pfnBoolGetter>(&Object::IsTabstop))
			->SetDefault(m_objStyle.default_tabstop);

		s.AddBool(XML_CONTROL_STYLE_GROUP, this, 
			memfun_cast<pfnBoolSetter>(&Control::SetGroup),
			memfun_cast<pfnBoolGetter>(&Control::IsGroup));

		s.AddString(XML_TOOLTIP, this,
			memfun_cast<pfnStringSetter>(&Control::SetToolTipText),
			memfun_cast<pfnStringGetter>(&Control::GetToolTipText));

		s.AddTextRenderBase(nullptr, this, m_pTextRender);
// 		s.AddStringEnum(XML_TEXTRENDER_TYPE, this,
// 			memfun_cast<pfnStringSetter>(&Object::LoadTextRender),
// 			memfun_cast<pfnStringGetter>(&Object::SaveTextRender))
// 			->FillTextRenderBaseTypeData()
// 			->ReloadOnChanged();
// 
// 		// 字体
// 		if (m_pTextRender)
// 		{
// 			SERIALIZEDATA data(*pData);
// 			data.szParentKey = XML_TEXTRENDER_TYPE;
// 			if (data.pUIApplication->IsDesignMode())
// 				data.SetErase(false);
// 			m_pTextRender->Serialize(&data);
// 		}

	}


}

void  Control::ModifyControlStyle(ControlStyle* add, ControlStyle* remove)
{
	if (add)
	{
		if (add->group)
			m_controlStyle.group = 1;
		if (add->ownerdraw)
			m_controlStyle.ownerdraw = 1;
	}

	if (remove)
	{
		if (remove->group)
			m_controlStyle.group = 0;
		if (remove->ownerdraw)
			m_controlStyle.ownerdraw = 0;
	}
}
bool  Control::TestControlStyle(ControlStyle* test)
{
	if (!test)
		return false;

	if (test->group && m_controlStyle.group)
		return true;
	if (test->ownerdraw && m_controlStyle.ownerdraw)
		return true;

	return false;
}

bool Control::IsGroup()
{
	return  m_controlStyle.group;
}
void Control::SetGroup(bool b)
{
	m_controlStyle.group = b;
}

// 获取字体，如果没有，则使用默认的
ITextRenderBase*  Control::GetTextRenderDefault()
{
    if (!m_pTextRender)
        CreateDefaultTextRender();

    return m_pTextRender;
}

// 如果没有在皮肤中配置字体，则外部可调用该函数在Paint时创建一个默认的字体
ITextRenderBase*  Control::CreateDefaultTextRender()
{
    if (!m_pTextRender)
    {
        GetUIApplication()->GetTextRenderFactroy().CreateTextRender(
            m_pSkinRes->GetISkinRes(), 
            TEXTRENDER_TYPE_SIMPLE,
            m_pIObject, 
            &m_pTextRender);

        if (m_pTextRender)
        {
			IMapAttribute* pMapAttr = m_pIMapAttributeRemain;
			if (!pMapAttr)
				UICreateIMapAttribute(&pMapAttr);
			
            SERIALIZEDATA data = {0};
            data.pUIApplication = GetUIApplication()->GetIUIApplication();
			data.pSkinRes = m_pSkinRes->GetISkinRes();
            data.pMapAttrib = pMapAttr;
            data.szPrefix = nullptr;
            data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
            m_pTextRender->Serialize(&data);

			if (!m_pIMapAttributeRemain)
				pMapAttr->Release();
        }
    }

    return m_pTextRender;
}



LPCTSTR  Control::GetToolTipText()
{
	return GetAttribute(XML_TOOLTIP, false);
}
void  Control::SetToolTipText(LPCTSTR szText)
{
	if (!szText || 0 == szText[0])
	{
		GetAttribute(XML_TOOLTIP, true);
	}
	else
	{
		AddAttribute(XML_TOOLTIP, szText);
	}
}

BOOL  Control::OnGetToolTipInfo(TOOLTIPITEM* pToolTipItem, IToolTipUI* pToolTip)
{
	if (nullptr == pToolTipItem || nullptr == pToolTip)
		return 0;

	LPCTSTR szTooltip = GetToolTipText();
	if (!szTooltip || !szTooltip[0])
		return 0;

    LPCTSTR szText = m_pSkinRes->GetI18nRes().MapConfigValue(szTooltip);
    pToolTip->SetText(szText);
	return 1;
}

// 当控件内容改变时，检查一下是否需要更新布局，如label.text, button.text
void Control::TryUpdateLayoutOnContentChanged()
{
	Object* pObj = this;
	while (pObj)
	{
		ILayoutParam*  param = pObj->GetSafeLayoutParam();
		if (!param || !param->IsSizedByContent())
			break;

		Object* parent = pObj->GetParentObject();
		if (!parent)
			break;

		UI::ILayout* pLayout = (UI::ILayout*)
			UISendMessage(parent, UI_MSG_GETLAYOUT);
		if (!pLayout)
			break;

		pLayout->SetDirty(true);
		parent->Invalidate();
		pObj = parent;
	}
}
