#ifndef _HTML_CSS_RESOLVER_CASCADE_RESOLVER_H_
#define _HTML_CSS_RESOLVER_CASCADE_RESOLVER_H_

#include "html/css/resolver/cascade_filter.h"
#include <stdint.h>

namespace html {

class CascadeResolver {
public:
  CascadeResolver(CascadeFilter filter, uint8_t generation)
      : filter_(filter), generation_(generation) {}

  CascadeFilter filter_;
  const uint8_t generation_ = 0;
};

}

#endif