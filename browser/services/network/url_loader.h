#ifndef SERVICES_NETWORK_URL_LOADER_H_
#define SERVICES_NETWORK_URL_LOADER_H_

#include <memory>
#include "net/url_request/url_request.h"
#include "net/url_request/url_request.h"
#include "net/base/io_buffer.h"
#include "services/network/public/mojom/url_loader.mojom.h"
#include "services/network/public/cpp/net_adapters.h"

namespace network {
class URLLoaderContext;
class ResourceRequest;

// 负责协调 调用方韩文 和 network进程 之间的数据流通
// URLRequest不负责跨进程，由URLLoader借助Mojo来完成。
class URLLoader : public net::URLRequest::Delegate {
public:
  URLLoader(URLLoaderContext &context, const ResourceRequest &request,
            mojom::URLLoaderClient* url_loader_client);
  void ScheduleStart();

  // net::URLRequest::Delegate
  int OnConnected(net::URLRequest *request, const net::TransportInfo &info) override;
  void OnResponseStarted(net::URLRequest* request, int net_error) override;
  void OnReadCompleted(net::URLRequest* request, int bytes_read) override;

private:
  void StartReading();
  void ReadMore();
  void ReadMoreAsync();
  void DidRead(int num_bytes, bool completed_synchronously,
               bool into_slop_bucket);
  void SendResponseToClient();
  void CompletePendingWrite(bool success);
  void NotifyCompleted(int error_code);

  void DeleteSelf();
  mojom::URLResponseHead* BuildResponseHead() const;

private:
  std::unique_ptr<net::URLRequest> url_request_;
  // std::unique_ptr<net::GrowableIOBuffer> pending_write_;
  std::shared_ptr<NetToMojoPendingBuffer> pending_write_;
  uint32_t pending_write_buffer_size_ = 0;
  uint32_t pending_write_buffer_offset_ = 0;

  mojom::URLLoaderClient* url_loader_client_ = nullptr;

  // Used when deferring sending the data to the client until mime sniffing is
  // finished.
  mojom::URLResponseHead* response_;
  mojo::ScopedDataPipeConsumerHandle consumer_handle_;
  mojo::ScopedDataPipeProducerHandle response_body_stream_;

  


    // State checker between URLRequest::Read() and `response_body_stream_`.
  enum class URLReadState {
    // While waiting for `response_body_stream_` to be writable.
    // TODO(yoichio): Since ReadMore() is called both when mojo pipe is ready
    // to read and we want to write to the pipe.
    // Consider spliting this into kNotReading and kWaitMojoPipeWritable.
    kWaitMojoPipeWritable,
    // While async URLRequest::ReadMore() task is posted.
    kReadMoreTaskPosted,
    // While there's a URLRequest::Read() call in progress.
    kURLReadInProgress
  };
  URLReadState url_read_state_ = URLReadState::kWaitMojoPipeWritable;

};
}

#endif