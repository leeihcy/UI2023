#include "include/interface/ilayout.h"
#include "object.h"

#include "include/interface/imapattr.h"
#include "include/interface/irenderbase.h"
#include "include/interface/itextrenderbase.h"
#include "include/interface/iuiapplication.h"
#include "src/attribute/attribute.h"
#include "src/attribute/bool_attribute.h"
#include "src/attribute/enum_attribute.h"
#include "src/attribute/stringselect_attribute.h"

namespace ui {
// 将xml用的配置转成对象的属性，注意，子类重载该函数时，必须先调用父类的该方法
// bReload表示为换肤时调用，此时不能再给某些属性赋值，例如text属性
void Object::onSerialize(SerializeParam *pData) {
  IMapAttribute *pMapAttrib = pData->pMapAttrib;
  if (pData->IsReload()) {
    m_back_render.reset();
    m_fore_render.reset();
    m_text_render.reset();
    // SAFE_RELEASE(m_pCursor);
  }

  if (pData->IsLoad()) {
    if (nullptr == pMapAttrib) {
      UI_LOG_ERROR("Invalid Argument.");
      return;
    }
  }

  {
    AttributeSerializer s(pData, "Object");
    s.AddString(XML_ID, m_strId)->AsData();

// #ifdef EDITOR_MODE
//     s.AddString(
//          XML_STYLECLASS,
//          [this](const char *t) {
//            if (t) {
//              m_strStyle = t;
//            } else {
//              m_strStyle.clear();
//            }
//          },
//          [this]() -> const char * { return m_strStyle.c_str(); })
//         ->ReloadOnChanged();
// #endif

    // styelclass被修改时，应该重新解析所有属性
    // s.AddString(XML_STYLECLASS, m_strStyleClass)->ReloadOnChanged();
    // s.AddString(XML_TOOLTIP, m_strTooltip);

    s.AddRect(XML_MARGIN, Slot(&Object::LoadMargin, this),
              Slot(&Object::SaveMargin, this));
    s.AddRect(XML_PADDING, Slot(&Object::LoadPadding, this),
              Slot(&Object::SavePadding, this));
    s.AddRect(XML_BORDER, Slot(&Object::LoadBorder, this),
              Slot(&Object::SaveBorder, this));

    s.AddEnum(XML_VISIBLE, Slot(&Object::LoadVisibleEx, this),
              Slot(&Object::SaveVisibleEx, this))
        ->AddOption(VISIBILITY_COLLAPSED, XML_VISIBILITY_COLLAPSED)
        ->AddOption(VISIBILITY_VISIBLE, XML_VISIBILITY_VISIBLE)
        ->AddOption(VISIBILITY_HIDDEN, XML_VISIBILITY_HIDDEN)
        ->SetDefault(VISIBILITY_VISIBLE)
        ->AsData();

    s.AddBool(XML_DISABLE, Slot(&Object::SetDisableDirect, this),
              Slot(&Object::IsSelfDisable, this))
        ->AsData();

    s.AddBool(XML_BACKGND_IS_TRANSPARENT, Slot(&Object::SetTransparent, this),
              Slot(&Object::IsTransparent, this))
        ->SetDefault(m_objStyle.default_transparent);

    s.AddBool(XML_NO_CLIP_DRAW, Slot(&Object::SetNoClip, this),
              Slot(&Object::IsNoClip, this));

    s.AddBool(XML_REJEST_MOUSE_MSG, Slot(&Object::SetRejectMouseMsgAll, this),
              Slot(&Object::IsRejectMouseMsgAll, this))
        ->SetDefault(m_objStyle.default_reject_all_mouse_msg);
    s.AddBool(XML_REJEST_SELF_MOUSE_MSG,
              Slot(&Object::SetRejectMouseMsgSelf, this),
              Slot(&Object::IsRejectMouseMsgSelf, this))
        ->SetDefault(m_objStyle.default_reject_self_mouse_msg);

    s.AddBool(XML_ISNCCHILD, Slot(&Object::SetAsNcObject, this),
              Slot(&Object::IsNcObject, this))
        ->SetDefault(m_objStyle.default_ncobject);

    s.AddBool(XML_LAYER, Slot(&Object::load_layer_config, this),
              Slot(&Object::HasLayer, this));

    // 	    s.AddString(XML_CURSOR, this,
    // 		    memfun_cast<pfnStringSetter>(&Object::SetCursorId),
    // 		    memfun_cast<pfnStringGetter>(&Object::SaveCursorId));

    // 设置背景渲染器
    s.AddRenderBase(XML_BACKGND_RENDER_PREFIX, this, m_back_render);
    // 设置前景绘制
    s.AddRenderBase(XML_FOREGND_RENDER_PREFIX, this, m_fore_render);

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
    // 	   if (m_back_render)
    // 	   {
    // 		   SerializeParam data(*pData);
    // 		   data.szParentKey = XML_BACKGND_RENDER_PREFIX XML_RENDER_TYPE;
    // 		   data.szPrefix = XML_BACKGND_RENDER_PREFIX;
    //
    // 		   // 在editor中，动态修改render type不要清除属性。
    // 		   // 1.
    // 属性可能共用一个key，如render.image=，即使换了type，属性也可以共享
    // 		   // 2. 要实现undo/redo，不能丢掉属性
    // 		   if (data.pUIApplication->IsDesignMode())
    // 			   data.SetErase(false);
    //
    // 		   m_back_render->Serialize(&data);
    // 	   }
    //
    // 	   // 前景属性
    // 	   if (m_fore_render)
    // 	   {
    // 		   SerializeParam data(*pData);
    // 		   data.szParentKey = XML_FOREGND_RENDER_PREFIX XML_RENDER_TYPE;
    // 		   data.szPrefix = XML_FOREGND_RENDER_PREFIX;
    // 		   if (data.pUIApplication->IsDesignMode())
    // 			   data.SetErase(false);
    //
    // 		   m_fore_render->Serialize(&data);
    // 	   }
  }

  layout.onSerialize(pData);
}

void Object::virtualOnLoad() {
  // 等对象在树中位置确定了，相关属性确定了之后，再创建layer
  if (m_objStyle.layer && !GetSelfLayer()) {
    m_objLayer.CreateLayer();
  }
}

} // namespace ui