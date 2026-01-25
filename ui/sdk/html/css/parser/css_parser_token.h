#ifndef _UI_SDK_HTML_CSS_PARSER_CSSPARSERTOKEN_H_
#define _UI_SDK_HTML_CSS_PARSER_CSSPARSERTOKEN_H_

#include "html/css/property/value_id.h"
#include "html/css/property/css_value.h"
#include "html/css/parser/at_rule_descriptor_id.h"
#include "html/css/property/property_id.h"
#include <string>
#include <assert.h>

namespace html {

/*
 <ident-token>, <function-token>, <at-keyword-token>, <hash-token>,
 <string-token>, <bad-string-token>, <url-token>, <bad-url-token>,
 <delim-token>, <number-token>, <percentage-token>, <dimension-token>,
 <whitespace-token>, <CDO-token>, <CDC-token>, <colon-token>, <semicolon-token>,
 <comma-token>, <[-token>, <]-token>, <(-token>, <)-token>, <{-token>, and
 <}-token>.
*/
enum class CSSParserTokenType : int {
  Invalid = -1,
  Eof = 0,
  Whitespace,
  Comment,
  Delimiter,
  Ident,
  
  Comma,
  Colon,
  Column,
  Semicolon,

  PrefixMatch,    // ^=
  SuffixMatch,    // &=
  DashMatch,      // |=
  SubstringMatch, // *=
  IncludeMatch,   // ~=
  
  Number,
  Dimension,
  Percentage,

  LeftParenthesis,
  RightParenthesis,
  LeftBracket,
  RightBracket,
  LeftBrace, // {
  RightBrace,

  Hash,
  AtKeyword,
  Function,
  Url,
  BadUrl,
  String,
  BadString,

  CDO, // <!--
  CDC, // -->

};

enum class CSSParserTokenBlockType {
  Not,
  Start,
  End,
};

enum class NumericValueType : int {
  Integer,
  Number,
};
// enum class NumericSign : int {
//   NoSign,
//   Plus,
//   Minus,
// };

enum class HashTokenType {
  Id,  // 后面跟着ident
  Unrestricted,
};

class CSSParserToken {
public:
  CSSParserToken(CSSParserTokenType type) : m_type(type) {}
  CSSParserToken(CSSParserTokenType type, CSSParserTokenBlockType block_type)
      : m_type(type), m_block_type(block_type) {}
  CSSParserToken(CSSParserTokenType type, const std::u16string &name)
      : m_type(type), m_name(name) {}
  CSSParserToken(CSSParserTokenType type, CSSParserTokenBlockType block_type,
                 const std::u16string &name)
      : m_type(type), m_block_type(block_type), m_name(name) {}

  CSSParserTokenType Type() const { return m_type; }
  CSSParserTokenType GetType() const { return m_type; }
  bool IsEOF () const { return m_type == CSSParserTokenType::Eof; }
  
  CSSParserTokenBlockType GetBlockType() const { return m_block_type; }

  static CSSParserToken MakeIdent(const std::u16string& name) {
    CSSParserToken t(CSSParserTokenType::Ident);
    t.m_name = name;
    return t;
  }

  static CSSParserToken MakeNumber(NumericValueType numeric_type, double number) {
    CSSParserToken t(CSSParserTokenType::Number);
    t.m_numeric_type = numeric_type;
    t.m_number = number;
    return t;
  }
  static CSSParserToken MakeDimension(double number, const std::u16string& unit) {
    CSSParserToken t(CSSParserTokenType::Dimension);
    t.m_number = number;
    t.m_unit_type = CSSPrimitiveValue::StringToUnitType(unit);
    return t;
  }
  static CSSParserToken MakePercentage(double number) {
    CSSParserToken t(CSSParserTokenType::Percentage);
    t.m_number = number;
    return t;
  }
  static CSSParserToken MakeDelimiter(char16_t delimiter) {
    CSSParserToken t(CSSParserTokenType::Delimiter);
    t.m_delimiter = delimiter;
    return t;
  }
  static CSSParserToken MakeHash(const std::u16string& name, HashTokenType hash_type) {
    CSSParserToken t(CSSParserTokenType::Hash);
    t.m_name = name;
    t.m_hash_type = hash_type;
    return t;
  }
  
  double Number() { return m_number; }
  CSSPrimitiveValue::UnitType GetUnitType() const { return m_unit_type; }
  const std::u16string& Name() const { return m_name; }
  const std::u16string& String() const { return m_name; }
  CSSParserTokenBlockType BlockType() { return m_block_type; }
  char16_t Delimiter() const { return m_delimiter; }
  CSSValueID ValueId() const;
  CSSValueID FunctionId() const;
  HashTokenType GetHashTokenType() const { return m_hash_type; }

  double NumericValue() const {
    assert(m_type == CSSParserTokenType::Number || m_type == CSSParserTokenType::Percentage ||
         m_type == CSSParserTokenType::Dimension);
    return m_number;
  }
  AtRuleDescriptorID ParseAsAtRuleDescriptorID() const;
  CSSPropertyID ParseAsUnresolvedCSSPropertyID() const;

private:
  CSSParserTokenType m_type = CSSParserTokenType::Invalid;

  // 将带括号的token，拆分成两条token，start block + end block.
  CSSParserTokenBlockType m_block_type = CSSParserTokenBlockType::Not;

  // Number + Dimension + Percent
  double m_number = 0; 

  // Number
  NumericValueType m_numeric_type = NumericValueType::Integer;
  
  // Ident
  std::u16string m_name;

  // Dimension unit
  CSSPrimitiveValue::UnitType m_unit_type;

  // type Delimiter
  char16_t m_delimiter = 0;

  // Hash
  HashTokenType m_hash_type = HashTokenType::Id;
};
}

#endif