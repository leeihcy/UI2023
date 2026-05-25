#include "net/socket/connect_job_factory.h"

namespace net {

namespace  {
TransportSocketParams::Endpoint ToTransportEndpoint(
    const ConnectJobFactory::Endpoint& endpoint) {
  if (std::holds_alternative<url::SchemeHostPort>(endpoint)) {
    return std::get<url::SchemeHostPort>(endpoint);
  }

  DCHECK(
      std::holds_alternative<ConnectJobFactory::SchemelessEndpoint>(endpoint));
  return std::get<ConnectJobFactory::SchemelessEndpoint>(endpoint)
      .host_port_pair;
}

}

ConnectJobFactory::ConnectJobFactory() : m_transport_connect_job_factory(std::make_unique<TransportConnectJob::Factory>()) {

}

std::unique_ptr<ConnectJob> ConnectJobFactory::CreateConnectJob(
    const ConnectJobFactory::Endpoint &endpoint,
    ConnectJob::Delegate* delegate) {
  // TODO: 按JOB类型进行创建。
  std::shared_ptr<TransportSocketParams> transport_job_params = std::make_shared<TransportSocketParams>();
  transport_job_params->destination_ = ToTransportEndpoint(endpoint);
  
  return m_transport_connect_job_factory->Create(transport_job_params, delegate);
}
}