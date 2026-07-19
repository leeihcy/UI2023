#include "net/url_request/url_request_http_job.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_context.h"

namespace net {


std::unique_ptr<URLRequestJob> URLRequestHttpJob::Create(URLRequest* request) {
  const GURL& url = request->url();

  // Check for HSTS upgrade.
  return std::make_unique<URLRequestHttpJob>(request);
}

void URLRequestHttpJob::Start() {
  printf("Hello URL Request Http Job \n");

  request_info_.url = request_->url();
  request_info_.method = request_->method();

  // TOOD: GetCookie

  StartTransaction();
}

void URLRequestHttpJob::SetResponseHeadersCallback(
    ResponseHeadersCallback callback) {
  response_headers_callback_ = std::move(callback);
}

void URLRequestHttpJob::StartTransaction() {
  m_transaction =
      request_->context()->http_transaction_factory()->CreateTransaction();

  m_transaction->SetConnectedCallback(
      std::bind(&URLRequestHttpJob::NotifyConnectedCallback, this,
                std::placeholders::_1));
  // m_transaction->SetRequestHeadersCallback(request_headers_callback_);
  // m_transaction->SetEarlyResponseHeadersCallback(early_response_headers_callback_);
  m_transaction->SetResponseHeadersCallback(response_headers_callback_);

  m_transaction->Start(&request_info_,
                       std::bind(&URLRequestHttpJob::OnStartCompleted, this,
                                 std::placeholders::_1));
}

int URLRequestHttpJob::NotifyConnectedCallback(const TransportInfo& info) {
  return URLRequestJob::NotifyConnected(info/*, std::move(callback)*/);
}

/*
>	services_network_network_service.dll!network::cors::CorsURLLoader::OnReceiveResponse	C++
 	services_network_network_service.dll!network::URLLoader::SendResponseToClient	C++
 	services_network_network_service.dll!network::URLLoader::StartReading	C++
 	services_network_network_service.dll!network::URLLoader::ContinueOnResponseStarted	C++
 	services_network_network_service.dll!network::URLLoader::ProcessInboundSharedStorageInterceptorOnResponseStarted	C++
 	services_network_network_service.dll!network::URLLoader::OnResponseStarted	C++
 	net.dll!net::URLRequest::NotifyResponseStarted	C++
 	net.dll!net::URLRequestJob::NotifyFinalHeadersReceived	C++
 	net.dll!net::URLRequestJob::NotifyHeadersComplete	C++
 	net.dll!net::URLRequestHttpJob::NotifyHeadersComplete	C++
 	net.dll!net::URLRequestHttpJob::SaveCookiesAndNotifyHeadersComplete	C++
 	net.dll!net::URLRequestHttpJob::OnStartCompleted	C++  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   ...
 	net.dll!net::HttpCache::Transaction::DoLoop	C++
 	net.dll!net::HttpCache::Transaction::OnIOComplete	C++
   ...
 	net.dll!net::HttpNetworkTransaction::DoCallback	C++
 	net.dll!net::HttpNetworkTransaction::OnIOComplete	C++
   ...
 	net.dll!net::HttpStreamParser::OnIOComplete	C++
   ...
 	net.dll!net::TCPClientSocket::DidCompleteReadWrite	C++
 	net.dll!net::TCPClientSocket::DidCompleteRead	C++
   ...
 	net.dll!net::TCPSocketDefaultWin::RetryRead	C++
   ...
 	net.dll!net::TCPSocketDefaultWin::DidSignalRead	C++
 	net.dll!net::TCPSocketDefaultWin::CoreImpl::ReadDelegate::OnObjectSignaled	C++
 	base.dll!base::win::ObjectWatcher::Signal	C++
 	base.dll!base::internal::DecayedFunctorTraits<...>::Invoke<...>	C++
 	base.dll!base::internal::InvokeHelper<...>::MakeItSo<...>	C++
 	base.dll!base::internal::Invoker<...>::RunImpl<...>	C++
 	base.dll!base::internal::Invoker<...>::Run	C++
 	base.dll!base::OnceCallback<...>::Run	C++
 	base.dll!base::TaskAnnotator::RunTaskImpl	C++
 	base.dll!base::TaskAnnotator::RunTask<...>	C++
 	base.dll!base::sequence_manager::internal::ThreadControllerWithMessagePumpImpl::DoWorkImpl	C++
 	base.dll!base::sequence_manager::internal::ThreadControllerWithMessagePumpImpl::DoWork	C++
 	base.dll!base::MessagePumpForIO::DoRunLoop	C++
 	base.dll!base::MessagePumpWin::Run	C++
 	base.dll!base::sequence_manager::internal::ThreadControllerWithMessagePumpImpl::Run	C++
 	base.dll!base::RunLoop::Run	C++
 	base.dll!base::Thread::Run	C++
 	content.dll!content::`anonymous namespace'::ChildIOThread::Run	C++
 	base.dll!base::Thread::ThreadMain	C++
 	base.dll!base::`anonymous namespace'::ThreadFunc	C++
 	kernel32.dll!00007ffc7cade8d7()	未知
 	ntdll.dll!00007ffc7e3cc3fc()	未知
*/
void URLRequestHttpJob::OnStartCompleted(int result) {
  // NetworkDelegate* network_delegate = request()->network_delegate();
  // if (network_delegate) {
  //  int error = network_delegate->NotifyHeadersReceived(
  //           request_,
  //           base::BindOnce(&URLRequestHttpJob::OnHeadersReceivedCallback,
  //                          weak_factory_.GetWeakPtr()),
  //           headers.get(), &override_response_headers_, endpoint,
  //           &preserve_fragment_on_redirect_url_, ssl_info);
  // }

  SaveCookiesAndNotifyHeadersComplete(OK);
}

void URLRequestHttpJob::SaveCookiesAndNotifyHeadersComplete(int result) {
  NotifyHeadersComplete();
}

void URLRequestHttpJob::NotifyHeadersComplete() {
  URLRequestJob::NotifyHeadersComplete();
}

int URLRequestHttpJob::ReadRawData(IOBuffer *buf, int buf_size) {
  return m_transaction->Read(buf, buf_size,
                             std::bind(&URLRequestHttpJob::OnReadCompleted,
                                       this, std::placeholders::_1));
}

void URLRequestHttpJob::OnReadCompleted(int result) {
   ReadRawDataComplete(result);
}

}