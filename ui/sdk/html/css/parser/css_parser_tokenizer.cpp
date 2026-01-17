#include "html/css/parser/css_parser_tokenizer.h"

#include "html/css/parser/css_parser_token.h"
#include "html/util/util.h"
#include <cassert>
#include <string>
#include <vector>
#include <math.h>

#define kEndOfFileMarker 0

namespace html {
namespace  {

// ASCII code point  
constexpr inline bool IsASCII(char16_t c) {
  return !(c & ~0x7F);
}

// letter
// An uppercase letter or a lowercase letter.
inline bool IsLetter(char16_t c) {
  return (c | 0x20) >= 'a' && (c | 0x20) <= 'z';
}

// https://www.w3.org/TR/css-syntax-3/#ident-start-code-point
// ident-start code point
// A letter, a non-ASCII code point, or U+005F LOW LINE (_).
bool IsIdentStartCodePoint(char16_t c) {
  return IsLetter(c) || c == '_' || !IsASCII(c);
}
inline bool IsCSSNewLine(char16_t cc) {
  // We check \r and \f here, since we have no preprocessing stage
  return (cc == '\r' || cc == '\n' || cc == '\f');
}

inline bool IsASCIIDigit(char16_t c) {
  return c >= '0' && c <= '9';
}
inline bool IsASCIIHexDigit(char16_t c) {
  return IsASCIIDigit(c) || ((c | 0x20) >= 'a' && (c | 0x20) <= 'f');
}

// https://www.w3.org/TR/css-syntax-3/#ident-code-point
bool IsIdentCodePoint(char16_t c) {
  // An ident-start code point, a digit, or U+002D HYPHEN-MINUS (-).
  return IsIdentStartCodePoint(c) || IsASCIIDigit(c) || c == '-';
}

inline bool IsHTMLSpace(char16_t character) {
  return character <= ' ' &&
         (character == ' ' || character == '\n' || character == '\t' ||
          character == '\r' || character == '\f');
}

static bool IsNonPrintableCodePoint(char16_t cc) {
  return (cc >= '\0' && cc <= '\x8') || cc == '\xb' ||
         (cc >= '\xe' && cc <= '\x1f') || cc == '\x7f';
}

} // namespace

char16_t CSSTokenizer::Consume() {
  char16_t c = m_input_stream.NextInputCodePoint();
  m_input_stream.Advance();
  return c;
} 
bool CSSTokenizer::ConsumeIfNext(char16_t c) {
  if (m_input_stream.Peek() == c) {
    m_input_stream.Advance();
    return true;
  }
  return false;
}
void CSSTokenizer::Reconsume(char16_t c) {
  m_input_stream.MoveBack();
}

void CSSTokenizer::ConsumeSingleWhitespaceIfNext() {
  // We check for \r\n and HTML spaces since we don't do preprocessing
  char16_t next = m_input_stream.Peek(0);
  if (next == '\r' && m_input_stream.Peek(1) == '\n') {
    m_input_stream.Advance(2);
  } else if (IsHTMLSpace(next)) {
    m_input_stream.Advance();
  }
}

//
// thirt_party/blink/renderer/core/css/parser/css_tokenizer.cc
// https://www.w3.org/TR/css-syntax-3/#tokenization
//
// repeatedly consume a token from input until an <EOF-token> is reached
//
CSSParserToken CSSTokenizer::NextToken(bool skip_comments) {
  
  do {
    m_prev_offset = m_input_stream.Offset();
    
    char16_t c = Consume();
    switch (c) {
    case 0:
      return CSSParserToken(CSSParserTokenType::Eof);

    case ' ':
    case '\t':
    case '\r':
    case '\n':
    case '\f': // 换页符 FormFeed
      m_input_stream.AdvanceUntilNonWhitespace();
      return CSSParserToken(CSSParserTokenType::Whitespace);

    case '\'':
    case '"':
      return ConsumeStringTokenUntil(c);

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      Reconsume(c);
      return ConsumeNumericToken();

    case '(':
      return BlockStart(CSSParserTokenType::LeftParenthesis);
    case ')':
      return BlockEnd(CSSParserTokenType::RightParenthesis,
                      CSSParserTokenType::LeftParenthesis);
    case '[':
      return BlockStart(CSSParserTokenType::LeftBracket);
    case ']':
      return BlockEnd(CSSParserTokenType::RightBracket,
                      CSSParserTokenType::LeftBracket);
    case '{':
      return BlockStart(CSSParserTokenType::LeftBrace);
    case '}':
      return BlockEnd(CSSParserTokenType::RightBrace,
                      CSSParserTokenType::LeftBrace);

    case '+':
    case '.':
      if (NextCharsAreNumber(c)) {
        Reconsume(c);
        return ConsumeNumericToken();
      }
      return CSSParserToken::MakeDelimiter(c);

    case '-': {
      if (NextCharsAreNumber(c)) {
        Reconsume(c);
        return ConsumeNumericToken();
      }
      if (m_input_stream.Peek(0) == '-' && m_input_stream.Peek(1) == '>') {
        m_input_stream.Advance(2);
        return CSSParserToken(CSSParserTokenType::CDC);
      }
      if (CheckIfThreeCodePointsWouldStartAnIdentifier(c)) {
        Reconsume(c);
        return ConsumeIdentLikeToken();
      }
      return CSSParserToken::MakeDelimiter(c);
    }

    case '*':
      if (ConsumeIfNext('=')) {
        // [attr*="value"]
        return CSSParserToken(CSSParserTokenType::SubstringMatch);
      }
      return CSSParserToken::MakeDelimiter('*');

    case '<':
      if (m_input_stream.Peek(0) == '!' && m_input_stream.Peek(1) == '-' &&
          m_input_stream.Peek(2) == '-') {
        m_input_stream.Advance(3);
        return CSSParserToken(CSSParserTokenType::CDO);
      }
      return CSSParserToken::MakeDelimiter('<');
    case ',':
      return CSSParserToken(CSSParserTokenType::Comma);
    case '/':
      if (ConsumeIfNext('*')) {
        ConsumeUntilCommentEndFound();
        if (skip_comments) {
          break; // continue to read next token.
        } else {
          return CSSParserToken(CSSParserTokenType::Comment);
        }
      }
      return CSSParserToken::MakeDelimiter(c);

    case '\\':
      if (TwoCharsAreValidEscape(c, m_input_stream.Peek(0))) {
        Reconsume(c);
        return ConsumeIdentLikeToken();
      }
      return CSSParserToken::MakeDelimiter(c);

    case ':':
      return CSSParserToken(CSSParserTokenType::Colon);
    case ';':
      return CSSParserToken(CSSParserTokenType::Semicolon);
    case '#':
      return Hash(c);

    case '^':
      if (ConsumeIfNext('=')) {
        return CSSParserToken(CSSParserTokenType::PrefixMatch);
      }
      return CSSParserToken::MakeDelimiter(c);

    case '$':
      if (ConsumeIfNext('=')) {
        return CSSParserToken(CSSParserTokenType::SuffixMatch);
      }
      return CSSParserToken::MakeDelimiter(c);
    case '|':
      if (ConsumeIfNext('=')) {
        return CSSParserToken(CSSParserTokenType::DashMatch);
      }
      if (ConsumeIfNext('|')) {
        return CSSParserToken(CSSParserTokenType::Column);
      }
      return CSSParserToken::MakeDelimiter(c);

    case '~':
      if (ConsumeIfNext('=')) {
        return CSSParserToken(CSSParserTokenType::IncludeMatch);
      }
      return CSSParserToken::MakeDelimiter(c);

    case '@':
      if (WouldStartAnIdentifier()) {
        return CSSParserToken(CSSParserTokenType::AtKeyword,
                              ConsumeAnIdentSequence());
      }
      return CSSParserToken::MakeDelimiter(c);

    case 'u':
    case 'U':
      return LetterU(c);

    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 11:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case '!':
    case '%':
    case '&':
    case '=':
    case '>':
    case '?':
    case '`':
    case 127:
      return CSSParserToken::MakeDelimiter(c);

    default:
      m_input_stream.MoveBack();
      return ConsumeIdentLikeToken();
    }
  } while(skip_comments);
  assert(false);
}

// https://www.w3.org/TR/css-syntax-3/#consume-string-token
CSSParserToken CSSTokenizer::ConsumeStringTokenUntil(char16_t ending_code_point) {

  // Strings without escapes get handled without allocations
  for (unsigned size = 0;; size++) {
    char16_t cc = m_input_stream.Peek(size);
    if (cc == ending_code_point) {
      unsigned start_offset = m_input_stream.Offset();
      m_input_stream.Advance(size + 1);
      return CSSParserToken(CSSParserTokenType::String, m_input_stream.RangeAt(start_offset, size));
    }
    if (IsCSSNewLine(cc)) {
      m_input_stream.Advance(size);
      return CSSParserToken(CSSParserTokenType::BadString);
    }
    if (cc == '\0' || cc == '\\') {
      break;
    }
  }

  std::u16string output;
  while (true) {
    char16_t cc = Consume();
    if (cc == ending_code_point || cc == kEndOfFileMarker) {
      return CSSParserToken(CSSParserTokenType::String, output);
    }
    if (IsCSSNewLine(cc)) {
      Reconsume(cc);
      return CSSParserToken(CSSParserTokenType::BadString);
    }
    if (cc == '\\') {
      if (m_input_stream.NextInputCodePoint() == kEndOfFileMarker) {
        continue;
      }
      if (IsCSSNewLine(m_input_stream.Peek(0))) {
        ConsumeSingleWhitespaceIfNext();  // This handles \r\n for us
      } else {
        output.push_back(ConsumeEscape());
      }
    } else {
      output.push_back(cc);
    }
  }
}

// http://www.w3.org/TR/css3-syntax/#starts-with-a-number
bool CSSTokenizer::NextCharsAreNumber(char16_t first) {
  char16_t second = m_input_stream.Peek();
  if (IsASCIIDigit(first)) {
    return true;
  }
  if (first == '+' || first == '-') {
    return ((IsASCIIDigit(second)) ||
            (second == '.' && IsASCIIDigit(m_input_stream.Peek(1))));
  }
  if (first == '.') {
    return (IsASCIIDigit(second));
  }
  return false;
}

// https://www.w3.org/TR/css-syntax-3/#consume-number
// https://www.w3.org/TR/css-syntax-3/#convert-string-to-number
//
// s·(i + f·10^-d)·10^te
//
CSSParserToken CSSTokenizer::ConsumeNumber() {
  struct NumberParts {
    int s = 1;  // sign
    unsigned int i = 0;  // integer
    unsigned int f = 0;  // fractional
    unsigned int d = 0;  // the number of f
    int t = 1;  // exponent single
    unsigned int e = 0;  // 指数 exponent

    double toDouble() {
      double value = i;
      if (f != 0) {
        value += f * pow(10.0, -(double)d);
      }
      if (s == -1) {
        value = -value;
      }
      if (e != 0) {
        if (t == 1) {
          value *= pow(10, e);
        } else {
          value *= pow(10, -(double)e);
        }
      }
      return value;
    }
  };
  NumberParts parts;

  NumericValueType type = NumericValueType::Integer;

  // 符号部分
  char16_t next = m_input_stream.Peek(0);
  if (next == '+') {
    m_input_stream.Advance();
  } else if (next == '-') {
    m_input_stream.Advance();
    parts.s = -1;
  }
  // 整数部分
  parts.i += m_input_stream.NextDigit();

  // 小数部分
  next = m_input_stream.Peek();
  if (next == '.') {
     next = m_input_stream.Peek(1);
     if (IsASCIIDigit(next)) {
      m_input_stream.Advance();

      type = NumericValueType::Number;
      parts.f = m_input_stream.NextDigit(&parts.d);

      next = m_input_stream.Peek();
     }
  }

  // 科学计数法
  // 1.3e2 = 1.3 × 10²
  if (next == 'E' || next == 'e') {
    next = m_input_stream.Peek(1);
    if (IsASCIIDigit(next)) {
      m_input_stream.Advance();

      type = NumericValueType::Number;
      parts.e = m_input_stream.NextDigit();

    } else if (next == '+' || next == '-') {
      if (next == '-') {
        parts.t = -1;
      }
      next = m_input_stream.Peek(2);
       if (IsASCIIDigit(next)) {
        m_input_stream.Advance(2);

        type = NumericValueType::Number;
        parts.e = m_input_stream.NextDigit();
       }
    }
  }

  // 数值转换
  return CSSParserToken::MakeNumber(type, parts.toDouble());
}

// https://www.w3.org/TR/css-syntax-3/#consume-numeric-token
CSSParserToken CSSTokenizer::ConsumeNumericToken() {
  // Consume a number and let number be the result.
  CSSParserToken token = ConsumeNumber();

  // If the next 3 input code points would start an ident sequence, then:
  if (WouldStartAnIdentifier()) {
    return CSSParserToken::MakeDimension(token.Number(), ConsumeAnIdentSequence());
  } else if (ConsumeIfNext('%')) {
    return CSSParserToken::MakePercentage(token.Number());
  }
  return token;
}

// https://www.w3.org/TR/css-syntax-3/#would-start-an-identifier
// 对应chromium中的NextCharsAreIdentifier
bool CSSTokenizer::WouldStartAnIdentifier()
{
  char16_t first = Consume();
  bool are_identifier = CheckIfThreeCodePointsWouldStartAnIdentifier(first);
  Reconsume(first);
  return are_identifier;
}
bool CSSTokenizer::CheckIfThreeCodePointsWouldStartAnIdentifier(char16_t first)
{
  char16_t second = m_input_stream.Peek(0);

  // ident-start code point Return true.
  if (IsIdentStartCodePoint(first)) {
    return true;
  }

  // U+005C REVERSE SOLIDUS (\)
  // If the first and second code points are a valid escape, return true
  if (TwoCharsAreValidEscape(first, second)) {
    return true;
  }

  if (first == '-') {
    // If the second code point is an ident-start code point 
    // or a U+002D HYPHEN-MINUS, 
    // or the second and third code points are a valid escape, return true.
    return IsIdentStartCodePoint(second) || second == '-' ||
           TwoCharsAreValidEscape(second, m_input_stream.Peek(1));
  }

  return false;
}

// https://www.w3.org/TR/css-syntax-3/#consume-escaped-code-point
char16_t CSSTokenizer::ConsumeEscape() {
  char16_t c = m_input_stream.NextInputCodePoint();
  m_input_stream.Advance();
  
  std::vector<char> hex_chars;
  hex_chars.reserve(6);

  if (IsASCIIHexDigit(c)) {
    unsigned consumed_hex_digits = 1;

    hex_chars.push_back(c);
    while (consumed_hex_digits < 6 &&
           IsASCIIHexDigit(m_input_stream.Peek(0))) {
      c = m_input_stream.NextInputCodePoint();
      m_input_stream.Advance();
      hex_chars.push_back(c);
      consumed_hex_digits++;
    };

    // If the next input code point is whitespace, consume it as well. 
    ConsumeSingleWhitespaceIfNext();
    
    char* end = &hex_chars[5];
    unsigned int code_point = strtoul(&hex_chars[0], &end, 16);

    if (code_point == 0 || (0xD800 <= code_point && code_point <= 0xDFFF) ||
        code_point > 0x10FFFF) {
      return 0xFFFD;
    }
    return code_point;
  }

  if (c == 0) {
    return 0xFFFD;
  }

  // Return the current input code point.
  // 注意：这里并不是像 \n 解析成 \n，而是直接返回 n
  return c;
}

// https://www.w3.org/TR/css-syntax-3/#consume-name
// 对应chromium中的 ConsumeName 
std::u16string CSSTokenizer::ConsumeAnIdentSequence() {
  /*
  Let result initially be an empty string.

  Repeatedly consume the next input code point from the stream:

  ident code point
    Append the code point to result.
  the stream starts with a valid escape
    Consume an escaped code point. Append the returned code point to result.
  anything else
    Reconsume the current input code point. Return result.
  */

  std::u16string buffer;
  buffer.reserve(8);

  char16_t c;
  while ((c = Consume())) {
    if (IsIdentCodePoint(c)) {
      buffer.push_back(c);
      continue; 
    }
    if (TwoCharsAreValidEscape(c, m_input_stream.Peek())) {
      // escape it!
      buffer.push_back(ConsumeEscape());
      continue;
    }
    break;
  }
  Reconsume(c);
  return buffer;
}

// https://www.w3.org/TR/css-syntax-3/#consume-ident-like-token
CSSParserToken CSSTokenizer::ConsumeIdentLikeToken() {
  // Consume an ident sequence, and let string be the result.
  std::u16string name = ConsumeAnIdentSequence();

  // 这里区分 
  // <function-token> url( "" ) 
  // 和
  // <url-token> url()  老的url写法，不推荐。
  //
  if (ConsumeIfNext('(')) {
    if (EqualIgnoringASCIICase(name.c_str(), u"url")) {
      // 去掉空白后，查看第一个第符是否为引号
      m_input_stream.AdvanceUntilNonWhitespace();

      char16_t next = m_input_stream.Peek(0);
      if (next != '"' && next != '\'') {
        return ConsumeUrlToken();
      }
    }
    return BlockStart(CSSParserTokenType::LeftParenthesis, 
      CSSParserTokenType::Function, 
      name/*, CssValueKeywordID(name)*/);
  }
  return CSSParserToken::MakeIdent(name);
}

// https://www.w3.org/TR/css-syntax-3/#consume-url-token
CSSParserToken CSSTokenizer::ConsumeUrlToken() {
  // Consume as much whitespace as possible.
  m_input_stream.AdvanceUntilNonWhitespace();

  // URL tokens without escapes get handled without allocations
  for (unsigned size = 0;; size++) {
    char16_t cc = m_input_stream.Peek(size);
    if (cc == ')') {
      unsigned start_offset = m_input_stream.Offset();
      m_input_stream.Advance(size + 1);
      return CSSParserToken(CSSParserTokenType::Url, m_input_stream.RangeAt(start_offset, size));
    }
    if (cc <= ' ' || cc == '\\' || cc == '"' || cc == '\'' || cc == '(' ||
        cc == '\x7f') {
      break;
    }
  }

  std::u16string result;
  result.reserve(8);
  while (true) {
    char16_t cc = Consume();
    if (cc == ')' || cc == kEndOfFileMarker) {
      return CSSParserToken(CSSParserTokenType::Url, result);
    }

    if (IsHTMLSpace(cc)) {
      m_input_stream.AdvanceUntilNonWhitespace();
      if (ConsumeIfNext(')') || m_input_stream.NextInputCodePoint() == kEndOfFileMarker) {
        return CSSParserToken(CSSParserTokenType::Url, result);
      }
      // 内容后面出现空格后，只能再接右括号，否则是BadUrl
      break;
    }

    if (cc == '"' || cc == '\'' || cc == '(' || IsNonPrintableCodePoint(cc)) {
      break;
    }

    if (cc == '\\') {
      if (TwoCharsAreValidEscape(cc, m_input_stream.Peek(0))) {
        result.push_back(ConsumeEscape());
        continue;
      }
      break;
    }

    result.push_back(cc);
  }

  ConsumeBadUrlRemnants();
  return CSSParserToken(CSSParserTokenType::BadUrl);
}

// "url( abc.png def)"
// 到d时，已经是bad url了，将剩下的也内容处理掉。
// Consume the remnants of a bad url
void CSSTokenizer::ConsumeBadUrlRemnants() {
  while (true) {
    char16_t cc = Consume();
    if (cc == ')' || cc == kEndOfFileMarker) {
      return;
    }
    if (TwoCharsAreValidEscape(cc, m_input_stream.Peek(0))) {
      ConsumeEscape();
    }
  }
}

CSSParserToken CSSTokenizer::BlockStart(CSSParserTokenType type) {
  m_block_stack.push(type);
  return CSSParserToken(type);
}
CSSParserToken CSSTokenizer::BlockStart(CSSParserTokenType block_type,
                                        CSSParserTokenType type,
                                        std::u16string name/*,
                                        CSSValueID id*/) {
  m_block_stack.push(block_type);
  return CSSParserToken(type, CSSParserTokenBlockType::Start, name);
  // return CSSParserToken(type, name, CSSParserTokenType::BlockStart,
  //                       static_cast<int>(id));
}
CSSParserToken CSSTokenizer::BlockEnd(CSSParserTokenType type,
                                      CSSParserTokenType start_type) {
  if (!m_block_stack.empty() && m_block_stack.top() == start_type) {
    m_block_stack.pop();
    return CSSParserToken(type, CSSParserTokenBlockType::End);
  }
  return CSSParserToken(type);
}

// 一直匹配到 */ 结束。
//
// TBD: 如果没匹配上 chromuim 仍会直接消费所有内容直到EOF，而不是 parse error.
// If the preceding paragraph ended by consuming an EOF code point, this is a parse error.
//
void CSSTokenizer::ConsumeUntilCommentEndFound() {
  char16_t c = Consume();
  while (c != 0) { // End of file 
    if (c != '*') {
      c = Consume();
      continue;
    }
    c = Consume();
    if (c == '/') {
      return;
    }
  }
}

// https://www.w3.org/TR/css-syntax-3/#starts-with-a-valid-escape
bool CSSTokenizer::TwoCharsAreValidEscape(char16_t first, char16_t second) {
  // If the first code point is not U+005C REVERSE SOLIDUS (\), return false.
  // Otherwise, if the second code point is a newline, return false.
  // Otherwise, return true.
  return first == '\\' && !IsCSSNewLine(second);
}

//
// The <unicode-range-token> was removed, as it was low value and occasionally actively harmful.
// (u+a { font-weight: bold; } was an invalid selector, for example...)
//
CSSParserToken CSSTokenizer::LetterU(char16_t cc) {
  Reconsume(cc);
  return ConsumeIdentLikeToken();
}

CSSParserToken CSSTokenizer::Hash(char16_t c) {
  char16_t next_char = m_input_stream.Peek(0);
  if (IsIdentCodePoint(next_char) ||
      TwoCharsAreValidEscape(next_char, m_input_stream.Peek(1))) {
    HashTokenType type = WouldStartAnIdentifier()
                             ? HashTokenType::Id
                             : HashTokenType::Unrestricted;
    return CSSParserToken::MakeHash(ConsumeAnIdentSequence(), type);
  }

  return CSSParserToken::MakeDelimiter(c);
}

}