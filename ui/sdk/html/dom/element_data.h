#ifndef _HTML_DOM_ELEMENT_DATA_H_
#define _HTML_DOM_ELEMENT_DATA_H_

#include "html/base/atomic_string.h"
#include "html/dom/space_split_string.h"

namespace html {

class ElementData {
public: 
  bool HasID() const { return !id_for_style_resolution_.IsNull(); }
  // bool HasClass() const { return !class_names_.IsNull(); }

  const AtomicString& IdForStyleResolution() const {
    return id_for_style_resolution_;
  }
  AtomicString SetIdForStyleResolution(AtomicString new_id) const {
    return std::exchange(id_for_style_resolution_, std::move(new_id));
  }

private:
  mutable SpaceSplitString class_names_;
  mutable AtomicString id_for_style_resolution_;

};

}

#endif