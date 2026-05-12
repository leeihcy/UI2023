#include "net/socket/transport_connect_job.h"
#include <assert.h>
#include "net/socket/tcp_client_socket.h"

namespace net {

TransportConnectJob::TransportConnectJob(const std::shared_ptr<TransportSocketParams>& params) : params_(params){

}

int TransportConnectJob::ConnectInternal() {
  DoResolveHost();
  DoTransportConnect();
  return 0;
}

void TransportConnectJob::DoResolveHost() {
  // 目前先跳过，用IP进行测试
  // params_->destination();
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
                         SubJobType type*/) :addresses_(std::move(addresses)) {

}
int TransportConnectSubJob::Start() {
  AddressList one_address(CurrentAddress());
  transport_socket_ = std::make_unique<TCPClientSocket>(one_address);

  return transport_socket_->Connect(
    std::bind(&TransportConnectSubJob::OnIOComplete, this, std::placeholders::_1));
  return 0;
}

void TransportConnectSubJob::OnIOComplete(int result) {
  // int rv = DoLoop(result);
  // if (rv != ERR_IO_PENDING)
  //   parent_job_->OnSubJobComplete(rv, this);  // |this| deleted
}

const IPEndPoint& TransportConnectSubJob::CurrentAddress() const {
  return addresses_[current_address_index_];
}

}