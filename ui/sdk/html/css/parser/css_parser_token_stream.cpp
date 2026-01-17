#include "html/css/parser/css_parser_token_stream.h"
#include "html/css/parser/css_parser_token.h"
#include "html/util/util.h"
#include <memory>

namespace html {

CSSParserTokenStream::CSSParserTokenStream()
    : m_next_token(CSSParserTokenType::Invalid){};


bool CSSParserTokenStream::AtEnd() {
  // m_boundaries表示手动设置的读取终点，例如下一个分号。
  return (m_boundaries & FlagForTokenType(m_next_token.Type())) ||
           m_next_token.GetBlockType() == CSSParserTokenBlockType::End;
}

bool CSSParserTokenStream::TokenMarksEnd(CSSParserTokenType end_type) {
  return (m_boundaries & FlagForTokenType(m_next_token.Type()) ||
          m_next_token.GetBlockType() == CSSParserTokenBlockType::End ||
          m_next_token.Type() == end_type);
}
bool CSSParserTokenStream::TokenMarksEnd(const CSSParserToken &token,
                                         CSSParserTokenType end_type) {
  return (m_boundaries & FlagForTokenType(token.Type()) ||
          token.GetBlockType() == CSSParserTokenBlockType::End ||
          token.Type() == end_type);
}

CSSParserTokenStream::State CSSParserTokenStream::Save() {
  return {m_tokenizer->Offset()};
}
void CSSParserTokenStream::Restore(State state) {
  if (state.offset == m_tokenizer->Offset()) {
    return;
  }
  m_tokenizer->Restore(state.offset);
  m_next_token = m_tokenizer->NextToken();
}

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

void CSSParserTokenStream::ConsumeWhitespace() {
  while (Peek().Type() == CSSParserTokenType::Whitespace) {
    Consume();
  }
}

// consume之后，继续吃掉后面的空格。
CSSParserToken CSSParserTokenStream::ConsumeIncludingWhitespace() {
  CSSParserToken result = Consume();
  ConsumeWhitespace();
  return result;
}

// ??
void CSSParserTokenStream::SkipUntilPeekedTypeIs(CSSParserTokenType type) {
  if (m_next_token.IsEof() || TokenMarksEnd(type)) {
    return;
  }
  // Process the lookahead token.
  unsigned nesting_level = 0;
  if (m_next_token.GetBlockType() == CSSParserTokenBlockType::Start) {
    nesting_level++;
  }

  // Add tokens to our return vector until we see either EOF or we meet the
  // return condition. (The termination condition is within the loop.)
  while (true) {
    CSSParserToken token = m_tokenizer->NextToken();
    if (token.IsEof() || (nesting_level == 0 && TokenMarksEnd(token, type))) {
      m_next_token = token;
      // offset_ = tokenizer_.PreviousOffset();
      return;
    } else if (token.GetBlockType() == CSSParserTokenBlockType::Start) {
      nesting_level++;
    } else if (token.GetBlockType() == CSSParserTokenBlockType::End) {
      nesting_level--;
    }
  }
}

bool CSSParserTokenStream::SkipToEndOfBlock() {
  unsigned nesting_level = 1;
  if (m_next_token.GetBlockType() == CSSParserTokenBlockType::Start) {
    nesting_level++;
  } else if (m_next_token.GetBlockType() == CSSParserTokenBlockType::End) {
    nesting_level--;
  }

  // Skip tokens until we see EOF or the closing brace.
  while (nesting_level != 0) {
    CSSParserToken token = m_tokenizer->NextToken(true);
    if (token.IsEof()) {
      break;
    } else if (token.GetBlockType() == CSSParserTokenBlockType::Start) {
      nesting_level++;
    } else if (token.GetBlockType() == CSSParserTokenBlockType::End) {
      nesting_level--;
    }
  }
}

// CSSParserToken CSSParserTokenStream::NextInputToken() {
//   return m_tokenizer->NextToken();
// }

}