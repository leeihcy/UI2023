#ifndef _UI_SVG_SRC_SVG_SVG_H_
#define _UI_SVG_SRC_SVG_SVG_H_

#include "sdk/include/interface/imessage.h"
#include "include/interface/isvg.h"
#include "src/element/container.h"


namespace ui { namespace svg {

class Svg : public ui::MessageProxy {
public:
  Svg(ISvg*);
  
public:
  void onRouteMessage(Msg* msg);

  Container* AsRoot() { return &m_root; }

  bool Load(const char* stream);
  
private:
  void onEraseBkgnd(ui::IRenderTarget* pRenderTarget);
  void onFinalConstruct();

private:
  ISvg* m_pISvg;
  Container  m_root;
};

}}

#endif