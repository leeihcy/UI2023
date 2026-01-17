#ifndef _UI_SDK_SRC_OBJECT_OBJECTDATA_H_
#define _UI_SDK_SRC_OBJECT_OBJECTDATA_H_
#include "html/css/parser/css_parser.h"

#include <map>

namespace ui {
struct IMeta;
struct IAttributeMap;

// 这里放置可以尽可能被同一个类型对象共享的数据，
// 如对象属性、inline 样式属性。
class ObjectData {
public:
  bool ParseInlineStyle(const char* style);

private:
  bool m_inline_style_parsed : 1 = false;

  // std::map<std::string, std::string> m_attributes;

  // style="xx" 内置样式
  std::unique_ptr<html::CSSPropertyValueSet> m_inline_style;
};

class ObjectDataCache {
public:
  static ObjectDataCache& Get();
  std::shared_ptr<ObjectData> GetCache(IMeta& meta, IAttributeMap* attr_map);
};

}

#endif