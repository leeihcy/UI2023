#include "svg_dom.h"
#include "src/element/circle/circle.h"
#include "src/element/ellipse/ellipse.h"
#include "src/element/line/line.h"
#include "src/element/rect/rect.h"
#include "svg.h"

#include "3rd/pugixml/pugixml.hpp"

namespace ui {
namespace svg {

SvgDom::SvgDom(Svg& svg) : m_svg(svg) {

}

ui::IMapAttribute * create_attribute_map(pugi::xml_node& node) {
  ui::IMapAttribute *pMapAttrib = ui::UICreateIMapAttribute();
  for (pugi::xml_attribute attr = node.first_attribute(); !attr.empty();
       attr = attr.next_attribute()) {
    pMapAttrib->AddAttr(attr.name(), attr.as_string());
  }
  return pMapAttrib;
}

void SvgDom::scan_node(Container *parent, pugi::xml_node &node) {
  pugi::xml_node child = node.first_child();
  while (child) {
    if (child.type() != pugi::node_comment) {
      handle_element(parent, child);
    }
    child = child.next_sibling();
  }
}

Element *create_element(const char *name) {
  Element *element = nullptr;
  if (strcmp("rect", name) == 0) {
    element = new ::Rect();
  } else if (strcmp("line", name) == 0) {
    element = new ::Line();
  } else if (strcmp("ellipse", name) == 0) {
    element = new ::Ellipse();
  } else if (strcmp("circle", name) == 0) {
    element = new ::Circle();
  }
  return element;
}

void SvgDom::handle_element(Container *parent, pugi::xml_node &node) {
  std::unique_ptr<Element> element(create_element(node.name()));
  if (element == nullptr) {
    return;
  }

  ui::IMapAttribute *pMapAttrib = create_attribute_map(node);
  
  ui::SerializeParam data = {0};
  data.pSkinRes = m_svg.GetISvg()->GetResource();
  data.nFlags = SERIALIZEFLAG_LOAD;
  data.pMapAttrib = pMapAttrib;
  element->SetAttribute(data);

  pMapAttrib->Destroy();


  // for (pugi::xml_attribute attr = node.first_attribute(); !attr.empty();
  //      attr = attr.next_attribute()) {
  //   element->SetAttribute(attr.name(), attr.as_string());
  // }

  if (parent) {
    parent->AddChild(std::move(element));
  }

  scan_node(nullptr, node);
}

bool SvgDom::Parse(const char* stream) {
  pugi::xml_document doc;
  if (pugi::status_ok !=
      doc.load_buffer((void *)stream, strlen(stream)).status) {
    return false;
  }

  pugi::xml_node svg_node = doc.child("svg");
  if (svg_node.empty()) {
    return false;
  }

  ui::IMapAttribute *pMapAttrib = create_attribute_map(svg_node);
  
  ui::SerializeParam data = {0};
  data.pSkinRes = m_svg.GetISvg()->GetResource();
  data.nFlags = SERIALIZEFLAG_LOAD;
  data.pMapAttrib = pMapAttrib;
  m_svg.SetAttribute(data);

  pMapAttrib->Destroy();

  // 加载子结点
  scan_node(m_svg.AsRoot(), svg_node);
  return true;
}

}
}