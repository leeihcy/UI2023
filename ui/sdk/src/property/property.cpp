#include "src/property/property.h"

namespace ui {

void freeValue(PropertyValue*& v, int flags) {
  if (flags & PF_MallocValue) {
    ::free(v);
  }
  v = nullptr;
}

Property::~Property() { freeValue(value, flags); }

}