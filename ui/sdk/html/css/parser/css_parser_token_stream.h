#ifndef _UI_SDK_HTML_CSS_PARSER_CSSPARSERTOKENSTREAM_H_
#define _UI_SDK_HTML_CSS_PARSER_CSSPARSERTOKENSTREAM_H_

#include "html/css/parser/css_parser_token.h"

#include <cstdlib>
#include <cwchar>
#include <string>
#include <stack>
//
// https://www.w3.org/TR/css-syntax-3/
//

namespace html {

// 先不考虑unicode

class CSSTokenizerInputStream {
public:
  explicit CSSTokenizerInputStream(const char16_t *buffer, unsigned int size)
      : m_buffer(buffer), m_size(size) {
    // TODO: Preprocessing the input stream
    // https://www.w3.org/TR/css-syntax-3/#input-preprocessing
    //
    // \r U+000D CARRIAGE RETURN (CR) -> \n U+000A LINE FEED (LF)
    // \f U+000C FORM FEED (FF) -> \n U+000A LINE FEED (LF)
    // \0 U+0000 NULL -> U+FFFD
  }

  char16_t NextInputCodePoint() const {
    if (m_offset >= m_size) {
      return 0;
    }
    char16_t c = m_buffer[m_offset];

    // 按照预处理的要求，需要将 U+0000 NULL 替换成 U+FFFD REPLACEMENT CHARACTER 
    // 防止空字符截断或基于空字符的攻击。
    return c ? c : 0xFFFD;
  }

  void Advance(unsigned int offset = 1) {
    m_offset += offset;
  }
  void MoveBack() {
    m_offset--;
  }

  inline bool IsASCIIDigit(char16_t c) {
    return c >= '0' && c <= '9';
  }
  unsigned int NextDigit(unsigned int* length = nullptr) {
    char buffer[12] = { 0 };
    int i = 0;

    while ((m_offset) < m_size) {
      char16_t c = m_buffer[m_offset];
      if (!IsASCIIDigit(c)) {
        break;
      }
      if (i < 12) {
        buffer[i] = (char)c;
        ++i;
      }
      ++m_offset;
    }
    // 4,294,967,295
    if (i > 10) {
      if (length) *length = 0;
      return 0;
    }
    if (length) *length = i;

    char* end = buffer + i;
    return strtoul(buffer, &end, 10);
  }

  void AdvanceUntilNonWhitespace() {
    while (m_offset < m_size && IsWhiteSpace(m_buffer[m_offset])) {
      ++m_offset;
    }
  }

  char16_t Peek(unsigned int lookahead_offset = 0) const {
    if (m_offset + lookahead_offset >= m_size) {
      return 0;
    }
    char16_t c = m_buffer[m_offset + lookahead_offset];
    return c ? c : 0xFFFD;
  }

private:
  bool IsWhiteSpace(char16_t c) {
    return c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\f';
  }
public:
  unsigned int Offset() {
    return m_offset;
  }
  std::u16string RangeAt(unsigned int start_offset, unsigned int size) {
    return std::u16string(m_buffer + start_offset, size);
  }
private:
  const char16_t* m_buffer = nullptr;
  unsigned int m_size = 0;
  unsigned int m_offset = 0;

#if 0
  // The last code point to have been consumed.
  char16_t m_current_input_code_point = 0;
#endif
};

class CSSTokenizer {
public:
  CSSTokenizer(const char16_t* buffer, int size) : m_input_stream(buffer, size) {}

  struct NextTokenConfig {
    bool skip_comments;
  };
  CSSParserToken NextToken(NextTokenConfig config);

private:
  char16_t Consume();
  bool ConsumeIfNext(char16_t);
  void Reconsume(char16_t c);

  CSSParserToken ConsumeStringTokenUntil(char16_t c);
  CSSParserToken ConsumeNumericToken();
  CSSParserToken ConsumeNumber();
  CSSParserToken ConsumeIdentLikeToken();
  std::u16string ConsumeAnIdentSequence();
  char16_t ConsumeEscape();
  void ConsumeSingleWhitespaceIfNext();

  CSSParserToken BlockStart(CSSParserTokenType type);
  CSSParserToken BlockStart(CSSParserTokenType block_type,
                            CSSParserTokenType type,
                            std::u16string name /*, CSSValueID id*/);
  CSSParserToken BlockEnd(CSSParserTokenType type,
                          CSSParserTokenType start_type);
  CSSParserToken ConsumeUrlToken();
  CSSParserToken Hash(char16_t);

  void ConsumeUntilCommentEndFound();
  void ConsumeBadUrlRemnants();
  CSSParserToken LetterU(char16_t);
  bool NextCharsAreNumber(char16_t first);
  bool WouldStartAnIdentifier();

  bool TwoCharsAreValidEscape(char16_t first, char16_t second);
  bool CheckIfThreeCodePointsWouldStartAnIdentifier(char16_t first);
  
private:
  CSSTokenizerInputStream m_input_stream;
  std::stack<CSSParserTokenType> m_block_stack;
};

class CSSParserTokenStream {
public:
  CSSParserTokenStream(const char16_t* buffer, size_t size);
};

}
#endif