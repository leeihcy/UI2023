#include <cassert>
#include <cstdio>
#include <string>
#include <math.h>
#include "html/css/parser/css_parser.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_tokenizer.h"
#include "html/css/parser/css_parser_token_stream.h"
#include "html/css/property/css_value.h"
#include "html/util/util.h"

void test1_codepoint() {
  std::string utf8_text("你好😀🎉");
  std::u16string unicode;

  html::Utf8ToUnicode(utf8_text.c_str(), utf8_text.length(), unicode);
  // printf("%lu -> %lu\n", utf8_text.length(), unicode.length());
  assert(utf8_text.length() == 14);
  assert(unicode.length() == 6);
}


// https://www.w3.org/TR/css-syntax-3/#consume-comment
void test_tokenizer_4_3_2_Consume_comments() {
  {
    const std::u16string style = u"/* 12345 */";
    html::CSSTokenizer tokenizer(style.c_str(), style.length());
    assert(tokenizer.NextToken(false).Type() == html::CSSParserTokenType::Comment);
  }
  {
    const std::u16string style = u"/** 123*45 **/";
    html::CSSTokenizer tokenizer(style.c_str(), style.length());
    assert(tokenizer.NextToken(false).Type() == html::CSSParserTokenType::Comment);
  }
  {
    const std::u16string style = u"/* eof";

    // 没有匹配上注释结束时，所有内容都是Comment。
    {
      html::CSSTokenizer tokenizer(style.c_str(), style.length());
      assert(tokenizer.NextToken(false).Type() == html::CSSParserTokenType::Comment);
    }{
      html::CSSTokenizer tokenizer(style.c_str(), style.length());

      assert(tokenizer.NextToken(true).Type() == html::CSSParserTokenType::Eof);
    }
  }
}

bool equal(double d1, double d2) {
  return abs(d1-d2) < 0.000001;
}

// https://www.w3.org/TR/css-syntax-3/#consume-numeric-token
void test_tokenizer_4_3_3_Consume_a_numeric_token() {
  {
    std::tuple<std::u16string, uint64_t> datas[] = {
      {u"0", 0},
      {u"123456", 123456},
      {u"1234567890", 1234567890},
      {u"4294967295", 4294967295}, 
      {u"4294967296", 0},
      {u"12345678901234567890", 0},
      {u"00001", 1}
    };
    for (auto& [input, output] : datas) {
      html::CSSTokenizerInputStream input_stream(input.c_str(), input.length());
      unsigned int value = input_stream.NextDigit(); 
      assert(value == output);
    }
  }

  {
    std::tuple<std::u16string, double> datas[] = {
      {u"0", 0},
      {u"+123.456", 123.456},
      {u"-9001.0005", -9001.0005},
      {u"4294967295", 4294967295}, 
      {u"4294967296", 0},  // 目前每个part的数值范围仍然是2^32之内
      {u"123e2", 12300.0},
      {u"-0.00123e-2", -0.0000123},
      {u"00001", 1},
      {u".01", 0.01},
    };

    for (auto& [input, output] : datas) {
      html::CSSTokenizer tokenizer(input.c_str(), input.length());
      auto token = tokenizer.NextToken();
      assert(token.Type() == html::CSSParserTokenType::Number);
      assert(equal(token.Number(), output));
    }
  }

  {
    std::tuple<std::u16string, double, html::CSSPrimitiveValue::UnitType> datas[] = {
      {u"10px", 10, html::CSSPrimitiveValue::UnitType::kPixels},
      // {u"2.5rem", 2.5, u"rem"},
      // {u"0.5turn ", 0.5, u"turn"},  
      // {u"-15deg ", -15, u"deg"},
    };
    for (auto& [input, number, unit] : datas) {
      html::CSSTokenizer tokenizer(input.c_str(), input.length());
      auto token = tokenizer.NextToken();
      assert(token.Type() == html::CSSParserTokenType::Dimension);
      assert(equal(token.Number(), number));
      assert(token.GetUnitType() == unit);
    }
  }
  {
    std::u16string input(u"80%");
    html::CSSTokenizer tokenizer(input.c_str(), input.length());
    auto token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::Percentage);
    assert(equal(token.Number(), 80));
  }
}

// <ident-token>, <function-token>, <url-token>, or <bad-url-token>
void test_tokenizer_4_3_4_Consume_an_ident_like_token() {
  {
    std::u16string input(u"-->");
    html::CSSTokenizer tokenizer(input.c_str(), input.length());
    auto token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::CDC);
  }
  {
    std::tuple<std::u16string, std::u16string> datas[] = {
      {u"abc", u"abc"},
      {u"-abc", u"-abc"},
      {u"-a-b--c_d", u"-a-b--c_d"},
    };
    for (auto& [input, output] : datas) {
      html::CSSTokenizer tokenizer(input.c_str(), input.length());
      auto token = tokenizer.NextToken();
      assert(token.Type() == html::CSSParserTokenType::Ident);
      assert(token.Name() == output);
    }
  }
  {
    std::u16string input(u"abc('123')");
    html::CSSTokenizer tokenizer(input.c_str(), input.length());
    auto token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::Function);
    assert(token.BlockType() == html::CSSParserTokenBlockType::Start);
    assert(token.Name() == u"abc");
  }
  {
    std::u16string input(u"min(1,2)");
    html::CSSTokenizer tokenizer(input.c_str(), input.length());
    auto token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::Function);
    assert(token.BlockType() == html::CSSParserTokenBlockType::Start);
    assert(token.Name() == u"min");
  }
}

// <string-token> or <bad-string-token>.
void test_tokenizer_4_3_5_Consume_a_string_token() {
  {
    std::tuple<std::u16string,std::u16string> data[] = {
      {u"\"abcd\"", u"abcd"},
      {u"'123'", u"123"},
    };
    for (auto& [input, output] : data) {
      html::CSSTokenizer tokenizer(input.c_str(), input.length());
      auto token = tokenizer.NextToken();
      assert(token.Type() == html::CSSParserTokenType::String);
      assert(token.Name() == output);
    }
  }
  {
    // 没有右引号，直接EOF
    std::u16string input(u"\"abcd");
    html::CSSTokenizer tokenizer(input.c_str(), input.length());
    auto token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::String);
    assert(token.Name() == u"abcd");
  }
  {
    // 直接遇到换行，Bad
    std::u16string input(u"\"abcd\ndef");
    html::CSSTokenizer tokenizer(input.c_str(), input.length());
    auto token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::BadString);
  }
  {
    // \ + 换行 是合法的
    std::u16string input(u"\"abcd\\\ndef");
    html::CSSTokenizer tokenizer(input.c_str(), input.length());
    auto token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::String);
    assert(token.Name() == u"abcddef");
  }
}

// https://www.w3.org/TR/css-syntax-3/#consume-url-token
void test_tokenizer_4_3_6_Consume_a_url_token() {
 {
    // url后面需要紧跟左括号
    std::u16string input(u"url ( abc.png )");
    html::CSSTokenizer tokenizer(input.c_str(), input.length());
    auto token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::Ident);
    assert(token.Name() == u"url");
  }
  {
    std::u16string input(u"url( abc.png )");
    html::CSSTokenizer tokenizer(input.c_str(), input.length());
    auto token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::Url);
    assert(token.Name() == u"abc.png");
  }
  {
    // url content尾部，只能是空格接右括号，不能再接其它字符。否则认为是BadUrl。
    // 同时需要将右括号前的其它内容跳过。
    std::u16string input(u"url( abc.png def)");
    html::CSSTokenizer tokenizer(input.c_str(), input.length());
    auto token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::BadUrl);

    token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::Eof);
  }
  {
    std::u16string input(u"url(\"abc.png\")");
    html::CSSTokenizer tokenizer(input.c_str(), input.length());
    auto token = tokenizer.NextToken();
    assert(token.Type() == html::CSSParserTokenType::Function);
    assert(token.BlockType() == html::CSSParserTokenBlockType::Start);
    assert(token.Name() == u"url");
  }
}

// https://www.w3.org/TR/css-syntax-3/#consume-escaped-code-point
void test_tokenizer_4_3_7_Consume_an_escaped_code_point() {
  {
    // 如果十六进制转义序列后面紧跟一个空白符（whitespace），该空白符会被忽略（作为转义序列的终止符）。

    // CSS 中的转义有两种形式：
    //  十六进制转义：\ + 1-6位十六进制数字（如 \a、\00000a）
    //  特殊字符转义：\ + 特定字符（如 \\、\"）

    std::tuple<std::u16string,std::u16string> data[] = {
      {u"'\\n'", u"n"},   // 注：和其它语言不一样。
      {u"'\\\\'", u"\\"}, 
      {u"'\\a'", u"\n"}, 
      {u"'\\A'", u"\n"}, 

      {u"'\\2aG'", u"*G"}, 
      {u"'\\2a G'", u"*G"},  // 吃掉后面那个空格。
      {u"'\\00002a G'", u"*G"},  // 吃掉后面那个空格。
      {u"'\\00002A1 G'", u"*1 G"},
    };
    for (auto& [input, output] : data) {
      html::CSSTokenizer tokenizer(input.c_str(), input.length());
      auto token = tokenizer.NextToken();
      assert(token.Type() == html::CSSParserTokenType::String);
      assert(token.Name() == output);
    }
  }
}

void test_tokenizer_4_3_11_Consume_an_ident_sequence() {
  {
     std::tuple<std::u16string,std::u16string> data[] = {
      {u"background", u"background"},
      {u"back\\ground", u"background"},
      {u"back ground", u"back"},
    };
    for (auto& [input, output] : data) {
      html::CSSTokenizer tokenizer(input.c_str(), input.length());
      auto token = tokenizer.NextToken();
      assert(token.Type() == html::CSSParserTokenType::Ident);
      assert(token.Name() == output);
    }
  }
}

void test_html_css_tokenizer() {
  // test1_codepoint();
  // test_tokenizer_4_3_2_Consume_comments();
  // test_tokenizer_4_3_3_Consume_a_numeric_token();
  // test_tokenizer_4_3_4_Consume_an_ident_like_token();
  // test_tokenizer_4_3_5_Consume_a_string_token();
  // test_tokenizer_4_3_6_Consume_a_url_token();
  // test_tokenizer_4_3_7_Consume_an_escaped_code_point();
  test_tokenizer_4_3_11_Consume_an_ident_sequence();
}
