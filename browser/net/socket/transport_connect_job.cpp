#include "net/socket/transport_connect_job.h"
#include <assert.h>

namespace net {

TransportConnectJob::TransportConnectJob() {

}

int TransportConnectJob::ConnectInternal() {
  DoResolveHost();
  DoTransportConnect();
  return 0;
}

void TransportConnectJob::DoResolveHost() {
  // 目前先跳过，用IP进行测试
}

void TransportConnectJob::DoTransportConnect() {
  if (m_endpoint_results.empty()) {
    assert(false);
  }

  std::vector<IPEndPoint> ipv4_addresses, ipv6_addresses;
  ipv4_addresses.push_back(m_endpoint_results[0].ip_endpoints[0]);

  ipv4_job_ = std::make_unique<TransportConnectSubJob>(
        std::move(ipv4_addresses), this/*, SUB_JOB_IPV4*/);
  ipv4_job_->Start();
}


TransportConnectSubJob::TransportConnectSubJob(std::vector<IPEndPoint> addresses,
                         TransportConnectJob* parent_job/*,
                         SubJobType type*/) {

}
int TransportConnectSubJob::Start() {
  //  transport_socket_ =
  //     parent_job_->client_socket_factory()->CreateTransportClientSocket(
  //         one_address);

  // return transport_socket_->Connect(base::BindOnce(
  //     &TransportConnectSubJob::OnIOComplete, base::Unretained(this)));
  return 0;
}

}