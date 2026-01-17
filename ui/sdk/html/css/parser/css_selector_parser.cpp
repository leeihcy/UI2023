#include "html/css/parser/css_selector_parser.h"
#include "html/base/atomic_string.h"
#include "html/css/css_selector.h"
#include "html/css/parser/css_parser.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_token_stream.h"
#include <string>

namespace html {

// static
void CSSSelectorParser::ConsumeSelector(CSSParserContext &context, StyleSheetContents* style_sheet,
                                        std::vector<A<CSSSelector>> &result) {
  CSSSelectorParser parser(style_sheet);
  context.token_stream.ConsumeWhitespace();

  ResultFlags result_flags = 0;
  parser.ConsumeComplexSelectorList(context, result, result_flags);
}

void CSSSelectorParser::ConsumeComplexSelectorList(
    CSSParserContext &context, std::vector<A<CSSSelector>> &result, ResultFlags& result_flags) {
  auto& stream = context.token_stream;

  ConsumeComplexSelector(context, result, result_flags);
  if (result.empty()) {
    return;
  }

  while (!stream.AtEnd() && stream.Peek().GetType() == CSSParserTokenType::Comma) {
    stream.ConsumeWhitespace();
    int size = result.size();
    ConsumeComplexSelector(context, result, result_flags);
    if (size == result.size()) {
      break;
    }
  }

  return;
}

// static 
CSSSelector::RelationType ConsoumeCombinator(CSSParserContext& context) {
  auto& stream = context.token_stream;
  CSSSelector::RelationType fallback_result = CSSSelector::RelationType::SubSelector;

  // 空格，表示子孙结点关系。
  while (stream.Peek().GetType() == CSSParserTokenType::Whitespace) {
    stream.Consume();
    fallback_result = CSSSelector::RelationType::Descendant;
  }

  if (stream.Peek().GetType() != CSSParserTokenType::Delimiter) {
    return fallback_result;
  }

  switch (stream.Peek().Delimiter()) {
  case '+':
    stream.ConsumeIncludingWhitespace();
    return CSSSelector::RelationType::DirectAdjacent;

  case '~':
    stream.ConsumeIncludingWhitespace();
    return CSSSelector::RelationType::IndirectAdjacent;

  case '>':
    stream.ConsumeIncludingWhitespace();
    return CSSSelector::RelationType::Child;

  default:
    break;
  }

  return fallback_result;
}

void CSSSelectorParser::ConsumeComplexSelector(
  CSSParserContext& context, std::vector<A<CSSSelector> >& result, ResultFlags& result_flags) {

  std::vector<A<CSSSelector> > compound_selector;
  ConsumeCompoundSelector(context, compound_selector, result_flags);
  if (compound_selector.empty()) {
    return;
  }

  std::reverse(compound_selector.begin(), compound_selector.end());

  CSSSelector::RelationType combinator = ConsoumeCombinator(context);
  if (combinator != CSSSelector::RelationType::SubSelector) {
    result_flags |= (int)kContainsComplexSelector; 

  }
}

bool CSSSelectorParser::ConsumeId(CSSParserContext& context) {
  auto& stream = context.token_stream;

  if (stream.Peek().GetHashTokenType() != HashTokenType::Id) {
    return false;
  }
  CSSSelector selector;
  selector.SetMatchType(CSSSelector::Id);
  selector.SetValue(stream.Consume().Name());
  m_output.push_back(std::move(selector));
  return true;
}

bool CSSSelectorParser::ConsumeClass(CSSParserContext& context) {
  auto& stream = context.token_stream;

  assert(stream.Peek().GetType() == CSSParserTokenType::Delimiter);
  assert(stream.Peek().Delimiter() == '.');

  stream.Consume();
  if (stream.Peek().GetType() != CSSParserTokenType::Ident) {
    return false;
  }

  CSSSelector selector;
  selector.SetMatchType(CSSSelector::Class);
  selector.SetValue(stream.Consume().Name());
  m_output.push_back(std::move(selector));
  return true;
}

const AtomicString CSSSelectorParser::DefaultNamespace() const {
  // While this flag is true, the default namespace is ignored. In other words,
  // the default namespace is '*' while this flag is true.
  bool ignore_default_namespace_ = false;

  if (!m_style_sheet || ignore_default_namespace_) {
    return g_star_atom;
  }
  return m_style_sheet->DefaultNamespace();
}

const AtomicString CSSSelectorParser::DetermineNamespace(
    const AtomicString& prefix) {
  if (prefix.IsNull()) {
    return DefaultNamespace();
  }
  if (prefix.text().empty()) {
    return g_empty_atom;  // No namespace. If an element/attribute has a
                          // namespace, we won't match it.
  }
  if (prefix == g_star_atom) {
    return g_star_atom;  // We'll match any namespace.
  }
  if (!m_style_sheet) {
    return g_null_atom;  // Cannot resolve prefix to namespace without a
                         // stylesheet, syntax error.
  }
  return m_style_sheet->NamespaceURIFromPrefix(prefix);
}


bool CSSSelectorParser::ConsumeAttribute(CSSParserContext &context) {
  auto& stream = context.token_stream;
  assert(stream.Peek().GetType() == CSSParserTokenType::LeftBracket);

  CSSParserTokenStream::BlockGuard guard(stream);
  stream.ConsumeWhitespace();

  AtomicString namespace_prefix;
  AtomicString attribute_name;
  if (!ConsumeName(context, attribute_name, namespace_prefix)) {
    return false;
  }
  if (attribute_name == g_null_atom) {
    return false;
  }
  stream.ConsumeWhitespace();

  // if (context_->IsHTMLDocument()) {
  //   attribute_name = attribute_name.LowerASCII();
  // }

  AtomicString namespace_uri = DetermineNamespace(namespace_prefix);
  if (namespace_uri.IsNull()) {
    return false;
  }

#if 0
  QualifiedName qualified_name =
      namespace_prefix.IsNull()
          ? QualifiedName(attribute_name)
          : QualifiedName(namespace_prefix, attribute_name, namespace_uri);

  if (stream.AtEnd()) {
    CSSSelector selector(CSSSelector::kAttributeSet, qualified_name,
                         CSSSelector::AttributeMatchType::kCaseSensitive);
    output_.push_back(std::move(selector));
    return true;
  }

  CSSSelector::MatchType match_type = ConsumeAttributeMatch(stream);

  CSSParserToken attribute_value = stream.Peek();
  if (attribute_value.GetType() != kIdentToken &&
      attribute_value.GetType() != kStringToken) {
    return false;
  }
  stream.ConsumeIncludingWhitespace();
  CSSSelector::AttributeMatchType case_sensitivity =
      ConsumeAttributeFlags(stream);
  if (!stream.AtEnd()) {
    return false;
  }

  CSSSelector selector(match_type, qualified_name, case_sensitivity,
                       attribute_value.Value().ToAtomicString());
  output_.push_back(std::move(selector));
#endif
  return true;
}

bool CSSSelectorParser::ConsumePseudo(CSSParserContext &context, ResultFlags &result_flags) {
  assert(false);
  return false;
}

bool CSSSelectorParser::ConsumeNestinParent(CSSParserContext &context, ResultFlags& result_flags) {
  assert(false);
  return false;
}

bool CSSSelectorParser::ConsumeSimpleSelector(CSSParserContext& context, ResultFlags& result_flags) {
  auto& stream = context.token_stream;
  ResultFlags local_result_flags = 0;
  const CSSParserToken& token = stream.Peek();
  bool ok = false;
  if (token.GetType() == CSSParserTokenType::Hash) {
    ok = ConsumeId(context);
  } else if (token.GetType() == CSSParserTokenType::Delimiter && token.Delimiter() == '.') {
    ok = ConsumeClass(context);
  } else if (token.GetType() == CSSParserTokenType::LeftBracket) { // 中括号
    ok = ConsumeAttribute(context);
  } else if (token.GetType() == CSSParserTokenType::Colon) {
    ok = ConsumePseudo(context, local_result_flags);
    if (ok) {
      local_result_flags |= kContainsPseudo;
    }
  } else if (token.GetType() == CSSParserTokenType::Delimiter && token.Delimiter() == '&') {
    ok = ConsumeNestinParent(context, local_result_flags);
  } else {
    return false;
  }

  if (!ok) {
    // failed_parsing_ = true;
    return false;
  }
  if (local_result_flags & kContainsScopeOrParent) {
    //....
  }
  result_flags |= local_result_flags;
  return true;
}

void CSSSelectorParser::ConsumeCompoundSelector(CSSParserContext& context, std::vector<A<CSSSelector> >& result, ResultFlags& result_flags) {
  auto& stream = context.token_stream;

  AtomicString namespace_prefix;
  AtomicString element_name;
  bool has_q_name = ConsumeName(context, element_name, namespace_prefix);

  while(ConsumeSimpleSelector(context, result_flags)) {

  }

}


// 处理命名空间
// 提取一个 <tag> name，或者 *， 或者 <namespace> | 
bool CSSSelectorParser::ConsumeName(CSSParserContext &context,
                                    AtomicString &name,
                                    AtomicString &namespace_prefix) {
  // null 表示 Default
  // * 表示 All

  name = g_null_atom;
  namespace_prefix = g_null_atom;

  auto& stream = context.token_stream;
  const CSSParserToken& first_token = stream.Peek();
  if (first_token.GetType() == CSSParserTokenType::Ident) {
    // 这可能是一个tag name，也可能是一个namespace，还需要判断后面是否跟着|
    name = first_token.Name();
    stream.Consume();
  } else if (first_token.GetType() == CSSParserTokenType::Delimiter &&
             first_token.Delimiter() == '*') {
    name = g_null_atom;
    stream.Consume();
  } else if (first_token.GetType() == CSSParserTokenType::Delimiter &&
             first_token.Delimiter() == '|') {
    // 可能是一个匿名命名空间
    name = g_empty_atom;
  } else {
    return false;
  }

  if (stream.Peek().GetType() != CSSParserTokenType::Delimiter ||
      stream.Peek().Delimiter() != '|') {
    return true;
  }

  CSSParserSavePoint savepoint(stream);
  stream.Consume();

  // 有|，把前面的内容作为namespace
  if (name == g_null_atom) {
    namespace_prefix = g_star_atom;
  } else {
    namespace_prefix = name;
  }

  if (stream.Peek().GetType() == CSSParserTokenType::Ident) {
    name = stream.Consume().Name();
  } else if (stream.Peek().GetType() == CSSParserTokenType::Delimiter &&
             stream.Peek().Delimiter() == '*') {
    stream.Consume();
    name = g_null_atom;
  } else {
    name = g_null_atom;
    namespace_prefix = g_null_atom;
    return false;
  }

  savepoint.Release();
  return true;
}
}