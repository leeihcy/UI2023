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


void RequestContext::AddWatchNotifyFinalizer(std::shared_ptr<Watch> watch,
                                             MojoResult result,
                                             const HandleSignalsState& state) {
  // DCHECK(IsCurrent());
  // watch_notify_finalizers_.push_back(
  //     WatchNotifyFinalizer(std::move(watch), result, state));
}

void RequestContext::AddWatchCancelFinalizer(std::shared_ptr<Watch> watch) {
  // DCHECK(IsCurrent());
  // watch_cancel_finalizers_.push_back(std::move(watch));
}


}
}