#include "html/css/selector_checker.h"
#include "html/base/atomic_string.h"
#include "html/css/css_selector.h"
#include "html/dom/element.h"
#include "html/dom/qualified_name.h"
#include <cassert>
#include <cstdlib>

namespace html {

bool EasySelectorChecker::IsEasy(const CSSSelector *selector) {
  bool has_descendant_selector = false;
  for (; selector != nullptr; selector = selector->NextSimpleSelector()) {
    if (!selector->IsLastInComplexSelector() &&
        selector->GetRelationType() != CSSSelector::RelationType::SubSelector &&
        selector->GetRelationType() != CSSSelector::RelationType::Descendant &&
        selector->GetRelationType() != CSSSelector::RelationType::Child) {
      return false;
    }

    if (selector->GetRelationType() == CSSSelector::RelationType::Descendant) {
      has_descendant_selector = true;
    } else if (selector->GetRelationType() == CSSSelector::RelationType::Child && has_descendant_selector) {
      return false;
    }
    // if (selector->IsCoveredByBucketing()) {
    //   continue;
    // }
    switch (selector->GetMatchType()) {
    case CSSSelector::MatchType::Tag:
    case CSSSelector::MatchType::UniversalTag:
    case CSSSelector::MatchType::Id:
    case CSSSelector::MatchType::Class:
      break;

    case CSSSelector::MatchType::AttributeExact:
    case CSSSelector::MatchType::AttributeSet:
      if (selector->GetAttribute().Prefix() == g_star_atom) {
        return false;
      }
      break;
    default:
      return false;
    }
  }
  return true;
}

bool EasySelectorChecker::Match(const CSSSelector* selector, const Element* element) {
  assert(IsEasy(selector));

  const CSSSelector* rewind_on_failure = nullptr;
  while (selector != nullptr) {
    if (/*selector->IsCoveredByBucketing() || */MatchOne(selector, element)) {
      if (selector->Relation() == CSSSelector::RelationType::Descendant) {
        rewind_on_failure = selector->NextSimpleSelector();

        element = element->parentElement();
        if (element == nullptr) {
          return false;
        }
      } else if (selector->Relation() == CSSSelector::RelationType::Child) {
        element = element->parentElement();
        if (element == nullptr) {
          return false;
        }
      }
      selector = selector->NextSimpleSelector();
    } else if (rewind_on_failure) {
      selector = rewind_on_failure;
      element = element->parentElement();
      if (element == nullptr) {
        return false;
      }
    } else {
      return false;
    }

  }
  return true;
}

bool EasySelectorChecker::MatchOne(const CSSSelector* selector, const Element* element) {
  switch (selector->GetMatchType()) {
  // case CSSSelector::MatchType::Tag: {
  //     const QualifiedName& tag_q_name = selector->TagQName();
  //     if (element->namespaceURI() != tag_q_name.NamespaceUri() && 
  //       tag_q_name.NamespaceUri() != g_star_atom) {
  //         // namespace mismatch
  //         return false;
  //     }
  //     if (element->localName() == tag_q_name.LocalName()) {
  //       return true;
  //     }
  //     if (!element->IsHTMLElement() && IsA<HTMLDocument>(element->GetDocument())) {
  //       return element->TagQName().LocalNameUpper() == tag_q_name.LocalNameUpper();
  //     } else {
  //       return false;
  //     }
  //   }
  //   break;

  // case CSSSelector::MatchType::Class:
  //   if (!element->CouldHaveClass(selector->Value())) {
  //     return false;
  //   }
  //   return element->HasClass() && element->ClassNames().Contains(selector->Value());
  //   break;

  case CSSSelector::MatchType::Id:
    return element->HasID() && element->IdForStyleResolution() == selector->Value();

  default:
    assert(false && "TODO:");
    abort();
  }
  return false;
}

}