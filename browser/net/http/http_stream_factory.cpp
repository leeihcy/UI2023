#include "net/http/http_stream_factory.h"
#include "net/http/http_basic_stream.h"
#include "net/http/http_network_session.h"

namespace net {

HttpStreamFactory::HttpStreamFactory(HttpNetworkSession *session)
    : m_session(session),
      m_job_factory(std::make_unique<HttpStreamFactory::JobFactory>()) {}

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

  auto job_controller = std::make_unique<JobController>(m_session, m_job_factory.get());
  JobController *job_controller_raw_ptr = job_controller.get();
  job_controller_set_.insert(std::move(job_controller));
  return job_controller_raw_ptr->Start();
}


std::unique_ptr<HttpStreamRequest>  HttpStreamFactory::JobController::Start() {
  auto request = std::make_unique<HttpStreamRequest>();
  m_request = request.get();  // 临时引用

  // 1. 判断是否需要走代理
  DoResolveProxy();

  // 2. 创建Job
  DoCreateJobs();
  
  return request;
}

void HttpStreamFactory::JobController::DoResolveProxy() {
  // TODO:
  // proxy_info_.UseDirect();
}

//
// 该函数的核心目标是：
// . 判断是否需要启动新的连接任务（Job）。
// . 创建不同类型的 Job：通常包括一个 MAIN_JOB 和一个或零个 ALTERNATIVE_JOB。
// . 启动竞速：让这些 Job 同时发起连接，谁先成功建立连接，谁就被用来发送请求。
//
void HttpStreamFactory::JobController::DoCreateJobs() {
  m_main_job = m_job_factory->CreateJob(m_session);

  // if (alternative_job_) {
  //   alternative_job_->Start(request_->stream_type());
  // }

  // if (dns_alpn_h3_job_) {
  //   dns_alpn_h3_job_->Start(request_->stream_type());
  // }

  if (m_main_job) {
    m_main_job->Start(/*m_request->stream_type()*/);
  }
}

HttpStreamFactory::Job::Job(HttpNetworkSession *session)
    : m_session(session), m_connection(std::make_unique<ClientSocketHandle>()) {

}

void HttpStreamFactory::Job::Start(/*HttpStreamRequest::StreamType stream_type*/) {
	DoInitConnection();
	DoCreateStream();
}

void HttpStreamFactory::Job::DoInitConnection() {
  // return InitSocketHandleForHttpRequest();
  // return InitSocketPoolHelper(m_connection.get());

  ClientSocketPool *pool = m_session->GetSocketPool(
      /*socket_pool_type, proxy_info.proxy_chain()*/);
  return m_connection->Init(pool);
}

void HttpStreamFactory::Job::DoCreateStream() {
	m_stream = std::make_unique<HttpBasicStream>();
}

}