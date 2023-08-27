#ifndef _UI_SVG_SRC_SVG_SVG_DOM_H_
#define _UI_SVG_SRC_SVG_SVG_DOM_H_

#include "3rd/pugixml/pugixml.hpp"
class Container;

namespace ui { namespace svg {
class Svg;

class SvgDom {
public:
  SvgDom(Svg& svg);
  bool Parse(const char* stream);

private:
  void handle_element(Container *parent, pugi::xml_node &node);
  void scan_node(Container *parent, pugi::xml_node &node);

private:
  Svg& m_svg;

};

}}
#endif