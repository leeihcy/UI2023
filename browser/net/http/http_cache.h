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
    int DoSendRequest();
    void OnIOComplete(int result);
  
  private:
    HttpCache* m_cache;

    // Initial request with which Start() was invoked.
    const HttpRequestInfo* initial_request_ = nullptr;
    const HttpRequestInfo* request_ = nullptr;

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