#ifndef _UI_ICONTROL_H_
#define _UI_ICONTROL_H_

#include "iobject.h"

namespace ui
{
	interface ITextRenderBase;

// control style
// #define CONTROL_STYLE_GROUP             0x00002000    // 是否是一个新组的开始
// #define CONTROL_STYLE_OWNERDRAW         0x00004000    // 是否自绘
typedef struct tagControlStyle
{
	bool  group : 1;
	bool  ownerdraw : 1;
}ControlStyle;

class Control;
struct UIAPI_UUID(78EC44D9-137E-46A1-8ACA-0E12A209FF4F) IControl 
         : public IObject
{
   void  ModifyControlStyle(ControlStyle* add, ControlStyle* remove);
   bool  TestControlStyle(ControlStyle* test);
   bool  IsGroup();
   void  SetGroup(bool);

   const wchar_t*  GetToolTipText();
   void  SetToolTipText(const wchar_t* szText);

   ITextRenderBase*  CreateDefaultTextRender();
   ITextRenderBase*  GetTextRenderDefault();
   void  TryUpdateLayoutOnContentChanged();

   UI_DECLARE_INTERFACE(Control);
};

}

#endif  // _UI_ICONTROL_H_