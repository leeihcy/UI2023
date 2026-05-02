#include "net/http/http_stream_factory.h"

namespace net {

std::unique_ptr<HttpStreamRequest> HttpStreamFactory::RequestStream() {
  // 这个函数内部很复杂。会涉及DNS的解析。
  /*
  net.dll!net::HostResolverManager::ResolveLocally
 	net.dll!net::HostResolverManager::RequestImpl::DoResolveLocally
 	net.dll!net::HostResolverManager::RequestImpl::DoLoop
 	net.dll!net::HostResolverManager::RequestImpl::Start
 	net.dll!net::TransportConnectJob::DoResolveHost
 	net.dll!net::TransportConnectJob::DoLoop
 	net.dll!net::TransportConnectJob::ConnectInternal
 	net.dll!net::ConnectJob::Connect
 	net.dll!net::HttpProxyConnectJob::DoTransportConnect
 	net.dll!net::HttpProxyConnectJob::DoLoop
 	net.dll!net::HttpProxyConnectJob::ConnectInternal
 	net.dll!net::ConnectJob::Connect
 	net.dll!net::SSLConnectJob::DoTunnelConnect
 	net.dll!net::SSLConnectJob::DoLoop
 	net.dll!net::SSLConnectJob::ConnectInternal
 	net.dll!net::ConnectJob::Connect
 	net.dll!net::TransportClientSocketPool::RequestSocketInternal
 	net.dll!net::TransportClientSocketPool::RequestSocket
 	net.dll!net::ClientSocketHandle::Init
 	net.dll!net::`anonymous namespace'::InitSocketPoolHelper
 	net.dll!net::InitSocketHandleForHttpRequest
 	net.dll!net::HttpStreamFactory::Job::DoInitConnectionImpl
 	net.dll!net::HttpStreamFactory::Job::DoInitConnection
 	net.dll!net::HttpStreamFactory::Job::DoLoop
 	net.dll!net::HttpStreamFactory::Job::RunLoop
 	net.dll!net::HttpStreamFactory::Job::StartInternal
 	net.dll!net::HttpStreamFactory::Job::Start
 	net.dll!net::HttpStreamFactory::JobController::DoCreateJobs
 	net.dll!net::HttpStreamFactory::JobController::DoLoop
 	net.dll!net::HttpStreamFactory::JobController::RunLoop
 	net.dll!net::HttpStreamFactory::JobController::Start
 	net.dll!net::HttpStreamFactory::RequestStreamInternal
	net.dll!net::HttpStreamFactory::RequestStream
 	net.dll!net::HttpNetworkTransaction::DoCreateStream
 	net.dll!net::HttpNetworkTransaction::DoLoop
  */

  auto job_controller = std::make_unique<JobController>();
  JobController *job_controller_raw_ptr = job_controller.get();
  job_controller_set_.insert(std::move(job_controller));
  return job_controller_raw_ptr->Start();
}


std::unique_ptr<HttpStreamRequest>  HttpStreamFactory::JobController::Start() {
  return std::make_unique<HttpStreamRequest>();
}

}