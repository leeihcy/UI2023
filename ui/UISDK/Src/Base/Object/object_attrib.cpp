#include "stdafx.h"
#include "object.h"
#include "Inc\Interface\ilayout.h"

#include "Src\Base\Attribute\attribute.h"
#include "Src\Base\Attribute\bool_attribute.h"
#include "Src\Base\Attribute\enum_attribute.h"
#include "Src\Base\Attribute\stringselect_attribute.h"
#include "Inc\Interface\irenderbase.h"
#include "Inc\Interface\itextrenderbase.h"
#include "Inc\Interface\imapattr.h"
#include "Inc\Interface\iuiapplication.h"

// 将xml用的配置转成对象的属性，注意，子类重载该函数时，必须先调用父类的该方法
// bReload表示为换肤时调用，此时不能再给某些属性赋值，例如text属性
void  Object::OnSerialize(SERIALIZEDATA* pData)
{
    IMapAttribute*  pMapAttrib = pData->pMapAttrib;
	if (pData->IsReload())
	{
		SAFE_RELEASE(m_pLayoutParam);
		SAFE_RELEASE(m_pBkgndRender);
		SAFE_RELEASE(m_pForegndRender);
		SAFE_RELEASE(m_pTextRender);
		//SAFE_RELEASE(m_pCursor);
	}

    if (pData->IsLoad())
    {
        if (nullptr == pMapAttrib)
        {
            UI_LOG_ERROR(_T("Invalid Argument."));
            return;
        }
    }

    {
		AttributeSerializer s(pData, TEXT("Object"));
        s.AddString(XML_ID, m_strId)->AsData();

#ifdef EDITOR_MODE
        s.AddString(XML_STYLECLASS,
            [this](LPCTSTR t){ if (t){m_strStyle = t;}else {m_strStyle.clear();} },
            [this]()->LPCTSTR{ return m_strStyle.c_str(); })
            ->ReloadOnChanged();
#endif

		// styelclass被修改时，应该重新解析所有属性
        // s.AddString(XML_STYLECLASS, m_strStyleClass)->ReloadOnChanged();
        // s.AddString(XML_TOOLTIP, m_strTooltip);

        s.AddRect(XML_MARGIN, this,
            memfun_cast<pfnRectSetter>(&Object::LoadMargin),
            memfun_cast<pfnRectGetter>(&Object::SaveMargin));
        s.AddRect(XML_PADDING, this,
            memfun_cast<pfnRectSetter>(&Object::LoadPadding),
            memfun_cast<pfnRectGetter>(&Object::SavePadding));
	    s.AddRect(XML_BORDER, this,
            memfun_cast<pfnRectSetter>(&Object::LoadBorder),
            memfun_cast<pfnRectGetter>(&Object::SaveBorder));

        s.AddEnum(XML_VISIBLE, this,
            memfun_cast<pfnLongSetter>(&Object::LoadVisibleEx), 
            memfun_cast<pfnLongGetter>(&Object::SaveVisibleEx))
			->AddOption(VISIBILITY_COLLAPSED, XML_VISIBILITY_COLLAPSED)
			->AddOption(VISIBILITY_VISIBLE, XML_VISIBILITY_VISIBLE)
			->AddOption(VISIBILITY_HIDDEN, XML_VISIBILITY_HIDDEN)
		    ->SetDefault(VISIBILITY_VISIBLE)->AsData();

        s.AddBool(XML_DISABLE, this, 
            memfun_cast<pfnBoolSetter>(&Object::SetDisableDirect), 
            memfun_cast<pfnBoolGetter>(&Object::IsSelfDisable))
            ->AsData();

	    s.AddBool(XML_BACKGND_IS_TRANSPARENT, this, 
		    memfun_cast<pfnBoolSetter>(&Object::SetTransparent), 
		    memfun_cast<pfnBoolGetter>(&Object::IsTransparent))
            ->SetDefault(m_objStyle.default_transparent);

		s.AddLong(XML_ZORDER, *(long*)&m_lzOrder);  // z序 (注：默认在从xml加载之后，AddChild之前会先解析一次)
		s.AddLong(XML_MAXWIDTH, *(long*)&m_nMaxWidth)
			->SetDefault(NDEF)
			->SetDpiScaleType(LONGATTR_DPI_SCALE_GRATETHAN_0);
		s.AddLong(XML_MAXHEIGHT, *(long*)&m_nMaxHeight)
			->SetDefault(NDEF)
			->SetDpiScaleType(LONGATTR_DPI_SCALE_GRATETHAN_0);
		s.AddLong(XML_MINWIDTH, m_lMinWidth)
			->SetDefault(NDEF)
			->SetDpiScaleType(LONGATTR_DPI_SCALE_GRATETHAN_0);
		s.AddLong(XML_MINHEIGHT, m_lMinHeight)
			->SetDefault(NDEF)
			->SetDpiScaleType(LONGATTR_DPI_SCALE_GRATETHAN_0);

		s.AddBool(XML_NO_CLIP_DRAW, this, 
		    memfun_cast<pfnBoolSetter>(&Object::SetNoClip), 
		    memfun_cast<pfnBoolGetter>(&Object::IsNoClip));

        s.AddBool(XML_REJEST_MOUSE_MSG, this, 
            memfun_cast<pfnBoolSetter>(&Object::SetRejectMouseMsgAll), 
            memfun_cast<pfnBoolGetter>(&Object::IsRejectMouseMsgAll))
            ->SetDefault(m_objStyle.default_reject_all_mouse_msg);
		s.AddBool(XML_REJEST_SELF_MOUSE_MSG, this, 
			memfun_cast<pfnBoolSetter>(&Object::SetRejectMouseMsgSelf), 
			memfun_cast<pfnBoolGetter>(&Object::IsRejectMouseMsgSelf))
			->SetDefault(m_objStyle.default_reject_self_mouse_msg);

		s.AddBool(XML_ISNCCHILD, this,
			memfun_cast<pfnBoolSetter>(&Object::SetAsNcObject), 
			memfun_cast<pfnBoolGetter>(&Object::IsNcObject))
			->SetDefault(m_objStyle.default_ncobject);

        s.AddBool(XML_LAYER, this, 
            memfun_cast<pfnBoolSetter>(&Object::load_layer_config), 
            memfun_cast<pfnBoolGetter>(&Object::HasLayer));

// 	    s.AddString(XML_CURSOR, this, 
// 		    memfun_cast<pfnStringSetter>(&Object::SetCursorId),
// 		    memfun_cast<pfnStringGetter>(&Object::SaveCursorId));

       s.AddRenderBase(XML_BACKGND_RENDER_PREFIX, this, m_pBkgndRender);  // 设置背景渲染器
	   s.AddRenderBase(XML_FOREGND_RENDER_PREFIX, this, m_pForegndRender); // 设置前景绘制

//         s.AddStringEnum(XML_BACKGND_RENDER_PREFIX XML_RENDER_TYPE, this,
//             memfun_cast<pfnStringSetter>(&Object::LoadBkgndRender),
//             memfun_cast<pfnStringGetter>(&Object::SaveBkgndRender))
//             ->FillRenderBaseTypeData()
//             ->ReloadOnChanged();
//         s.AddStringEnum(XML_FOREGND_RENDER_PREFIX XML_RENDER_TYPE, this,
//             memfun_cast<pfnStringSetter>(&Object::LoadForegndRender),
//             memfun_cast<pfnStringGetter>(&Object::SaveForegndRender))
//             ->FillRenderBaseTypeData()
//             ->ReloadOnChanged();

	   // 背景属性
// 	   if (m_pBkgndRender)
// 	   {
// 		   SERIALIZEDATA data(*pData);
// 		   data.szParentKey = XML_BACKGND_RENDER_PREFIX XML_RENDER_TYPE;
// 		   data.szPrefix = XML_BACKGND_RENDER_PREFIX;
// 
// 		   // 在editor中，动态修改render type不要清除属性。
// 		   // 1. 属性可能共用一个key，如render.image=，即使换了type，属性也可以共享
// 		   // 2. 要实现undo/redo，不能丢掉属性
// 		   if (data.pUIApplication->IsDesignMode())
// 			   data.SetErase(false);
// 
// 		   m_pBkgndRender->Serialize(&data);
// 	   }
// 
// 	   // 前景属性
// 	   if (m_pForegndRender)
// 	   {
// 		   SERIALIZEDATA data(*pData);
// 		   data.szParentKey = XML_FOREGND_RENDER_PREFIX XML_RENDER_TYPE;
// 		   data.szPrefix = XML_FOREGND_RENDER_PREFIX;
// 		   if (data.pUIApplication->IsDesignMode())
// 			   data.SetErase(false);
// 
// 		   m_pForegndRender->Serialize(&data);
// 	   }
    }

	// 布局属性
	if (m_pLayoutParam)
	{
		m_pLayoutParam->Serialize(pData);
	}
	else if (pData->IsReload())
	{
		CreateLayoutParam();
	}

	
}

void  Object::virtualOnLoad()
{
    // 等对象在树中位置确定了，相关属性确定了之后，再创建layer
	if (m_objStyle.layer && !GetSelfLayer())
	{
		m_objLayer.CreateLayer();
	}
}