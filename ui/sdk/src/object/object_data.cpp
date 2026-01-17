#include "src/object/object_data.h"
#include "html/css/parser/css_parser.h"
#include "include/interface/iattributemap.h"
#include <memory>

namespace ui {

bool ObjectData::ParseInlineStyle(const char* style) {
  if (m_inline_style_parsed) {
    return true;
  }
  if (!style || !style[0]) {
    return true;
  }

  html::CSSParser parser;
  m_inline_style = parser.ParseInlineStyleDeclaration(style, strlen(style));
  return true;
}

ObjectDataCache& ObjectDataCache::Get() {
  static ObjectDataCache s;
  return s;
}

std::shared_ptr<ObjectData> ObjectDataCache::GetCache(IMeta& meta, IAttributeMap* attr_map) {
  // TODO: 
  return std::make_shared<ObjectData>();
}
}