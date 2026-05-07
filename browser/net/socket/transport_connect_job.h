#ifndef NET_SOCKET_TRANSPORT_CONNECT_JOB_H_
#define NET_SOCKET_TRANSPORT_CONNECT_JOB_H_

#include "net/socket/connect_job.h"
#include <memory>
#include <vector>
#include <array>

namespace net {

class IPAddressBytes {
public:
  using IPAddressStorage = std::array<uint8_t, 16>;
  IPAddressStorage bytes_;
  uint8_t size_;
};

class IPAddress {
public:
  IPAddressBytes ip_address_;
};

class IPEndPoint {
public:
  IPAddress address_;
  uint16_t port_ = 0;
};

struct HostResolverEndpointResult {
  // IP endpoints at which to connect to the service.
  std::vector<net::IPEndPoint> ip_endpoints;
};

class TransportConnectJob;
class TransportConnectSubJob {
public:
  TransportConnectSubJob(std::vector<IPEndPoint> addresses,
                         TransportConnectJob* parent_job/*,
                         SubJobType type*/);
  int Start();
};

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

  enum SubJobType { SUB_JOB_IPV4, SUB_JOB_IPV6 };
protected:
  int ConnectInternal() override;
  void DoResolveHost();
  void DoTransportConnect();

private:
  std::vector<HostResolverEndpointResult> m_endpoint_results;

  std::unique_ptr<TransportConnectSubJob> ipv4_job_;
  // std::unique_ptr<TransportConnectSubJob> ipv6_job_;
};

}

#endif