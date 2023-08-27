#ifndef _UI_SVG_SRC_SVG_SVG_H_
#define _UI_SVG_SRC_SVG_SVG_H_

#include "sdk/include/interface/imessage.h"
#include "include/interface/isvg.h"
#include "src/element/container.h"


namespace ui { 
struct SerializeParam;

namespace svg {

class Svg : public ui::MessageProxy {
public:
  Svg(ISvg*);
  
  ISvg* GetISvg() { return m_pISvg; }

public:
  void onRouteMessage(Msg* msg);

  Container* AsRoot() { return &m_root; }

  bool Load(const char* stream);
  void SetAttribute(ui::SerializeParam& data);

protected:
  void onFinalConstruct();
  void onEraseBkgnd(ui::IRenderTarget* pRenderTarget);
  void onPaint(ui::IRenderTarget *rt);

private:
  ISvg* m_pISvg;
  Container m_root;

  int m_width = 0;
  int m_height = 0;

  // 将view box范围内的内容，显示到整个svg控件。
  Rect m_view_box;
};

}}

#endif