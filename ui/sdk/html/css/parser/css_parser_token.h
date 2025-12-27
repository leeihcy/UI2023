#ifndef _UI_SDK_HTML_CSS_PARSER_CSSPARSERTOKEN_H_
#define _UI_SDK_HTML_CSS_PARSER_CSSPARSERTOKEN_H_

#include <string>
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
  WhiteSpace,
  Comment,
  Delimiter,
  Ident,
  
  Comma,
  Colon,
  Column,
  Semicolon,

  PrefixMatch,
  SuffixMatch,
  DashMatch,
  SubstringMatch,
  IncludeMatch,
  
  Number,
  Dimension,
  Percent,

  LeftParenthesis,
  RightParenthesis,
  LeftBracket,
  RightBracket,
  LeftBrace,
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

enum CSSParserTokenBlockType {
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
  Hash,
  HashUnrestricted,
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
    t.m_unit = unit;
    return t;
  }
  static CSSParserToken MakePercentage(double number) {
    CSSParserToken t(CSSParserTokenType::Percent);
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
  const std::u16string& Unit() { return m_unit; }
  const std::u16string& Name() { return m_name; }
  const std::u16string& String() { return m_name; }
  CSSParserTokenBlockType BlockType() { return m_block_type; }

private:
  CSSParserTokenType m_type = CSSParserTokenType::Invalid;

  // 将带括号的token，拆分成两条token，start block + end block.
  CSSParserTokenBlockType m_block_type = CSSParserTokenBlockType::Not;

  // Number + Dimension + Percent
  double m_number = 0; 

  // Number
  NumericValueType m_numeric_type = NumericValueType::Integer;

  // Dimension
  std::u16string m_unit;

  // Ident
  std::u16string m_name;

  // type Delimiter
  char16_t m_delimiter = 0;

  // Hash
  HashTokenType m_hash_type = HashTokenType::Hash;
};
}

#endif