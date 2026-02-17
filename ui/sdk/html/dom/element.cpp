#include "html/dom/element.h"

namespace html {


// inline const SpaceSplitString& Element::ClassNames() const {
//   DCHECK(HasClass());
//   DCHECK(HasElementData());
//   return GetElementData()->ClassNames();
// }

// inline bool Element::HasClassName(const AtomicString& class_name) const {
//   return HasElementData() &&
//          GetElementData()->ClassNames().Contains(class_name);
// }

// inline bool Element::HasClass() const {
//   return HasElementData() && GetElementData()->HasClass();
// }

inline const AtomicString& Element::IdForStyleResolution() const {
  assert(HasID());
  return GetElementData()->IdForStyleResolution();
}



#if 0
ineline void Element::SetAttributeInternal(
    size_t index,
    const QualifiedName& name,
    const AtomicString& new_value,
    AttributeModificationReason reason) {
  if (new_value.IsNull()) {
    if (index != kNotFound) {
      RemoveAttributeInternal(index, reason);
    }
    return;
  }

  if (index == kNotFound) {
    AppendAttributeInternal(name, new_value, reason);
    return;
  }

  const Attribute& existing_attribute =
      GetElementData()->Attributes().at(index);
  QualifiedName existing_attribute_name = existing_attribute.GetName();

  if (new_value == existing_attribute.Value()) {
    if (reason !=
        AttributeModificationReason::kBySynchronizationOfLazyAttribute) {
      WillModifyAttribute(existing_attribute_name, new_value, new_value);
      DidModifyAttribute(existing_attribute_name, new_value, new_value, reason);
    }
  } else {
    Attribute& new_attribute = EnsureUniqueElementData().Attributes().at(index);
    AtomicString existing_attribute_value = std::move(new_attribute.Value());
    if (reason !=
        AttributeModificationReason::kBySynchronizationOfLazyAttribute) {
      WillModifyAttribute(existing_attribute_name, existing_attribute_value,
                          new_value);
    }
    new_attribute.SetValue(new_value);
    if (reason !=
        AttributeModificationReason::kBySynchronizationOfLazyAttribute) {
      DidModifyAttribute(existing_attribute_name, existing_attribute_value,
                         new_value, reason);
    }
  }
}
void Element::AppendAttributeInternal(const QualifiedName& name,
                                      const AtomicString& value,
                                      AttributeModificationReason reason) {
  if (reason !=
      AttributeModificationReason::kBySynchronizationOfLazyAttribute) {
    WillModifyAttribute(name, g_null_atom, value);
  }
  attribute_or_class_bloom_ |= FilterForAttribute(name);
  UpdateSubtreeBloomFilterAfterInsert();
  EnsureUniqueElementData().Attributes().Append(name, value);
  if (reason !=
      AttributeModificationReason::kBySynchronizationOfLazyAttribute) {
    DidAddAttribute(name, value);
  }
}
#endif


void Element::DidAddAttribute(const QualifiedName& name,
                              const AtomicString& value) {
  AttributeChanged(AttributeModificationParams(
      name, g_null_atom, value, AttributeModificationReason::kDirectly));
#if 0
  if (name == html_names::kIdAttr) {
    UpdateId(g_null_atom, value);
  }
  probe::DidModifyDOMAttr(this, name, value);
#endif
}


void Element::ParseAttribute(const AttributeModificationParams& params) {
#if 0
  // Note that `HTMLElement::ParseAttribute` does not call this base class
  // method for anything other than *namespaced* attributes, e.g. `xml:lang`.
  // Therefore, in most cases, normal HTML attributes will not get handled here.
  if (params.name.Matches(xml_names::kLangAttr)) {
    LangAttributeChanged();
  }
#endif
}

void Element::AttributeChanged(const AttributeModificationParams& params) {
  ParseAttribute(params);
#if 0
  GetDocument().IncDOMTreeVersion();
  GetDocument().NotifyAttributeChanged(*this, params.name, params.old_value,
                                       params.new_value);
#endif

  const QualifiedName& name = params.name;
  if (name == /*html_names::kIdAttr*/ u"id") {

    AtomicString lowercase_id;
    // if (GetDocument().InQuirksMode() && !params.new_value.IsLowerASCII()) {
    //   lowercase_id = params.new_value.LowerASCII();
    // }
    const AtomicString& new_id = lowercase_id ? lowercase_id : params.new_value;
    // if (new_id != GetElementData()->IdForStyleResolution()) {
      AtomicString old_id = GetElementData()->SetIdForStyleResolution(new_id);
    //   GetDocument().GetStyleEngine().IdChangedForElement(old_id, new_id, *this);
    // }
#if 0
    ProcessElementRenderBlocking(new_id);

    if (GetDocument().HasPendingExpectLinkElements() &&
        IsFinishedParsingChildren()) {
      DCHECK(GetDocument().GetRenderBlockingResourceManager());
      GetDocument()
          .GetRenderBlockingResourceManager()
          ->RemovePendingParsingElement(GetIdAttribute(), this);
    }

    // If the id changes that may have been a target of overscroll command, we
    // need to notify that an overscroll-target pseudo class may have changed.
    const auto& overscroll_command_targets =
        GetDocument().OverscrollCommandTargets();
    auto invalidate_overscroll_target_state = [&](const AtomicString& idref) {
      OverscrollTargetStateChanged();
      // We also may have a new target with the same id. Note that this
      // invalidates all elements with this id, which should be a small set
      // typically.
      for (auto& element : GetDocument().GetAllElementsById(idref)) {
        element->OverscrollTargetStateChanged();
      }
    };

    if (!params.old_value.empty() &&
        overscroll_command_targets.Contains(params.old_value)) {
      invalidate_overscroll_target_state(params.old_value);
    }
    if (!params.new_value.empty() &&
        overscroll_command_targets.Contains(params.new_value)) {
      invalidate_overscroll_target_state(params.new_value);
    }
#endif
  }
#if 0 
  else if (name == html_names::kClassAttr) {
    if (params.old_value == params.new_value &&
        params.reason != AttributeModificationReason::kByMoveToNewDocument &&
        params.reason != AttributeModificationReason::kByCloning) {
      return;
    }
    ClassAttributeChanged(params.new_value);
    UpdateClassList(params.old_value, params.new_value);
    SoftNavigationHeuristics::ModifiedAttribute(this, name);
  } else if (name == html_names::kNameAttr) {
    SetHasName(!params.new_value.IsNull());
  } else if (HasTagName(html_names::kATag) && name == html_names::kHrefAttr) {
    // <a> element's href changed - update its scroll target group membership.
    To<HTMLAnchorElement>(this)->UpdateScrollTargetGroupMembership();
  } else if (name == html_names::kPartAttr) {
    part().DidUpdateAttributeValue(params.old_value, params.new_value);
    GetDocument().GetStyleEngine().PartChangedForElement(*this);
  } else if (name == html_names::kMarkerAttr) {
    marker().DidUpdateAttributeValue(params.old_value, params.new_value);
  } else if (name == html_names::kExportpartsAttr) {
    data_ = EnsureRareData().SetPartNamesMap(params.new_value);
    GetDocument().GetStyleEngine().ExportpartsChangedForElement(*this);
  } else if (name == html_names::kTabindexAttr) {
    int tabindex = 0;
    if (params.new_value.empty() ||
        !ParseHTMLInteger(params.new_value, tabindex)) {
      ClearTabIndexExplicitlyIfNeeded();
    } else {
      // We only set when value is in integer range.
      SetTabIndexExplicitly();
    }
    if (params.reason == AttributeModificationReason::kDirectly &&
        AdjustedFocusedElementInTreeScope() == this) {
      // The attribute change may cause supportsFocus() to return false
      // for the element which had focus.
      //
      // TODO(tkent): We should avoid updating style.  We'd like to check only
      // DOM-level focusability here.
      GetDocument().UpdateStyleAndLayoutTreeForElement(
          this, DocumentUpdateReason::kFocus);
      if (!IsFocusable() && !GetFocusableArea()) {
        blur();
      }
    }
  } else if (name == html_names::kSlotAttr) {
    if (params.old_value != params.new_value) {
      if (ShadowRoot* root = ShadowRootOfParent()) {
        root->DidChangeHostChildSlotName(params.old_value, params.new_value);
      }
    }
  } else if (name == html_names::kFocusgroupAttr) {
    // Only update the focusgroup flags when the node has been added to the
    // tree. This is because the computed focusgroup value will depend on the
    // focusgroup value of its closest ancestor node that is a focusgroup, if
    // any.
    if (parentNode()) {
      UpdateFocusgroup(params.new_value);
    }
  } else if (RuntimeEnabledFeatures::OverscrollGesturesEnabled() &&
             name == html_names::kOverscrollcontainerAttr) {
    if (params.new_value.IsNull() || params.old_value.IsNull()) {
      // TODO(crbug.com/467968812): We can optimize this in some cases since a
      // container that disappears necessarily adds its elements to the
      // ancestor container. However, if the container appears, it's harder to
      // figure out which elements are contained by it without doing a subtree
      // recalc.
      SetNeedsStyleRecalc(kSubtreeStyleChange,
                          StyleChangeReasonForTracing::FromAttribute(name));
    }
  } else if (IsStyledElement()) {
    if (name == html_names::kStyleAttr) {
      if (params.old_value == params.new_value) {
        return;
      }
      StyleAttributeChanged(params.new_value, params.reason);
      SoftNavigationHeuristics::ModifiedAttribute(this, name);
    } else if (IsPresentationAttribute(name)) {
      if (name == html_names::kAnchorAttr) {
        if (RuntimeEnabledFeatures::HTMLAnchorAttributeEnabled()) {
          EnsureAnchorElementObserver().Notify();
        }
      } else if (name == html_names::kHiddenAttr) {
        if (params.new_value == keywords::kUntilFound) {
          EnsureDisplayLockContext().SetIsHiddenUntilFoundElement(true);
        } else if (DisplayLockContext* context = GetDisplayLockContext()) {
          context->SetIsHiddenUntilFoundElement(false);
        }
      } else if (name == html_names::kInertAttr) {
        UseCounter::Count(GetDocument(), WebFeature::kInertAttribute);
      }
      // NOTE: We could test here if we have a shared ElementData
      // with presentation attribute style, and avoid re-dirtying
      // if so, but somehow, that benchmarks poorly, so we don't do it.
      // It's fairly rare that this would save a lot of time anyway.
      GetElementData()->SetPresentationAttributeStyleIsDirty(true);
      SetNeedsStyleRecalc(kLocalStyleChange,
                          StyleChangeReasonForTracing::FromAttribute(name));
    }
  }

  if (IsElementReflectionAttribute(name)) {
    SynchronizeContentAttributeAndElementReference(name);
    if (name == html_names::kInterestforAttr &&
        RuntimeEnabledFeatures::HTMLInterestForAttributeEnabled()) {
      UseCounter::Count(GetDocument(), WebFeature::kInterestFor);
      if (!params.old_value.IsNull()) {
        // We are changing the value of the `interestfor` attribute, so
        // ensure it doesn't have interest.
        ChangeInterestState(InterestForElement(), InterestState::kNoInterest);
      }
    }
  }

  InvalidateNodeListCachesInAncestors(&name, this, nullptr);

  if (isConnected()) {
    if (AXObjectCache* cache = GetDocument().ExistingAXObjectCache()) {
      if (params.old_value != params.new_value) {
        cache->HandleAttributeChanged(name, this);
      }
    }
  }
#endif
}
  
}