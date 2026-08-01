#include "mojo/core/request_context.h"

namespace mojo {
namespace core {

namespace {

constinit thread_local RequestContext* current_context = nullptr;

}  // namespace


RequestContext::RequestContext() : RequestContext(Source::LOCAL_API_CALL) {}

RequestContext::RequestContext(Source source) : source_(source) {
  // We allow nested RequestContexts to exist as long as they aren't actually
  // used for anything.
  if (!current_context) {
    current_context = this;
  }
}

RequestContext::~RequestContext() {
  if (IsCurrent()) {
    current_context = nullptr;
  }
}


// static
RequestContext* RequestContext::current() {
  // DCHECK(current_context);
  return current_context;
}

bool RequestContext::IsCurrent() const {
  return current_context == this;
}


}
}