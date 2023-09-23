#ifndef _UI_SVG_SRC_SVG_SVG_DOM_H_
#define _UI_SVG_SRC_SVG_SVG_DOM_H_

#include "3rd/pugixml/pugixml.hpp"
#include <map>
#include <memory>

namespace svg {
class Element;
class Svg;

class Dom {
public:
  Dom(/*Svg& svg*/);
  std::unique_ptr<svg::Svg> Parse(const char *stream);

private:
  std::unique_ptr<Element> handle_element(pugi::xml_node &node);
  void scan_node(Element *parent, pugi::xml_node &node);

private:
  // Svg& m_svg;
};

} // namespace svg
#endif