#ifndef _HTML_DOM_ELEMENT_H_
#define _HTML_DOM_ELEMENT_H_

#include "html/dom/node.h"
#include "html/base/memory.h"
#include "html/dom/element_data.h"
#include "html/dom/qualified_name.h"

namespace html {

class ContainerNode : public Node {
};

class Element : public ContainerNode {
public:
  Element() : m_element_data(std::move(A<ElementData>::make_new())){
  }
  // inline const SpaceSplitString& Element::ClassNames() const;
  // inline bool HasClassName(const AtomicString& class_name) const;
  inline bool HasID() const;
  // inline bool HasClass() const;

  inline const AtomicString &IdForStyleResolution() const;

  bool HasElementData() const { return m_element_data; }
  const ElementData* GetElementData() const { return m_element_data.get(); }
  

  enum class AttributeModificationReason {
    kDirectly,
    kByParser,
    kByCloning,
    kByMoveToNewDocument,
    kBySynchronizationOfLazyAttribute
  };
  struct AttributeModificationParams {
    // STACK_ALLOCATED();

   public:
    AttributeModificationParams(const QualifiedName& qname,
                                const AtomicString& old_value,
                                const AtomicString& new_value,
                                AttributeModificationReason reason)
        : name(qname),
          old_value(old_value),
          new_value(new_value),
          reason(reason) {}

    const QualifiedName& name;
    const AtomicString& old_value;
    const AtomicString& new_value;
    const AttributeModificationReason reason;
  };
  void AttributeChanged(const AttributeModificationParams &params);
  void DidAddAttribute(const QualifiedName &name, const AtomicString &value);
  void ParseAttribute(const AttributeModificationParams& params);

private:
  A<ElementData> m_element_data = nullptr;
};


inline bool Element::HasID() const {
  return HasElementData() && GetElementData()->HasID();
}

}

#endif