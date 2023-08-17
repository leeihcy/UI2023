#ifndef _UI_SVG_SRC_SVG_SVG_H_
#define _UI_SVG_SRC_SVG_SVG_H_

#include "sdk/include/interface/imessage.h"
#include "include/interface/isvg.h"

namespace ui { namespace svg {

class Svg : public ui::MessageProxy {
public:
  Svg(ISvg*);

  UI_BEGIN_MSG_MAP()
    UIMSG_ERASEBKGND(OnEraseBkgnd)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(Svg, IPanel)

private:
  void  OnEraseBkgnd(ui::IRenderTarget* pRenderTarget);
  
private:
  ISvg* m_pISvg;
};

}}

#endif