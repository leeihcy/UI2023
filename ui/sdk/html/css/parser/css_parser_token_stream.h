#ifndef _UI_SDK_HTML_CSS_PARSER_CSSPARSERTOKENSTREAM_H_
#define _UI_SDK_HTML_CSS_PARSER_CSSPARSERTOKENSTREAM_H_

#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_tokenizer.h"
#include <cstdlib>
#include <string>

//
// https://www.w3.org/TR/css-syntax-3/
//
// 输入： token list （调用Tokenizer）
// 输出： at-rules（特殊指令）, qualified rules（普通样式规则）, and/or declarations（样式声明）.
//
//  qualified rule = [select] + block{ list of declarations }
//  declaration = property or descriptor name : value  [!important]
// 
//  property对应普通样式，descriptor对应@rule。

namespace html {
class CSSTokenizer;

class CSSParserTokenStream {
public:
  CSSParserTokenStream();
  void SetInput(const char* buffer, size_t size);

  const CSSParserToken& Peek() const { return m_next_token; }

  CSSParserToken Consume();
  void ConsumeWhiteSpace();

private:
  std::u16string m_input_unicode;
  std::unique_ptr<CSSTokenizer> m_tokenizer;
  CSSParserToken m_next_token;
};

}
#endif