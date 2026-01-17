#ifndef _HTML_CSS_CSSSELECTOR_H_
#define _HTML_CSS_CSSSELECTOR_H_

#include <string>
namespace html {

class CSSSelector {
public:
  enum MatchType {
    Unknown,
    InvalidList,

    Tag,              // <div>
    UniversalTag,     // *
    Id,               // #id
    Class,            // .class
    PseudoClass,      // :nth-child(2)
    PseudoElement,    // ::first-line
    PageSeudoClass,   // @page :right

    AttributeExact,   // E[foo="bar"],  input[type="submit"]
    AttributeSet,     // E[foo],        只要这个属性存在
    AttributeHypen,   // E[foo|="bar"], 属性以指定值开头，后跟连字符-，或等于该值, div[class|="btn"]
    AttributeList,    // E[foo~="bar"], 属性值是以空格分隔的列表，其中包含指定值
    AttributeContain, // E[foo*="bar"], 属性值包含指定子字符串
    AttributeBegin,   // E[foo^="bar"], 属性值以指定字符串开头
    AttributeEnd,     // E[foo&="bar"], 属性值以指定字符串结尾
    FirstAttributeSelectorMatch = AttributeExact

  };

  enum class RelationType {
    // .btn.primary, 两个连着的样式，表示同时具备这两个类的元素。
    SubSelector,
    // "Space"
    Descendant,
    // >
    Child,
    // +
    DirectAdjacent,
    // ~
    IndirectAdjacent,
  };

public:
  void SetMatchType(MatchType t) { m_match_type = t; }
  MatchType GetMatchType() const { return m_match_type; }

  void SetValue(const std::u16string& value) { m_value = value; }
  
private:
  MatchType m_match_type;
  std::u16string m_value;
};

}

#endif