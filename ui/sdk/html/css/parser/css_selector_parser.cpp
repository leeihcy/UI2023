#include "html/css/parser/css_selector_parser.h"
#include "html/base/atomic_string.h"
#include "html/css/css_selector.h"
#include "html/css/css_selector_list.h"
#include "html/css/parser/css_parser.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_token_stream.h"
#include "html/dom/qualified_name.h"
#include "html/util/util.h"
#include <cstdlib>
#include <string>


namespace html {

// static
void CSSSelectorParser::ConsumeSelector(CSSSelectorParserContext &context, StyleSheetContents* style_sheet,
                                        std::vector<CSSSelector> &result) {
  CSSSelectorParser parser(style_sheet);
  context.token_stream.ConsumeWhitespace();

  std::span<CSSSelector> out = parser.ConsumeComplexSelectorList(context);
  result.swap(parser.m_output);
}
// html, head, body { }
std::span<CSSSelector> CSSSelectorParser::ConsumeComplexSelectorList(CSSSelectorParserContext &context) {
  auto &stream = context.token_stream;

  ResetVectorAfterScope reset_vector(m_output);
  std::span<CSSSelector> result = ConsumeComplexSelector(context);
  if (result.empty()) {
    return {};
  }

  // 逗号分隔多个不同的selector，例如：a:hover, button:hover
  while (!stream.AtEnd() &&
         stream.Peek().GetType() == CSSParserTokenType::Comma) {
    stream.ConsumeIncludingWhitespace();
    int size = m_output.size();
    ConsumeComplexSelector(context);
    if (size == m_output.size()) {
      break;
    }
  }

  if (m_failed_parsing) {
    return {};
  }
  return reset_vector.CommitAddedElements();
}

CSSSelector::RelationType
CSSSelectorParser::ConsumeCombinator(CSSSelectorParserContext &context) {
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

bool CSSSelectorParser::PeekIsCombinator(CSSParserTokenStream& stream) {
  stream.ConsumeWhitespace();

  if (stream.Peek().GetType() != CSSParserTokenType::Delimiter) {
    return false;
  }

  switch (stream.Peek().Delimiter()) {
    case '+':
    case '~':
    case '>':
      return true;
    default:
      return false;
  }
}

namespace {

enum CompoundSelectorFlags {
  kHasPseudoElementForRightmostCompound = 1 << 0,
};

unsigned ExtractCompoundFlags(const CSSSelector& simple_selector/*,
                              CSSParserMode parser_mode*/) {
  if (simple_selector.GetMatchType() != CSSSelector::PseudoElement) {
    return 0;
  }
  return kHasPseudoElementForRightmostCompound;
}

unsigned ExtractCompoundFlags(const std::span<CSSSelector> compound_selector/*,
                              CSSParserMode parser_mode*/) {
  unsigned compound_flags = 0;
  for (const CSSSelector& simple : compound_selector) {
    if (compound_flags) {
      break;
    }
    compound_flags |= ExtractCompoundFlags(simple/*, parser_mode*/);
  }
  return compound_flags;
}

}  // namespace


bool CSSSelectorParser::ConsumePartialComplexSelector(
    CSSSelectorParserContext& context,
    CSSSelector::RelationType& combinator,
    unsigned previous_compound_flags) {
  do {
    std::span<CSSSelector> compound_selector =
        ConsumeCompoundSelector(context);
    if (compound_selector.empty()) {
      // No more selectors. If we ended with some explicit combinator
      // (e.g. “a >” and then nothing), that's a parse error.
      // But if not, we're simply done and return everything
      // we've parsed so far.
      return combinator == CSSSelector::RelationType::Descendant;
    }
    compound_selector.back().SetRelation(combinator);

    // See ConsumeComplexSelector().
    // std::ranges::reverse(compound_selector);
    std::reverse(compound_selector.begin(), compound_selector.end());

    if (previous_compound_flags & kHasPseudoElementForRightmostCompound) {
      // If we've already seen a compound that needs to be rightmost, and still
      // get more, that's a parse error.
      return false;
    }
    previous_compound_flags =
        ExtractCompoundFlags(compound_selector/*, context_->Mode()*/);
  } while ((combinator = ConsumeCombinator(context)) != CSSSelector::RelationType::SubSelector);

  return true;
}


static CSSNestingType GetNestingTypeForSelectorList(
    const CSSSelector* selector) {
  if (selector == nullptr) {
    return CSSNestingType::None;
  }
  CSSNestingType nesting_type = CSSNestingType::None;
  for (;;) {  // Termination condition within loop.
    nesting_type = std::max(nesting_type, selector->GetNestingType());

    auto* list = selector->SelectorList();
    if (list != nullptr) {
      nesting_type =
          std::max(nesting_type, GetNestingTypeForSelectorList(list->First()));
    }
    if (selector->IsLastInSelectorList() ||
        nesting_type == CSSNestingType::Nesting) {
      break;
    }

    (++selector);
  }
  return nesting_type;
}

static CSSNestingType GetNestingTypeForSelectorList(
    std::span<const CSSSelector> selector) {
  if (selector.empty()) {
    return CSSNestingType::None;
  }
  CSSNestingType nesting_type = CSSNestingType::None;
  for (const CSSSelector& curr : selector) {
    nesting_type = std::max(nesting_type, curr.GetNestingType());

    auto* list = curr.SelectorList();
    if (list != nullptr) {
      nesting_type =
          std::max(nesting_type, GetNestingTypeForSelectorList(list->First()));
    }
    if (curr.IsLastInSelectorList() ||
        nesting_type == CSSNestingType::Nesting) {
      break;
    }
  }
  return nesting_type;
}

static CSSSelector CreateImplicitAnchor(
    CSSNestingType nesting_type,
    const StyleRule* parent_rule_for_nesting) {
  assert(nesting_type == CSSNestingType::Nesting ||
         nesting_type == CSSNestingType::Scope);
  CSSSelector selector =
      (nesting_type == CSSNestingType::Nesting)
          ? CSSSelector(parent_rule_for_nesting, /*is_implicit=*/true)
          : CSSSelector(AtomicString(u"scope"), /*is_implicit=*/true);
  selector.SetScopeContaining(true);
  return selector;
}

static std::optional<CSSSelector> MaybeCreateImplicitDescendantAnchor(
    CSSNestingType nesting_type,
    const StyleRule* parent_rule_for_nesting,
    std::span<const CSSSelector> selector) {
  switch (nesting_type) {
    case CSSNestingType::None:
      break;
    case CSSNestingType::Scope:
    case CSSNestingType::Nesting:
      static_assert(CSSNestingType::None < CSSNestingType::Scope);
      static_assert(CSSNestingType::Scope < CSSNestingType::Nesting);
      // For kNesting, we should only produce an implied descendant combinator
      // if the selector list is not nest-containing.
      //
      // For kScope, we should should only produce an implied descendant
      // combinator if the selector list is not :scope-containing. Note however
      // that selectors which are nest-containing are also treated as
      // :scope-containing.
      if (GetNestingTypeForSelectorList(selector) < nesting_type) {
        return CreateImplicitAnchor(nesting_type, parent_rule_for_nesting);
      }
      break;
    case CSSNestingType::Function:
      // NOTREACHED();
      abort();
  }
  return std::nullopt;
}

void MarkAsEntireComplexSelector(std::span<CSSSelector> selectors) {
#if defined(_DEBUG)
  for (CSSSelector& selector : selectors.first(selectors.size() - 1)) {
    assert(!selector.IsLastInComplexSelector());
  }
#endif
  selectors.back().SetLastInComplexSelector(true);
}

// 不包含逗号的单个selector
std::span<CSSSelector> CSSSelectorParser::ConsumeComplexSelector(
  CSSSelectorParserContext& context) {
  // 在block中，允许以&开头，用于减少前缀重复，如：
  // .button { }
  // .button:hover  { }
  // 可以缩写为：
  // .button {
  //   &:hover { }
  // }
  if (context.nesting_type != CSSNestingType::None && PeekIsCombinator(context.token_stream)) {
    return ConsumeNestedRelativeSelector(context);
  }

  ResetVectorAfterScope reset_vector(m_output);

  std::span<CSSSelector> compound_selector = ConsumeCompoundSelector(context);
  if (compound_selector.empty()) {
    return {};
  }

  std::reverse(compound_selector.begin(), compound_selector.end());

  // 获取与下一个selector之间的连接符，如空格，>子对象。
  CSSSelector::RelationType combinator = ConsumeCombinator(context);
  if (combinator != CSSSelector::RelationType::SubSelector) {
    context.result_flags |= (int)kContainsComplexSelector; 

    unsigned previous_compound_flags =
        ExtractCompoundFlags(compound_selector/*, context_->Mode()*/);
    if (!ConsumePartialComplexSelector(context, combinator,
                                       previous_compound_flags)) {
      return {};
    }
  }

  auto result = reset_vector.AddedElements();
  std::reverse(result.begin(), result.end());

  if (context.nesting_type != CSSNestingType::None) {
    size_t last_index = m_output.size() - 1;
    m_output[last_index].SetLastInSelectorList(true);
    if (std::optional<CSSSelector> anchor = MaybeCreateImplicitDescendantAnchor(
            context.nesting_type, parent_rule_for_nesting_,
            reset_vector.AddedElements())) {
      m_output.back().SetRelation(CSSSelector::RelationType::Descendant);
      m_output.push_back(anchor.value());
      context.result_flags |= kContainsScopeOrParent;
    }

    m_output[last_index].SetLastInSelectorList(false);
  }

  MarkAsEntireComplexSelector(reset_vector.AddedElements());
  return reset_vector.CommitAddedElements();
}

bool CSSSelectorParser::ConsumeId(CSSSelectorParserContext& context) {
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

bool CSSSelectorParser::ConsumeClass(CSSSelectorParserContext& context) {
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


bool CSSSelectorParser::ConsumeAttribute(CSSSelectorParserContext &context) {
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
    attribute_name = attribute_name.Lower();
  // }

  AtomicString namespace_uri = DetermineNamespace(namespace_prefix);
  if (namespace_uri.IsNull()) {
    return false;
  }

  QualifiedName qualified_name =
      namespace_prefix.IsNull()
          ? QualifiedName(attribute_name)
          : QualifiedName(namespace_prefix, attribute_name, namespace_uri);

  if (stream.AtEnd()) {
    // [foo]
    CSSSelector selector(CSSSelector::MatchType::AttributeSet, qualified_name,
                         CSSSelector::AttributeMatchType::kCaseSensitive);
    m_output.push_back(std::move(selector));
    return true;
  }
  
  // $=  ^=  *=
  CSSSelector::MatchType match_type = ConsumeAttributeMatch(context);
  CSSParserToken attribute_value = stream.Peek();
  if (attribute_value.GetType() != CSSParserTokenType::Ident &&
      attribute_value.GetType() != CSSParserTokenType::String) {
    return false;
  }
  stream.ConsumeIncludingWhitespace();

  // i s 大小写后缀
  CSSSelector::AttributeMatchType case_sensitivity =
      ConsumeAttributeFlags(context);
  if (!stream.AtEnd()) {
    return false;
  }
  CSSSelector selector(match_type, qualified_name, case_sensitivity,
                       AtomicString(attribute_value.Name()));
  m_output.push_back(std::move(selector));
  return true;
}


CSSSelector::MatchType CSSSelectorParser::ConsumeAttributeMatch(CSSSelectorParserContext &context) {
  CSSParserTokenStream& stream = context.token_stream;

  const CSSParserToken& token = stream.Peek();
  switch (token.GetType()) {
    case CSSParserTokenType::IncludeMatch:
      stream.ConsumeIncludingWhitespace();
      return CSSSelector::AttributeList;
    case CSSParserTokenType::DashMatch:
      stream.ConsumeIncludingWhitespace();
      return CSSSelector::AttributeHyphen;
    case CSSParserTokenType::PrefixMatch:
      stream.ConsumeIncludingWhitespace();
      return CSSSelector::AttributeBegin;
    case CSSParserTokenType::SuffixMatch:
      stream.ConsumeIncludingWhitespace();
      return CSSSelector::AttributeEnd;
    case CSSParserTokenType::SubstringMatch:
      stream.ConsumeIncludingWhitespace();
      return CSSSelector::AttributeContain;
    case CSSParserTokenType::Delimiter:
      if (token.Delimiter() == '=') {
        stream.ConsumeIncludingWhitespace();
        return CSSSelector::AttributeExact;
      }
      [[fallthrough]];
    default:
      m_failed_parsing = true;
      return CSSSelector::AttributeExact;
  }
}

// [foo="bar" i]
CSSSelector::AttributeMatchType CSSSelectorParser::ConsumeAttributeFlags(
    CSSSelectorParserContext& context) {
  CSSParserTokenStream& stream = context.token_stream;

  if (stream.Peek().GetType() != CSSParserTokenType::Ident) {
    return CSSSelector::AttributeMatchType::kCaseSensitive;
  }
  const CSSParserToken& flag = stream.ConsumeIncludingWhitespace();
  if (EqualIgnoringASCIICase(flag.Name().c_str(), u"i")) {
    return CSSSelector::AttributeMatchType::kCaseInsensitive;
  } else if (EqualIgnoringASCIICase(flag.Name().c_str(), u"s") /*&&
             RuntimeEnabledFeatures::CSSCaseSensitiveSelectorEnabled()*/) {
    return CSSSelector::AttributeMatchType::kCaseSensitiveAlways;
  }
  m_failed_parsing = true;
  return CSSSelector::AttributeMatchType::kCaseSensitive;
}

bool CSSSelectorParser::ConsumePseudo(CSSSelectorParserContext &context, ResultFlags &result_flags) {
  assert(false);
  return false;
}

bool CSSSelectorParser::ConsumeNestinParent(CSSSelectorParserContext &context, ResultFlags &result_flags) {
  assert(false);
  return false;
}

bool CSSSelectorParser::ConsumeSimpleSelector(CSSSelectorParserContext& context) {
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
    m_failed_parsing = true;
    return false;
  }
  if (local_result_flags & kContainsScopeOrParent) {
    //....
  }
  context.result_flags |= local_result_flags;
  return true;
}
// 复合选择器，是由多个简单选择器组合而成的，例如：div.example#id[data-type]
std::span<CSSSelector> CSSSelectorParser::ConsumeCompoundSelector(CSSSelectorParserContext& context) {
  auto& stream = context.token_stream;
  
  ResetVectorAfterScope reset_vector(m_output);
  size_t start_pos = m_output.size();
  // base::AutoReset<CSSSelector::PseudoType> reset_restricting(
  //     &restricting_pseudo_element_, restricting_pseudo_element_);
  // base::AutoReset<bool> reset_found_host_in_compound(&found_host_in_compound_,
  //                                                    false);

  AtomicString namespace_prefix;
  AtomicString element_name;
  bool has_q_name = ConsumeName(context, element_name, namespace_prefix);
  //if (context_->IsHTMLDocument()) {
    element_name = element_name.Lower();
  //}

  // A tag name is not valid following a pseudo-element. This can happen for
  // e.g. :::part(x):is(div).
  // if (restricting_pseudo_element_ != CSSSelector::kPseudoUnknown &&
  //     has_q_name) {
  //   failed_parsing_ = true;
  //   return {};  // Failure.
  // }

  std::vector<size_t> has_pseudo_index_in_compound;
  while(ConsumeSimpleSelector(context)) {
    const CSSSelector& simple_selector = m_output.back();
    if (simple_selector.GetMatchType() == CSSSelector::PseudoElement) {
      restricting_pseudo_element_ = simple_selector.GetPseudoType();
    }
    if (simple_selector.GetPseudoType() == CSSSelector::kPseudoHas) {
      has_pseudo_index_in_compound.push_back(m_output.size() - 1);
    }
    m_output.back().SetRelation(CSSSelector::RelationType::SubSelector);
  }
  if (found_host_in_compound_) {
    for (size_t has_pseudo_index : has_pseudo_index_in_compound) {
      assert(has_pseudo_index < m_output.size());
      CSSSelector* has_pseudo = &m_output[has_pseudo_index];
      assert(has_pseudo->GetPseudoType() == CSSSelector::kPseudoHas);
      has_pseudo->SetHasArgumentMatchInShadowTree();
    }
  }

  //  base::AutoReset<bool> ignore_namespace(
  //     &ignore_default_namespace_,
  //     ignore_default_namespace_ || (resist_default_namespace_ && !has_q_name &&
  //                                   AtEndIgnoringWhitespace(stream)));

  if (reset_vector.AddedElements().empty()) {
    if (!has_q_name) {
      return {};
    }
    assert(has_q_name);
    AtomicString namespace_uri = DetermineNamespace(namespace_prefix);
    if (namespace_uri.IsNull()) {
      // context_->Count(WebFeature::kCSSUnknownNamespacePrefixInSelector);
      m_failed_parsing = true;
      return {};
    }
    if (namespace_uri == DefaultNamespace()) {
      namespace_prefix = g_null_atom;
    }
    m_output.push_back(CSSSelector(
        QualifiedName(namespace_prefix, element_name, namespace_uri)));
    return reset_vector.CommitAddedElements();
  }

  PrependTypeSelectorIfNeeded(namespace_prefix, has_q_name, element_name,
                              start_pos);

  // The relationship between all of these are that they are sub-selectors.
  for (CSSSelector& selector : reset_vector.AddedElements().first(
           reset_vector.AddedElements().size() - 1)) {
    selector.SetRelation(CSSSelector::RelationType::SubSelector);
  }

  SplitCompoundAtImplicitCombinator(reset_vector.AddedElements());
  return reset_vector.CommitAddedElements();
}

CSSSelector::RelationType GetImplicitCombinatorForMatching(
    CSSSelector::PseudoType pseudo_type) {
  // switch (pseudo_type) {
  //   case CSSSelector::PseudoType::kPseudoSlotted:
  //     return CSSSelector::RelationType::kShadowSlot;
  //   case CSSSelector::PseudoType::kPseudoWebKitCustomElement:
  //   case CSSSelector::PseudoType::kPseudoBlinkInternalElement:
  //   case CSSSelector::PseudoType::kPseudoCue:
  //   case CSSSelector::PseudoType::kPseudoDetailsContent:
  //   case CSSSelector::PseudoType::kPseudoPlaceholder:
  //   case CSSSelector::PseudoType::kPseudoFileSelectorButton:
  //   case CSSSelector::PseudoType::kPseudoPicker:
  //   case CSSSelector::PseudoType::kPseudoPermissionIcon:
  //     return CSSSelector::RelationType::kUAShadow;
  //   case CSSSelector::PseudoType::kPseudoPart:
  //     return CSSSelector::RelationType::kShadowPart;
  //   case CSSSelector::PseudoType::kPseudoBefore:
  //   case CSSSelector::PseudoType::kPseudoAfter:
  //   case CSSSelector::PseudoType::kPseudoMarker:
  //     // TODO(crbug.com/444386484): Support additional pseudo-elements.
  //     if (RuntimeEnabledFeatures::CSSLogicalCombinationPseudoEnabled()) {
  //       return CSSSelector::RelationType::PseudoChild;
  //     }
  //     [[fallthrough]];
  //   default:
      return CSSSelector::RelationType::SubSelector;
  // }
}
CSSSelector::RelationType GetImplicitShadowCombinatorForMatching(CSSSelector::PseudoType pseudo_type) {
  // TODO:
  return CSSSelector::RelationType::SubSelector;
}

bool NeedsImplicitCombinatorForMatching(const CSSSelector& selector) {
  return GetImplicitCombinatorForMatching(selector.GetPseudoType()) !=
         CSSSelector::RelationType::SubSelector;
}
bool NeedsImplicitShadowCombinatorForMatching(const CSSSelector& selector) {
  return GetImplicitShadowCombinatorForMatching(selector.GetPseudoType()) != CSSSelector::RelationType::SubSelector;
}

void CSSSelectorParser::SplitCompoundAtImplicitCombinator(
    std::span<CSSSelector> selectors) {
  // slot[name=foo]::slotted(div) -> [ ::slotted(div), slot, [name=foo] ]
  for (size_t i = 1; i < selectors.size(); ++i) {
    if (NeedsImplicitCombinatorForMatching(selectors[i])) {
      CSSSelector::RelationType relation =
          GetImplicitCombinatorForMatching(selectors[i].GetPseudoType());
      std::rotate(selectors.begin(), selectors.begin() + i, selectors.end());

      std::span<CSSSelector> remaining = selectors.first(selectors.size() - i);
      // We might need to split the compound multiple times, since a number of
      // the relevant pseudo-elements can be combined, and they all need an
      // implicit combinator for matching.
      SplitCompoundAtImplicitCombinator(remaining);
      remaining.back().SetRelation(relation);
      break;
    }
  }
}


// tag[foo=bar]
// 解析出了tag后，并不会立即添加到output中。[foo=bar]会被加入到output中。
// 
void CSSSelectorParser::PrependTypeSelectorIfNeeded(
    const AtomicString& namespace_prefix,
    bool has_q_name,
    const AtomicString& element_name,
    size_t start_index_of_compound_selector) {
  const CSSSelector& compound_selector = m_output[start_index_of_compound_selector];

  // if (!has_q_name && DefaultNamespace() == g_star_atom &&
  //     !NeedsImplicitShadowCombinatorForMatching(compound_selector)) {
  //   return;
  // }

  AtomicString determined_element_name = !has_q_name? g_null_atom : element_name;
  AtomicString namespace_uri = DetermineNamespace(namespace_prefix);
  if (namespace_uri.IsNull()) {
    m_failed_parsing = true;
    return;
  }
  AtomicString determined_prefix = namespace_prefix;
  if (namespace_uri == DefaultNamespace()) {
    determined_prefix = g_null_atom;
  }
  QualifiedName tag = QualifiedName(determined_prefix, determined_element_name, namespace_uri);

  bool is_host_pseudo = false;
  // bool is_host_pseudo = IsHostPseudoSelector(compound_selector);
  // if (is_host_pseudo && !has_q_name && namespace_prefix.IsNull()) {
  //   return;
  // }

  if (tag != g_any_name || is_host_pseudo ||
      NeedsImplicitShadowCombinatorForMatching(compound_selector)) {
    const bool is_implicit =
        determined_prefix == g_null_atom &&
        determined_element_name == CSSSelector::UniversalSelectorAtom() &&
        !is_host_pseudo;

    m_output.insert(m_output.begin() + start_index_of_compound_selector,
                    CSSSelector(tag, is_implicit));
  }
}

// 处理命名空间
// 提取一个 <tag> name，或者 *， 或者 <namespace> | 
// 注：namespace和|之间不能有空格。空格在selector中代表后代选择器。
bool CSSSelectorParser::ConsumeName(CSSSelectorParserContext &context,
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


std::span<CSSSelector> CSSSelectorParser::ConsumeNestedRelativeSelector(
    CSSSelectorParserContext &context) {
  assert(context.nesting_type != CSSNestingType::None);

  ResetVectorAfterScope reset_vector(m_output);

  m_output.push_back(
      CreateImplicitAnchor(context.nesting_type, parent_rule_for_nesting_));
  context.result_flags |= kContainsScopeOrParent;
  CSSSelector::RelationType combinator = ConsumeCombinator(context);
  unsigned previous_compound_flags = 0;
  if (!ConsumePartialComplexSelector(context, combinator,
                                     previous_compound_flags)) {
    return {};
  }

  std::span<CSSSelector> result = reset_vector.AddedElements();
  std::reverse(result.begin(), result.end());

  MarkAsEntireComplexSelector(reset_vector.AddedElements());
  return reset_vector.CommitAddedElements();
}

}