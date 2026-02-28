#include "html/css/rule_set.h"
#include "html/base/atomic_string.h"
#include "html/base/casting.h"
#include "html/css/css_selector.h"
#include "html/css/style_rule.h"
#include "html/css/style_sheet_contents.h"
#include <cassert>
#include <vector>

namespace html {

namespace {

struct BucketingValues {
public:
  AtomicString id;
  AtomicString class_name;
  AtomicString attr_name;
  AtomicString attr_value;
  bool is_exact_attr = false;
  AtomicString custom_pseudo_element_name;
  AtomicString tag_name;
  AtomicString part_name;
  AtomicString ua_shadow_pseudo;
  CSSSelector::PseudoType pseudo_type = CSSSelector::PseudoType::kPseudoUnknown;
  bool has_slotted = false;
};

}

void RuleSet::AddRulesFromSheet(const StyleSheetContents* sheet, const StyleScope* style_scope) {
  // 1. pre import layer statement rules

  // 2. import rules

  // 3. child rules
  AddChildRules(nullptr, sheet->GetChildRules(), style_scope);
}

void RuleSet::AddChildRules(StyleRule* parent_rule, 
  const std::vector<A<StyleRuleBase>>& rules,
 const StyleScope* style_scope) {
  for (auto& i : rules) {
    auto* style_rule = DynamicTo<StyleRule>(i.get());
    if (style_rule) {
      AddStyleRule(style_rule, parent_rule, style_scope);
    } else {
      assert(false && "TODO");
    }
  }
}

void RuleSet::AddStyleRule(StyleRule* style_rule, StyleRule* parent_rule,
                           const StyleScope* style_scope) {
  int selector_count = style_rule->GetSelectorCount();
  for (int i = 0; i < selector_count; i++) {
    AddRule(style_rule, i, style_scope);
  }

  if (style_rule->GetChildRuleCount()) {
    AddChildRules(style_rule, style_rule->GetChildRules(), style_scope);
  }
}

void RuleSet::AddRule(StyleRule *style_rule, size_t selector_index,
                      const StyleScope *style_scope) {
  RuleData rule_data(*style_rule, selector_index, m_rule_count);
  m_rule_count++;

  FindBestBucketAndAdd<BucketCoverage::kCompute>(rule_data.MutableSelector(),
                                                 rule_data, style_scope);
}

// The return value indicates if extracting can continue
// or should be stopped due to reaching some pseudo-element
// that doesn't allow extracting bucketing rules after itself
// in selector.
static bool ExtractBucketingValues(const CSSSelector* selector,
                                   const StyleScope* style_scope,
                                   BucketingValues& values) {
  switch (selector->GetMatchType()) {
    case CSSSelector::MatchType::Id:
      values.id = selector->Value();
      break;
    case CSSSelector::MatchType::Class:
      values.class_name = selector->Value();
      break;
    case CSSSelector::MatchType::Tag:
      values.tag_name = selector->TagQName().LocalName();
      break;
    // case CSSSelector::MatchType::PseudoElement:
    //   // TODO(403505399): We shouldn't allow bucketing of pseudo-classes
    //   // after pseudo-elements for now, as it confuses bucketing.
    //   if (ShouldStopExtractingAtPseudoElement(selector->GetPseudoType())) {
    //     return false;
    //   }
    //   [[fallthrough]];
    case CSSSelector::MatchType::PseudoClass:
    case CSSSelector::MatchType::PagePseudoClass:
      // Must match the cases in RuleSet::FindBestBucketAndAdd.
      switch (selector->GetPseudoType()) {
        case CSSSelector::kPseudoFocus:
        // case CSSSelector::kPseudoCue:
        // case CSSSelector::kPseudoLink:
        // case CSSSelector::kPseudoVisited:
        // case CSSSelector::kPseudoWebkitAnyLink:
        // case CSSSelector::kPseudoAnyLink:
        // case CSSSelector::kPseudoFocusVisible:
        // case CSSSelector::kPseudoHost:
        // case CSSSelector::kPseudoHostContext:
        // case CSSSelector::kPseudoSlotted:
        // case CSSSelector::kPseudoSelectorFragmentAnchor:
        case CSSSelector::kPseudoRoot:
        // case CSSSelector::kPseudoActiveViewTransition:
        // case CSSSelector::kPseudoOverscrollTarget:
          // Pseudo classes.
          values.pseudo_type = selector->GetPseudoType();
          // if (values.pseudo_type == CSSSelector::kPseudoSlotted) {
          //   values.has_slotted = true;
          // }
          break;
        // case CSSSelector::kPseudoPlaceholder:
        // case CSSSelector::kPseudoDetailsContent:
        // case CSSSelector::kPseudoPermissionIcon:
        // case CSSSelector::kPseudoFileSelectorButton:
        // case CSSSelector::kPseudoScrollbarButton:
        // case CSSSelector::kPseudoScrollbarCorner:
        // case CSSSelector::kPseudoScrollbarThumb:
        // case CSSSelector::kPseudoScrollbarTrack:
        // case CSSSelector::kPseudoScrollbarTrackPiece:
        //   // Pseudo elements; do not overwrite a pseudo class
        //   // (in particular, :host).
        //   if (values.pseudo_type == CSSSelector::kPseudoUnknown) {
        //     values.pseudo_type = selector->GetPseudoType();
        //     values.ua_shadow_pseudo =
        //         shadow_element_utils::StringForUAShadowPseudoId(
        //             CSSSelector::GetPseudoId(values.pseudo_type));
        //   }
        //   break;
        // case CSSSelector::kPseudoWebKitCustomElement:
        // case CSSSelector::kPseudoBlinkInternalElement:
        //   values.custom_pseudo_element_name = selector->Value();
        //   break;
        // case CSSSelector::kPseudoPart:
        //   values.part_name = selector->Value();
        //   break;
    //     case CSSSelector::kPseudoPicker:
    //       if (selector->Argument() == "select") {
    //         values.ua_shadow_pseudo = shadow_element_names::kPickerSelect;
    //       }
    //       break;
    //     case CSSSelector::kPseudoIs:
    //     case CSSSelector::kPseudoWhere:
    //     case CSSSelector::kPseudoParent: {
    //       const CSSSelector* selector_list = selector->SelectorListOrParent();
    //       // If the :is/:where has only a single argument, it effectively acts
    //       // like a normal selector (save for specificity), and we can put it
    //       // into a bucket based on that selector.
    //       //
    //       // Note that `selector_list` may be nullptr for top-level '&'
    //       // selectors.
    //       //
    //       // Note also that FindBestBucketAndAdd assumes that you cannot
    //       // reach a pseudo-element via a '&' selector (crbug.com/380107557).
    //       // We ensure that this cannot happen by never adding rules
    //       // like '::before { & {} }' to the RuleSet in the first place,
    //       // see CollectMetadataFromSelector. Rules with mixed
    //       // allowed/disallowed selectors, e.g. '::before, .foo { & {} }',
    //       // *are* added to the RuleSet, but fail the IsSingleComplexSelector
    //       // check below, satisfying the assumptions of FindBestBucketAndAdd.
    //       if (selector_list &&
    //           CSSSelectorList::IsSingleComplexSelector(*selector_list)) {
    //         bool should_continue =
    //             ExtractBucketingValues(selector_list, style_scope, values);
    //         CHECK(should_continue);
    //       }
    //       break;
    //     }
    //     case CSSSelector::kPseudoScope: {
    //       // Just like :is() and :where(), we can bucket :scope as the
    //       // <scope-start> it refers to, as long as the <scope-start>
    //       // contains a single selector.
    //       //
    //       // Note that the <scope-start> selector is optional, therefore
    //       // From() may return nullptr below.
    //       const CSSSelector* selector_list =
    //           style_scope ? style_scope->From() : nullptr;
    //       if (selector_list &&
    //           CSSSelectorList::IsSingleComplexSelector(*selector_list)) {
    //         bool should_continue = ExtractBucketingValues(
    //             selector_list, style_scope->Parent(), values);
    //         CHECK(should_continue);
    //       }
    //       break;
    //     }
        default:
          break;
      }
      break;
    // case CSSSelector::kAttributeSet:
    //   values.attr_name = selector->Attribute().LocalName();
    //   values.attr_value = g_empty_atom;
    //   break;
    // case CSSSelector::kAttributeExact:
    // case CSSSelector::kAttributeHyphen:
    // case CSSSelector::kAttributeList:
    // case CSSSelector::kAttributeContain:
    // case CSSSelector::kAttributeBegin:
    // case CSSSelector::kAttributeEnd:
    //   values.is_exact_attr =
    //       (selector->Match() == CSSSelector::kAttributeExact);
    //   values.attr_name = selector->Attribute().LocalName();
    //   values.attr_value = selector->Value();
    //   break;
    default:
      break;
  }
  return true;
}

static 
void ExtractBestBucketingValues(const CSSSelector& component,  const StyleScope* style_scope, BucketingValues& values) {
  for (const CSSSelector* it = &component; it; it = it->NextSimpleSelector()) {
    if (!ExtractBucketingValues(it, style_scope, values)) {
      return;
    }
    switch (it->Relation()) {
      case CSSSelector::RelationType::SubSelector:
        continue;
      case CSSSelector::RelationType::kUAShadow: {
        // Any selector containing ::slotted() currently *must* go in
        // the slotted bucket. Since we allow UA-shadow pseudo-element
        // selectors after ::slotted(), and because such selectors exist
        // in a different compound from ::slotted() (effectively [1]),
        // we have to check if the originating compound contains ::slotted()
        // as well.
        //
        // Note that the same is not true for ::part(); selectors on
        // on the form ::part(p)::ua-shadow must bucket according
        // to ::ua-shadow.
        //
        // This discrepancy comes from the fact that StyleResolver::
        // MatchOuterScopeRules (which handles parts and UA shadow
        // pseudos) does look in the UA shadow bucket across trees,
        // but MatchSlottedRules *only* looks in the slotted bucket.
        // TODO(crbug.com/40068507): This discrepancy is weird.
        //
        // [1] CSSSelectorParser::SplitCompoundAtImplicitCombinator
        const CSSSelector* originating = it->NextSimpleSelector();
        assert(originating);
        BucketingValues originating_values;
        ExtractBestBucketingValues(*originating, style_scope,
                                   originating_values);
        values.has_slotted |= originating_values.has_slotted;
        return;
      };
      default:
        // We reached the end of the compound selector.
        return;
    }
  }
}


void RuleSet::AddToBucket(const AtomicString& key,
                          RuleMap& map,
                          const RuleData& rule_data) {
  if (map.IsCompacted()) {
    // This normally should not happen, but may with UA stylesheets;
    // see class comment on RuleMap.
    map.Uncompact();
  }
  if (!map.Add(key, rule_data)) {
    // This should really only happen in case of an attack;
    // we stick it in the universal bucket so that correctness
    // is preserved, even though the performance will be suboptimal.
    RuleData rule_data_copy = rule_data;
    // UnmarkAsCoveredByBucketing(rule_data_copy.MutableSelector());
    AddToBucket(universal_rules_, rule_data_copy);
    return;
  }
  // Don't call ComputeBloomFilterHashes() here; RuleMap needs that space for
  // group information, and will call ComputeBloomFilterHashes() itself on
  // compaction.
  need_compaction_ = true;
}


void RuleSet::AddToBucket(std::vector<RuleData>& rules,
                          const RuleData& rule_data) {
  rules.push_back(rule_data);
  // rules.back().ComputeEntirelyCoveredByBucketing();
  need_compaction_ = true;
}


template<RuleSet::BucketCoverage bucket_coverage>
void RuleSet::FindBestBucketAndAdd(CSSSelector& selector, const RuleData& rule_data,
const StyleScope* style_scope) {
  BucketingValues values;

  ExtractBestBucketingValues(selector, style_scope, values);

  if (!values.id.empty()) {
    // if (bucket_coverage == BucketCoverage::kCompute) {
    //   MarkAsCoveredByBucketing(component,
    //                            [&values](const CSSSelector& selector) {
    //                              return selector.Match() == CSSSelector::kId &&
    //                                     selector.Value() == values.id;
    //                            });
    // }
    AddToBucket(values.id, id_rules_, rule_data);
    return;
  }

  // if (!values.class_name.empty()) {
  //   if (bucket_coverage == BucketCoverage::kCompute) {
  //     MarkAsCoveredByBucketing(
  //         component, [&values](const CSSSelector& selector) {
  //           return selector.Match() == CSSSelector::kClass &&
  //                  selector.Value() == values.class_name;
  //         });
  //   }
  //   AddToBucket(values.class_name, class_rules_, rule_data);
  //   return;
  // }
}




}