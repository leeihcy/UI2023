#ifndef _UI_SVG_SRC_ELEMENT_RECT_H_
#define _UI_SVG_SRC_ELEMENT_RECT_H_

#include "sdk/include/interface/imessage.h"
#include "include/interface/irect.h"


namespace ui { namespace svg {

class Rect : public ui::MessageProxy {
public:
  Rect(IRect*);

  UI_BEGIN_MSG_MAP()
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(Rect, IObject)

private:
  IRect* m_pIRect;
  
};

}}

#endif