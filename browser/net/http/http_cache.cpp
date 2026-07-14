#include "net/http/http_cache.h"

#include <assert.h>
#include "base/check_op.h"
#include "net/base/net_errors.h"


namespace  net {

HttpCache::HttpCache(std::unique_ptr<HttpTransactionFactory> network_layer)
    : m_network_layer(std::move(network_layer)) {}

std::unique_ptr<HttpTransaction> HttpCache::CreateTransaction() {

 auto new_transaction =
      std::make_unique<HttpCache::Transaction>(/*priority, */this);

  return new_transaction;
}

HttpNetworkSession* HttpCache::GetSession() {
  return m_network_layer->GetSession();
}

HttpCache::Transaction::Transaction(HttpCache* cache) : m_cache(cache) {
  io_callback_ = std::bind(&Transaction::OnIOComplete, this, std::placeholders::_1);
}

int HttpCache::Transaction::Start(const HttpRequestInfo *request,
                                  CompletionOnceCallback callback) {
  initial_request_ = request;
  request_ = initial_request_;

  // TODO: Load Cache

  // We have to wait until the backend is initialized so we start the SM.
  //next_state_ = STATE_GET_BACKEND;
  next_state_ = STATE_SEND_REQUEST;
  int rv = DoLoop(OK);

  // Setting this here allows us to check for the existence of a callback_ to
  // determine if we are still inside Start.
  if (rv == ERR_IO_PENDING) {
      callback_ = std::move(callback);
  }

  return rv;
}

int HttpCache::Transaction::DoLoop(int result) {
  int rv = result;
  State state = next_state_;
  do {
    state = next_state_;
    next_state_ = STATE_UNSET;
    // base::AutoReset<bool> scoped_in_do_loop(&in_do_loop_, true);

    switch (state) {
    case STATE_SEND_REQUEST:
      DCHECK_EQ(OK, rv);
      rv = DoSendRequest();
      break;
    case STATE_SEND_REQUEST_COMPLETE:
      next_state_ = STATE_SUCCESSFUL_SEND_REQUEST;
      break;
    case STATE_SUCCESSFUL_SEND_REQUEST:
        DCHECK_EQ(OK, rv);
        //rv = DoSuccessfulSendRequest();
        next_state_ = STATE_FINISH_HEADERS;
        break;
    case STATE_FINISH_HEADERS:
        //rv = DoFinishHeaders(rv);
        next_state_ = STATE_FINISH_HEADERS_COMPLETE;
        break;
    case STATE_FINISH_HEADERS_COMPLETE:
        //rv = DoFinishHeadersComplete(rv);
        // next_state_ = STATE_NETWORK_READ;
        next_state_ = STATE_NONE;
        break;
    case STATE_NETWORK_READ:
        DCHECK_EQ(OK, rv);
        rv = DoNetworkRead();
        break;
    default:
      //assert(false);
        break;
    }
  } while (rv != ERR_IO_PENDING && next_state_ != STATE_NONE);

  // if (rv != ERR_IO_PENDING && !callback_.is_null()) {
  //     read_buf_ = nullptr;  // Release the buffer before invoking the
  //     callback. std::move(callback_).Run(rv); callback_ = nullptr;
  // }
  return rv;
}

void HttpCache::Transaction::SetResponseHeadersCallback(
    ResponseHeadersCallback callback) {
  response_headers_callback_ = std::move(callback);
}

int HttpCache::Transaction::DoSendRequest() {
   m_network_trans = m_cache->m_network_layer->CreateTransaction(/*priority_*/);

   m_network_trans->SetResponseHeadersCallback(response_headers_callback_);

   next_state_ = STATE_SEND_REQUEST_COMPLETE;
   int rv = m_network_trans->Start(request_, io_callback_);
   return rv;
}

int HttpCache::Transaction::DoNetworkRead() {
  assert(false);
  return 0;
    // next_state_ = STATE_NETWORK_READ_COMPLETE;
    // return m_network_trans->Read(read_buf_.get(), read_buf_len_, io_callback_);
}

int HttpCache::Transaction::Read(IOBuffer* buf,
                                 int buf_len,
                                 CompletionOnceCallback callback) {
  return m_network_trans->Read(buf, buf_len, callback);
}

void HttpCache::Transaction::OnIOComplete(int result) {
   DoLoop(result);
}

}