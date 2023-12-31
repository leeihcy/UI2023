#include "dom.h"
#include "src/element/circle/circle.h"
#include "src/element/ellipse/ellipse.h"
#include "src/element/group/group.h"
#include "src/element/line/line.h"
#include "src/element/path/path.h"
#include "src/element/polyline/polyline.h"
#include "src/element/rect/rect.h"
#include "src/element/svg/svg.h"
#include "src/element/use/use.h"
#include "src/element/text/text.h"

#include "3rd/pugixml/pugixml.hpp"
#include "sdk/include/interface/imapattr.h"
#include "sdk/include/macro/helper.h"
#include "sdk/include/uiapi.h"

namespace svg {

Dom::Dom() {} // ui::Svg &svg) : m_svg(svg) {}

void create_attribute_map(pugi::xml_node &node, ui::IMapAttribute *pMapAttrib) {
  for (pugi::xml_attribute attr = node.first_attribute(); !attr.empty();
       attr = attr.next_attribute()) {
    pMapAttrib->AddAttr(attr.name(), attr.as_string());
  }
}

void Dom::scan_node(Element *parent, pugi::xml_node &node) {
  pugi::xml_node child = node.first_child();
  while (child) {
    if (child.type() == pugi::node_comment) {
      child = child.next_sibling();
      continue;
    }

    // 如 <text>SVG</text> 通过data设置text内容
    if (child.type() == pugi::node_pcdata) {
      parent->SetXmlNodeData(child.value());
      break;
    }

    std::unique_ptr<Element> e = handle_element(child);
    if (e && parent) {
      parent->AddChild(std::move(e));
    }
    child = child.next_sibling();
  }
}

Element *create_element(const char *name) {
  Element *element = nullptr;
  if (strcmp("svg", name) == 0) {
    element = new svg::Svg();
  } else if (strcmp("rect", name) == 0) {
    element = new svg::Rect();
  } else if (strcmp("line", name) == 0) {
    element = new svg::Line();
  } else if (strcmp("ellipse", name) == 0) {
    element = new svg::Ellipse();
  } else if (strcmp("circle", name) == 0) {
    element = new svg::Circle();
  } else if (strcmp("text", name) == 0) {
    element = new svg::Text();
  } else if (strcmp("polyline", name) == 0) {
    element = new svg::Polyline();
  } else if (strcmp("polygon", name) == 0) {
    Polyline *p = new svg::Polyline();
    p->AsPolygon();
    element = p;
  } else if (strcmp("path", name) == 0) {
    element = new svg::Path();
  } else if (strcmp("g", name) == 0) {
    element = new svg::Group();
  } else if (strcmp("use", name) == 0) {
    element = new svg::Use();
  }
  else {
    UIASSERT(false);
  }
  return element;
}

std::unique_ptr<Element> Dom::handle_element(pugi::xml_node &node) {
  std::unique_ptr<Element> element(create_element(node.name()));
  if (element == nullptr) {
    return nullptr;
  }

  std::shared_ptr<ui::IMapAttribute> pMapAttrib = ui::UICreateIMapAttribute();
  create_attribute_map(node, pMapAttrib.get());

  ui::SerializeParam data = {0};
  data.nFlags = ui::SERIALIZEFLAG_LOAD;
  data.pMapAttrib = pMapAttrib.get();
  element->SetAttribute(data);

  scan_node(element.get(), node);

  return element;
}

std::unique_ptr<svg::Svg> Dom::Parse(const char *stream) {
  pugi::xml_document doc;
  if (pugi::status_ok !=
      doc.load_buffer((void *)stream, strlen(stream)).status) {
    return nullptr;
  }

  pugi::xml_node svg_node = doc.child("svg");
  if (svg_node.empty()) {
    return nullptr;
  }

  // 加载子结点
  std::unique_ptr<svg::Element> root = handle_element(svg_node);
  return std::unique_ptr<svg::Svg>(static_cast<svg::Svg *>(root.release()));
}

} // namespace svg