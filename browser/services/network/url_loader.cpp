#include "services/network/url_loader.h"
#include "services/network/url_loader_factory.h"
#include "net/url_request/url_request_context.h"
#include "services/network/public/cpp/resource_request.h"

namespace network {

URLLoader::URLLoader(URLLoaderContext &context, const ResourceRequest &request,
                     mojom::URLLoaderClient *url_loader_client)
    : url_loader_client_(url_loader_client) {
  url_request_ =
      context.GetUrlRequestContext()->CreateRequest(request.url, this);
  ScheduleStart();
}

void URLLoader::ScheduleStart() {
  url_request_->Start();
}

int URLLoader::OnConnected(net::URLRequest *request, const net::TransportInfo &info) {

  // local network access (LNA)处理逻辑。
  
  return 0;
}

void URLLoader::OnResponseStarted(net::URLRequest* request, int net_error) {
  StartReading();
}

void URLLoader::StartReading() {
  // Start reading...
  ReadMore();
}

void URLLoader::ReadMore() {
  const int size = 64 * 1024;
  if (!pending_write_) {
    pending_write_ = std::make_unique<net::GrowableIOBuffer>();
    pending_write_->SetCapacity(size);
  }

  // URLRequest::Read
  // url_read_state_ = URLReadState::kURLReadInProgress;
  int bytes_read = url_request_->Read(pending_write_.get(), size);
  if (bytes_read != net::ERR_IO_PENDING) {
    DidRead(bytes_read, /*completed_synchronously=*/true,
            /*into_slop_bucket=*/false);
    // |this| may have been deleted.
  }
}

void URLLoader::DidRead(int num_bytes,
                        bool completed_synchronously,
                        bool into_slop_bucket) {
  SendResponseToClient();
}

void URLLoader::SendResponseToClient() {
  // response_->emitted_extra_info = emitted_devtools_raw_request_;

  url_loader_client_->OnReceiveResponse(nullptr/*
      response_->Clone(), std::move(consumer_handle_), std::nullopt*/);
}

}