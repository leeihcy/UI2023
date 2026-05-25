#include "net/socket/connect_job.h"

namespace net {

ConnectJob::ConnectJob(Delegate* delegate) : delegate_(delegate) {
  
}

void ConnectJob::SetSocket(std::unique_ptr<StreamSocket> socket/*,
                 std::optional<std::set<std::string>> dns_aliases*/) {
  socket_ = std::move(socket);
}

void ConnectJob::NotifyDelegateOfCompletion(int rv) {
  // The delegate will own |this|.
  Delegate* delegate = delegate_;
  delegate_ = nullptr;

  // StopTimerAndLogConnectCompletion(rv);
  delegate->OnConnectJobComplete(rv, this);
}


} // namespace net