#include "net/http/http_network_transaction.h"
#include "net/http/http_network_session.h"

namespace net {

HttpNetworkTransaction::HttpNetworkTransaction(HttpNetworkSession *session)
    : m_session(session) {
  io_callback_ = std::bind(&HttpNetworkTransaction::OnIOComplete, this,
                           std::placeholders::_1);
}

int HttpNetworkTransaction::Start(const HttpRequestInfo* request_info, CompletionOnceCallback callback) {
  request_ = request_info;

  DoCreateStream();
  // DoInitStream();

  //  if (rv == ERR_IO_PENDING)
  //   callback_ = std::move(callback);
  return 0;
}

// void HttpNetworkTransaction::SetRequestHeadersCallback(
//     RequestHeadersCallback callback) {
//   DCHECK(!stream_);
//   request_headers_callback_ = std::move(callback);
// }

void HttpNetworkTransaction::DoCreateStream() {
  stream_request_ = m_session->http_stream_factory()->RequestStream(
      *request_, static_cast<HttpStreamRequest::Delegate *>(this));
}

//
//  	net.dll!net::HttpBasicStream::InitializeStream	C++
// >	net.dll!net::HttpNetworkTransaction::DoInitStream	C++
//  	net.dll!net::HttpNetworkTransaction::DoLoop	C++
//  	net.dll!net::HttpNetworkTransaction::OnIOComplete	C++
//  	net.dll!net::HttpNetworkTransaction::OnStreamReady	C++
//  	net.dll!net::HttpStreamFactory::JobController::OnStreamReady	C++
//  	net.dll!net::HttpStreamFactory::Job::OnStreamReadyCallback	C++
//
void HttpNetworkTransaction::DoInitStream() {
  // stream_request_->InitializeStream();
  stream_->InitializeStream();
}

void HttpNetworkTransaction::OnStreamReady(/*const ProxyInfo& used_proxy_info,*/
                                           std::unique_ptr<HttpStream> stream) {
  stream_ = std::move(stream);

  // stream_->SetRequestHeadersCallback(request_headers_callback_);
  // proxy_info_ = used_proxy_info;

  // OnIOComplete(OK);
  //   DoCreateStreamComplete();
  DoConnectedCallback();
  //
  //     URLLoader::OnConnected
  //     URLRequest::NotifyConnected
  //     URLRequestHttpJob::NotifyConnectedCallback
  // return connected_callback_.Run();


  // DoConnectedCallbackComplete();

  DoInitStream();
  // DoInitStreamComplete();
  // DoGenerateProxyAuthToken();
  // DoGenerateProxyAuthTokenComplete();
  // DoGenerateServerAuthToken();
  // DoGenerateServerAuthTokenComplete();
  // DoInitRequestBody(); // upload_data_stream
  // DoInitRequestBodyComplete();
  DoBuildRequest();
  // DoBuildRequestComplete();
  int result = DoSendRequest();
  DoSendRequestComplete(result);
 
  result = DoReadHeaders();
  DoReadHeadersComplete(result);
}

void HttpNetworkTransaction::DoConnectedCallback() {
  stream_->RegisterRequest(request_);
}

void HttpNetworkTransaction::DoBuildRequest() {
  BuildRequestHeaders(false);
}

int HttpNetworkTransaction::BuildRequestHeaders(
    bool using_http_proxy_without_tunnel) {
#if 0
  request_headers_.SetHeader(HttpRequestHeaders::kHost,
                             GetHostAndOptionalPort(request_->url));

  // For compat with HTTP/1.0 servers and proxies:
  if (using_http_proxy_without_tunnel) {
    request_headers_.SetHeader(HttpRequestHeaders::kProxyConnection,
                               "keep-alive");
  } else {
    request_headers_.SetHeader(HttpRequestHeaders::kConnection, "keep-alive");
  }

  // Add a content length header?
  if (request_->upload_data_stream) {
    if (request_->upload_data_stream->is_chunked()) {
      request_headers_.SetHeader(HttpRequestHeaders::kTransferEncoding,
                                 "chunked");
    } else {
      request_headers_.SetHeader(
          HttpRequestHeaders::kContentLength,
          base::NumberToString(request_->upload_data_stream->size()));
    }
  } else if (request_->method == "POST" || request_->method == "PUT") {
    // An empty POST/PUT request still needs a content length.  As for HEAD,
    // IE and Safari also add a content length header.  Presumably it is to
    // support sending a HEAD request to an URL that only expects to be sent a
    // POST or some other method that normally would have a message body.
    // Firefox (40.0) does not send the header, and RFC 7230 & 7231
    // specify that it should not be sent due to undefined behavior.
    request_headers_.SetHeader(HttpRequestHeaders::kContentLength, "0");
  }

  // Honor load flags that impact proxy caches.
  if (request_->load_flags & LOAD_BYPASS_CACHE) {
    request_headers_.SetHeader(HttpRequestHeaders::kPragma, "no-cache");
    request_headers_.SetHeader(HttpRequestHeaders::kCacheControl, "no-cache");
  } else if (request_->load_flags & LOAD_VALIDATE_CACHE) {
    request_headers_.SetHeader(HttpRequestHeaders::kCacheControl, "max-age=0");
  }

  if (ShouldApplyProxyAuth() && HaveAuth(HttpAuth::AUTH_PROXY))
    auth_controllers_[HttpAuth::AUTH_PROXY]->AddAuthorizationHeader(
        &request_headers_);
  if (ShouldApplyServerAuth() && HaveAuth(HttpAuth::AUTH_SERVER))
    auth_controllers_[HttpAuth::AUTH_SERVER]->AddAuthorizationHeader(
        &request_headers_);

  request_headers_.MergeFrom(request_->extra_headers);

  if (modify_headers_callbacks_) {
    modify_headers_callbacks_.Run(&request_headers_);
  }

  response_.did_use_server_http_auth =
      request_headers_.HasHeader(HttpRequestHeaders::kAuthorization);
#endif
  return OK;
}

int HttpNetworkTransaction::DoSendRequest() {
  // stream_->SetRequestIdempotency(request_->idempotency);
  return stream_->SendRequest(request_headers_/*, &response_, io_callback_*/);
}

int HttpNetworkTransaction::DoSendRequestComplete(int result) {
  assert (result >= 0);
  return 0;
}

int HttpNetworkTransaction::DoReadHeaders() {
  // next_state_ = STATE_READ_HEADERS_COMPLETE;
  return stream_->ReadResponseHeaders(io_callback_);
}

void HttpNetworkTransaction::SetResponseHeadersCallback(
    ResponseHeadersCallback callback) {
  response_headers_callback_ = std::move(callback);
}

void HttpNetworkTransaction::DoReadHeadersComplete(int result) {
  if (result == ERR_SSL_CLIENT_AUTH_CERT_NEEDED) {
    assert(false);
  }

  if (result == ERR_HTTP_1_1_REQUIRED ||
      result == ERR_PROXY_HTTP_1_1_REQUIRED) {
    assert(false);
  }

  if (result < 0) {
    assert(false);
    // return HandleIOError(result);
  }
  if (response_headers_callback_)
    response_headers_callback_(response_.headers);

  return;
}

void HttpNetworkTransaction::OnIOComplete(int result){
  // HttpCache::Transaction::OnIOComplete
  // std::move(callback_)(result);

  // int rv = DoLoop(result);
  // if (rv != ERR_IO_PENDING)
    DoCallback(result);
}

int HttpNetworkTransaction::Read(IOBuffer *buf, int buf_len,
                                 CompletionOnceCallback callback) {
  read_buf_ = buf;
  read_buf_len_ = buf_len;

  int result = DoReadBody();
  if (result == ERR_IO_PENDING) {
    callback_ = std::move(callback);
    return result;
  }

  return DoReadBodyComplete(result);
}

int HttpNetworkTransaction::DoReadBody() {
  return stream_->ReadResponseBody(
      read_buf_, read_buf_len_, io_callback_);
}

int HttpNetworkTransaction::DoReadBodyComplete(int result) {
  return 0;
}

// 
// >	net.dll!net::HttpNetworkTransaction::DoCallback	C++
//  	net.dll!net::HttpNetworkTransaction::OnIOComplete	C++
//  	net.dll!net::HttpNetworkTransaction::OnStreamReady	C++
//  	net.dll!net::HttpStreamFactory::JobController::OnStreamReady	C++
//  	net.dll!net::HttpStreamFactory::Job::OnStreamReadyCallback	C++
// 
void HttpNetworkTransaction::DoCallback(int rv) {
    std::move(callback_)(rv);
}

}