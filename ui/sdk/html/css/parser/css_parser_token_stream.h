#ifndef _UI_SDK_HTML_CSS_PARSER_CSSPARSERTOKENSTREAM_H_
#define _UI_SDK_HTML_CSS_PARSER_CSSPARSERTOKENSTREAM_H_

#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_tokenizer.h"
#include <cstddef>
#include <cstdlib>
#include <string>
#include <memory>

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
  CSSParserToken& Peek() { return m_next_token; }
  
  bool AtEnd();
  
  CSSParserToken Consume();
  void ConsumeWhitespace();
  CSSParserToken ConsumeIncludingWhitespace();

public:
  struct State {
    unsigned int offset;
  };
  State Save();
  void Restore(State);

public:
  static constexpr uint64_t FlagForTokenType(CSSParserTokenType token_type) {
    return 1ull << static_cast<uint64_t>(token_type);
  }

  class Boundary {
    // STACK_ALLOCATED();
  public:
    Boundary(CSSParserTokenStream &stream, CSSParserTokenType boundary_type)
        : m_variable(&stream.m_boundaries) {
      m_original_value = *m_variable;
      *m_variable = m_original_value | FlagForTokenType(boundary_type);
    }
    ~Boundary() { 
      if (m_variable) {
        *m_variable = m_original_value;
      }
    }
  private:
    uint64_t *m_variable = nullptr;
    uint64_t m_original_value = 0;
  };

private:
  std::u16string m_input_unicode;
  std::unique_ptr<CSSTokenizer> m_tokenizer;
  CSSParserToken m_next_token;

  // 用于手动临时设置一个界限，只读取到指定的位置即结束。例如：
  // "foo:red;bar:blue"
  //  {
  //    CSSParserTokenStream::Boundary boundary(stream, kSemicolonToken);
  //    stream.SkipUntilPeekedTypeIs<>();
  //    EXPECT_TRUE(stream.AtEnd());
  //  }
  //  此时将只读取到第一个分号。
  uint64_t m_boundaries = FlagForTokenType(CSSParserTokenType::Eof);
};


// A RAII helper that allows you to rewind the parser if needed
// (e.g., if parsing fails after you've already consumed a few tokens).
class CSSParserSavePoint {
  // STACK_ALLOCATED();

 public:
  explicit CSSParserSavePoint(CSSParserTokenStream& stream)
      : stream_(stream), savepoint_(stream.Save()) {}

  ~CSSParserSavePoint() {
    if (!released_) {
      // stream_.EnsureLookAhead();
      stream_.Restore(savepoint_);
    }
  }

  void Release() {
    released_ = true;
  }

 private:
  CSSParserTokenStream& stream_;
  CSSParserTokenStream::State savepoint_;
  bool released_ = false;
};

}
#endif