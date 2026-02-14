#ifndef _HTML_DOM_NODE_H_
#define _HTML_DOM_NODE_H_

#include <cstdint>

namespace html {

class Node // : public EnentTarget
{
public:
  enum NodeType {
    kElementNode = 1,
  };

  enum NodeFlags : uint32_t {
  };

  enum class ElementNamespaceType : uint32_t {
    kHTML = 0,
    kMathML = 1, // << kElementNamespaceTypeShift,
    kSVG = 2,
    kOther = 3,
  };
  inline ElementNamespaceType GetElementNamespaceType() const {
    // return static_cast<ElementNamespaceType>(node_flags & kElementNamespaceTypeMask);
    return m_namespace_type;
  }

  inline bool IsHTMLElement() const {
    return GetElementNamespaceType() == ElementNamespaceType::kHTML;
  }
  inline bool IsMathMLElement() const {
    return GetElementNamespaceType() == ElementNamespaceType::kMathML;
  }
  inline bool IsSVGElement() const {
    return GetElementNamespaceType() == ElementNamespaceType::kSVG;
  }


private:
  uint32_t node_flags = 0;
  ElementNamespaceType m_namespace_type = ElementNamespaceType::kHTML;
};

}

#endif