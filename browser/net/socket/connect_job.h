#ifndef NET_SOCKET_CONNECT_JOB_H_
#define NET_SOCKET_CONNECT_JOB_H_

#include <memory>
#include "net/socket/stream_socket.h"

namespace net {

class ConnectJob {
public:
  class Delegate {
  public:
    virtual ~Delegate() = default;
    virtual void OnConnectJobComplete(int result, ConnectJob* job) = 0;
  };

  ConnectJob(Delegate*);
  ~ConnectJob();
  
  // Releases ownership of the underlying socket to the caller. Returns the
  // released socket, or nullptr if there was a connection error.
  std::unique_ptr<StreamSocket> PassSocket();
  StreamSocket* socket() { return socket_.get(); }

  int Connect() {
    return ConnectInternal();
  }
  void SetSocket(std::unique_ptr<StreamSocket> socket/*,
                 std::optional<std::set<std::string>> dns_aliases*/);
  void NotifyDelegateOfCompletion(int rv);

protected:
  virtual int ConnectInternal() = 0;

protected:
  std::unique_ptr<StreamSocket> socket_;
  Delegate* delegate_ = nullptr;
};


}

#endif