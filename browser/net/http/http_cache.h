#ifndef NET_HTTP_HTTP_CACHE_H_
#define NET_HTTP_HTTP_CACHE_H_
#include "net/http/http_transaction_factory.h"

namespace net {

class HttpCache : public HttpTransactionFactory {
public:
  class Transaction : public HttpTransaction {
  public:
    Transaction(HttpCache* cache);
    int Start(const HttpRequestInfo *request_info,
              CompletionOnceCallback callback) override;
    int Read(IOBuffer *buf, int buf_len,
             CompletionOnceCallback callback) override;
    void SetResponseHeadersCallback(ResponseHeadersCallback callback) override;

  private:
    int DoLoop(int result);
    int DoSendRequest();
    int DoNetworkRead();
    void OnIOComplete(int result);
  
  private:
    enum State {
      STATE_UNSET,

      // Normally, states are traversed in approximately this order.
      STATE_NONE,
      STATE_GET_BACKEND,
      STATE_GET_BACKEND_COMPLETE,
      STATE_INIT_ENTRY,
      STATE_OPEN_OR_CREATE_ENTRY,
      STATE_OPEN_OR_CREATE_ENTRY_COMPLETE,
      STATE_DOOM_ENTRY,
      STATE_DOOM_ENTRY_COMPLETE,
      STATE_CREATE_ENTRY,
      STATE_CREATE_ENTRY_COMPLETE,
      STATE_ADD_TO_ENTRY,
      STATE_ADD_TO_ENTRY_COMPLETE,
      STATE_DONE_HEADERS_ADD_TO_ENTRY_COMPLETE,
      STATE_CACHE_READ_RESPONSE,
      STATE_CACHE_READ_RESPONSE_COMPLETE,
      STATE_WRITE_UPDATED_PREFETCH_RESPONSE,
      STATE_WRITE_UPDATED_PREFETCH_RESPONSE_COMPLETE,
      STATE_CACHE_DISPATCH_VALIDATION,
      STATE_CACHE_QUERY_DATA,
      STATE_CACHE_QUERY_DATA_COMPLETE,
      STATE_START_PARTIAL_CACHE_VALIDATION,
      STATE_COMPLETE_PARTIAL_CACHE_VALIDATION,
      STATE_CACHE_UPDATE_STALE_WHILE_REVALIDATE_TIMEOUT,
      STATE_CACHE_UPDATE_STALE_WHILE_REVALIDATE_TIMEOUT_COMPLETE,
      STATE_CONNECTED_CALLBACK,
      STATE_CONNECTED_CALLBACK_COMPLETE,
      STATE_SETUP_ENTRY_FOR_READ,
      STATE_SEND_REQUEST,
      STATE_SEND_REQUEST_COMPLETE,
      STATE_SUCCESSFUL_SEND_REQUEST,
      STATE_UPDATE_CACHED_RESPONSE,
      STATE_CACHE_WRITE_UPDATED_RESPONSE,
      STATE_CACHE_WRITE_UPDATED_RESPONSE_COMPLETE,
      STATE_UPDATE_CACHED_RESPONSE_COMPLETE,
      STATE_OVERWRITE_CACHED_RESPONSE,
      STATE_CACHE_WRITE_RESPONSE,
      STATE_CACHE_WRITE_RESPONSE_COMPLETE,
      STATE_TRUNCATE_CACHED_DATA,
      STATE_TRUNCATE_CACHED_DATA_COMPLETE,
      STATE_TRUNCATE_CACHED_METADATA,
      STATE_TRUNCATE_CACHED_METADATA_COMPLETE,
      STATE_PARTIAL_HEADERS_RECEIVED,
      STATE_HEADERS_PHASE_CANNOT_PROCEED,
      STATE_FINISH_HEADERS,
      STATE_FINISH_HEADERS_COMPLETE,

      // These states are entered from Read.
      STATE_NETWORK_READ_CACHE_WRITE,
      STATE_NETWORK_READ_CACHE_WRITE_COMPLETE,
      STATE_CACHE_READ_DATA,
      STATE_CACHE_READ_DATA_COMPLETE,
      // These states are entered if the request should be handled exclusively
      // by the network layer (skipping the cache entirely).
      STATE_NETWORK_READ,
      STATE_NETWORK_READ_COMPLETE,
    };
    State next_state_{STATE_NONE};

    HttpCache* m_cache;

    // Initial request with which Start() was invoked.
    const HttpRequestInfo* initial_request_ = nullptr;
    const HttpRequestInfo* request_ = nullptr;

    CompletionOnceCallback callback_;  // Consumer's callback.

    ResponseHeadersCallback response_headers_callback_;
    CompletionRepeatingCallback io_callback_;

    std::unique_ptr<HttpTransaction> m_network_trans;
  };

public:
  HttpCache(std::unique_ptr<HttpTransactionFactory> network_layer);
  std::unique_ptr<HttpTransaction> CreateTransaction() override;
  HttpNetworkSession* GetSession() override;
  
public:
  std::unique_ptr<HttpTransactionFactory> m_network_layer;
};

}

#endif