#include "net/socket/connect_job_factory.h"

namespace net {

ConnectJobFactory::ConnectJobFactory() : m_transport_connect_job_factory(std::make_unique<TransportConnectJob::Factory>()) {

}

std::unique_ptr<ConnectJob> ConnectJobFactory::CreateConnectJob() 
{
  // TODO: 按JOB类型进行创建。

  return m_transport_connect_job_factory->Create();
}


}