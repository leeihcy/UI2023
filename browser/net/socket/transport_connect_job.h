#ifndef NET_SOCKET_TRANSPORT_CONNECT_JOB_H_
#define NET_SOCKET_TRANSPORT_CONNECT_JOB_H_

#include "net/socket/connect_job.h"
#include <memory>

namespace net {

class TransportConnectJob : public ConnectJob {
public:
  class Factory {
    public:
      virtual std::unique_ptr<TransportConnectJob> Create() {
        return std::make_unique<TransportConnectJob>();
      }
  };
public:
  TransportConnectJob();

protected:
  int ConnectInternal() override;
};

}

#endif