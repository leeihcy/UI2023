#include "html/css/parser/css_parser_token_stream.h"
#include "html/css/parser/css_parser_token.h"
#include "html/util/util.h"
#include <memory>

namespace html {

CSSParserTokenStream::CSSParserTokenStream()
    : m_next_token(CSSParserTokenType::Invalid){};

void CSSParserTokenStream::SetInput(const char* buffer, size_t size) {
  html::Utf8ToUnicode(buffer, size, m_input_unicode);
  m_tokenizer = std::make_unique<CSSTokenizer>(m_input_unicode.c_str(), m_input_unicode.length());

  m_next_token = m_tokenizer->NextToken();
}


CSSParserToken CSSParserTokenStream::Consume() {
  CSSParserToken token = m_next_token;
  m_next_token = m_tokenizer->NextToken();
  return token;
}

void CSSParserTokenStream::ConsumeWhiteSpace() {
  while (Peek().Type() == CSSParserTokenType::WhiteSpace) {
    Consume();
  }
}

// CSSParserToken CSSParserTokenStream::NextInputToken() {
//   return m_tokenizer->NextToken();
// }

}