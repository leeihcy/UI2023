#ifndef NET_SOCKET_TRANSPORT_CONNECT_JOB_H_
#define NET_SOCKET_TRANSPORT_CONNECT_JOB_H_

#include "net/socket/connect_job.h"
#include <array>
#include <memory>
#include <vector>
#include <variant>
#include "net/socket/stream_socket.h"
#include "net/base/ip_endpoint.h"
#include "net/base/host_port_pair.h"
#include "url/scheme_host_port.h"

namespace net {


struct HostResolverEndpointResult {
  // IP endpoints at which to connect to the service.
  std::vector<net::IPEndPoint> ip_endpoints;
};

class TransportSocketParams {
public:
  using Endpoint = std::variant<url::SchemeHostPort, HostPortPair>;

  const Endpoint& destination() const { return destination_; }
  Endpoint destination_;
};

class TransportConnectJob;

// 分为IPV4和IPV6两个类型。
class TransportConnectSubJob {
public:
  TransportConnectSubJob(std::vector<IPEndPoint> addresses,
                         TransportConnectJob* parent_job/*,
                         SubJobType type*/);
  int Start();
    
  const IPEndPoint& CurrentAddress() const;
  void OnIOComplete(int result);
  
  std::unique_ptr<StreamSocket> PassSocket() {
    return std::move(transport_socket_);
  }

private:
  TransportConnectJob* parent_job_;
  
  std::unique_ptr<StreamSocket> transport_socket_;
  std::vector<IPEndPoint> addresses_;
  size_t current_address_index_ = 0;
};

class TransportConnectJob : public ConnectJob {
public:
  class Factory {
    public:
      virtual std::unique_ptr<TransportConnectJob> Create(const std::shared_ptr<TransportSocketParams>& params,  Delegate* delegate) {
        return std::make_unique<TransportConnectJob>(params, delegate);
      }
  };
public:
  TransportConnectJob(const std::shared_ptr<TransportSocketParams>& params, Delegate* delegate);

  enum SubJobType { SUB_JOB_IPV4, SUB_JOB_IPV6 };
  void OnSubJobComplete(int result, TransportConnectSubJob *job);

  void OnIOComplete(int result);

protected:
  int ConnectInternal() override;
  void DoResolveHost();
  void DoTransportConnect();

private:
  std::vector<HostResolverEndpointResult> endpoint_results_;

  std::shared_ptr<TransportSocketParams> params_;

  std::unique_ptr<TransportConnectSubJob> ipv4_job_;
  // std::unique_ptr<TransportConnectSubJob> ipv6_job_;
};

}

#endif