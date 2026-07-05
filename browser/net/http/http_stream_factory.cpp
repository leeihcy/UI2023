#include "net/http/http_stream_factory.h"
#include "net/http/http_basic_stream.h"
#include "net/http/http_network_session.h"

namespace net {

HttpStreamFactory::HttpStreamFactory(HttpNetworkSession *session)
    : m_session(session),
      m_job_factory(std::make_unique<HttpStreamFactory::JobFactory>()) {}

std::unique_ptr<HttpStreamRequest>
HttpStreamFactory::RequestStream(const HttpRequestInfo &request_info,
                                 HttpStreamRequest::Delegate *delegate) {
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

  auto job_controller = std::make_unique<JobController>(delegate, m_session, m_job_factory.get(), request_info);
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
 
	// 当前请求是否走新的 HttpStreamPool 架构，而不是继续走老的 Job/ConnectJob/SocketPool 创建链路。
	// 旧路径：(状态机太大、HTTP/1 HTTP/2 HTTP/3逻辑重复、连接复用能力差)
	// JobController
  //     │
  //     ├─ Main Job
  //     │     │
  //     │     ├─ DNS
  //     │     ├─ TCP
  //     │     └─ SSL
  //     │
  //     └─ Alternative Job
  //            │
  //            └─ QUIC
  
  // 新路径：（可通过 --disable-features=HappyEyeballsV3 禁用）
	// JobController
	// 			│
	// 			▼
	// HttpStreamPool
	// 			│
	// 			▼
	// AttemptManager
	// 			│
	// 			├─ TCP Attempt
	// 			├─ QUIC Attempt
	// 			└─ HappyEyeballs Attempt

	// SwitchToHttpStreamPool();

  url::SchemeHostPort destination(request_info_.url);
  // DCHECK(destination.IsValid());
  // ConvertWsToHttp(destination);

  m_main_job = m_job_factory->CreateJob(this, m_session, std::move(destination));

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

HttpStreamFactory::Job::Job(Delegate *delegate, HttpNetworkSession *session,
                            url::SchemeHostPort destination)
    : delegate_(delegate), m_session(session),
      destination_(std::move(destination)),
      m_connection(std::make_unique<ClientSocketHandle>()) {}

void HttpStreamFactory::Job::Start(/*HttpStreamRequest::StreamType stream_type*/) {
	DoInitConnection();
}

int HttpStreamFactory::Job::DoInitConnection() {
  // return InitSocketHandleForHttpRequest();
  // return InitSocketPoolHelper(m_connection.get());

  ClientSocketPool *pool = m_session->GetSocketPool(
      /*socket_pool_type, proxy_info.proxy_chain()*/);

  ClientSocketPool::GroupId connection_group;
  connection_group.destination_ = destination_;
  return m_connection->Init(connection_group,
                            std::bind(&HttpStreamFactory::Job::OnIOComplete,
                                      this, std::placeholders::_1),
                            pool);
}

void HttpStreamFactory::Job::OnIOComplete(int result) {
  DoCreateStream();
	OnStreamReadyCallback();
}

void HttpStreamFactory::Job::DoCreateStream() {
	m_stream = std::make_unique<HttpBasicStream>(std::move(m_connection));
}

void HttpStreamFactory::Job::OnStreamReadyCallback() {
	delegate_->OnStreamReady(this);
}

void HttpStreamFactory::JobController::OnStreamReady(Job *job) {
  std::unique_ptr<HttpStream> stream = job->ReleaseStream();
  // MarkRequestComplete(job);
  OnJobSucceeded(job);

  // HttpNetworkTransaction::OnStreamReady
  delegate_->OnStreamReady(/*job->proxy_info(),*/ std::move(stream));
}

void HttpStreamFactory::JobController::OnJobSucceeded(Job *job) {
  if (!bound_job_) {
    BindJob(job);
    return;
  }
}

void HttpStreamFactory::JobController::BindJob(Job* job) {
  job_bound_ = true;
  bound_job_ = job;

  // OrphanUnboundJob();
}

}