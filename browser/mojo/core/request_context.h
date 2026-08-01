#ifndef MOJO_CORE_REQUEST_CONTEXT_H_
#define MOJO_CORE_REQUEST_CONTEXT_H_

#include "mojo/mojo_export.h"

namespace mojo {
namespace core {

class MOJO_EXPORT RequestContext {
public:
  // Identifies the source of the current stack frame's RequestContext.
  enum class Source {
    LOCAL_API_CALL,
    SYSTEM,
  };

  // Constructs a RequestContext with a LOCAL_API_CALL Source.
  RequestContext();

  explicit RequestContext(Source source);

  RequestContext(const RequestContext&) = delete;
  RequestContext& operator=(const RequestContext&) = delete;

  ~RequestContext();

  // Returns the current thread-local RequestContext.
  static RequestContext* current();

  Source source() const { return source_; }
  
 private:
  // Is this request context the current one?
  bool IsCurrent() const;

private:
  const Source source_;
};

}
}

#endif