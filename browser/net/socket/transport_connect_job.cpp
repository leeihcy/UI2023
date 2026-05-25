#include "net/socket/transport_connect_job.h"
#include <assert.h>
#include "net/socket/tcp_client_socket.h"

namespace net {

TransportConnectJob::TransportConnectJob(
    const std::shared_ptr<TransportSocketParams> &params, Delegate *delegate)
    : ConnectJob(delegate), params_(params) {}

int TransportConnectJob::ConnectInternal() {
  DoResolveHost();
  DoTransportConnect();
  return 0;
}

void TransportConnectJob::DoResolveHost() {
  // 目前先跳过，用IP进行测试
  // params_->destination();

  std::vector<IPEndPoint> ip_endpoints;

  if (std::holds_alternative<url::SchemeHostPort>(params_->destination())) {
    url::SchemeHostPort host =
        std::get<url::SchemeHostPort>(params_->destination());
    IPEndPoint ip_endpoint(*IPAddress::FromIPLiteral(host.host_), host.port_);
    ip_endpoints.push_back(ip_endpoint);
  } else {
    net::HostPortPair host = std::get<HostPortPair>(params_->destination());
  }

  HostResolverEndpointResult new_result;
  new_result.ip_endpoints = std::move(ip_endpoints);
  // new_result.metadata = result.metadata;

  endpoint_results_.clear();
  endpoint_results_.push_back(std::move(new_result));
}

void TransportConnectJob::DoTransportConnect() {
  if (endpoint_results_.empty()) {
    assert(false);
  }

  std::vector<IPEndPoint> ipv4_addresses, ipv6_addresses;

  // TODO: 
  // 遍历所有的 endpoint.ip_endpoints 列表，这里简单取第一个。
  ipv4_addresses.push_back(endpoint_results_[0].ip_endpoints[0]);

  ipv4_job_ = std::make_unique<TransportConnectSubJob>(
        std::move(ipv4_addresses), this/*, SUB_JOB_IPV4*/);
  ipv4_job_->Start();
}

void TransportConnectJob::OnSubJobComplete(int result,
                                           TransportConnectSubJob *job) {
  // 处理IPV4和IPV6哪个更先成功就用哪个。
  // result = HandleSubJobComplete(result, job);

  if (result == OK) {
    SetSocket(job->PassSocket()/*, dns_aliases_*/);
  }
  if (result != ERR_IO_PENDING) {
    OnIOComplete(result);
  }
}

void TransportConnectJob::OnIOComplete(int result) {
  // result = DoLoop(result);
  if (result != ERR_IO_PENDING)
    NotifyDelegateOfCompletion(result);  // Deletes |this|
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
   int rv = result; // DoLoop(result);
   if (rv != ERR_IO_PENDING)
     parent_job_->OnSubJobComplete(rv, this);  // |this| deleted
}

const IPEndPoint& TransportConnectSubJob::CurrentAddress() const {
  return addresses_[current_address_index_];
}

}