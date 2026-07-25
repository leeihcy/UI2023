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

/*
>	net.dll!net::HttpCache::Transaction::Read	C++
 	net.dll!net::URLRequestHttpJob::ReadRawData	C++
 	net.dll!net::URLRequestJob::ReadRawDataHelper	C++
 	net.dll!net::URLRequestJob::URLRequestJobSourceStream::Read	C++
 	net.dll!net::URLRequestJob::Read	C++
 	net.dll!net::URLRequest::Read	C++
 	services_network_network_service.dll!network::URLLoader::ReadMore	C++
 	services_network_network_service.dll!network::URLLoader::StartReading	C++
 	services_network_network_service.dll!network::URLLoader::ContinueOnResponseStarted	C++
 	services_network_network_service.dll!network::URLLoader::ProcessInboundSharedStorageInterceptorOnResponseStarted	C++
 	services_network_network_service.dll!network::URLLoader::OnResponseStarted	C++
 	net.dll!net::URLRequest::NotifyResponseStarted	C++
 	net.dll!net::URLRequestJob::NotifyFinalHeadersReceived	C++
 	net.dll!net::URLRequestJob::NotifyHeadersComplete	C++
 	net.dll!net::URLRequestHttpJob::NotifyHeadersComplete	C++
 	net.dll!net::URLRequestHttpJob::SaveCookiesAndNotifyHeadersComplete	C++
 	net.dll!net::URLRequestHttpJob::OnStartCompleted	C++
*/
void URLLoader::OnResponseStarted(net::URLRequest* request, int net_error) {
  response_ = BuildResponseHead();

  if (!response_body_stream_.is_valid()) {
    MojoCreateDataPipeOptions options;
    options.struct_size = sizeof(MojoCreateDataPipeOptions);
    options.flags = MOJO_CREATE_DATA_PIPE_FLAG_NONE;
    options.element_num_bytes = 1;

    constexpr uint32_t kLargerDataPipeAllocationSize = 2 * 1024 * 1024;
    options.capacity_num_bytes = kLargerDataPipeAllocationSize;

    MojoResult result =
        mojo::CreateDataPipe(&options, response_body_stream_, consumer_handle_);
  }
  
  StartReading();
}

mojom::URLResponseHead* URLLoader::BuildResponseHead() const {
  // url_loader_util::BuildResponseHead
  auto response = new mojom::URLResponseHead();
  // response->headers = url_request_.response_headers();

  return response;
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
  // SendResponseToClient();
  if (num_bytes <= 0) {
    if (!into_slop_bucket) {
      NotifyCompleted(num_bytes);
      // |this| will have been deleted.
    }
    return;
  }

  if (completed_synchronously) {
    // ReadMoreAsync();
    assert(false);
  } else {
    ReadMore();
  }
}

/*
>	services_network_network_service.dll!network::URLLoader::NotifyCompleted	C++
 	services_network_network_service.dll!network::URLLoader::DidRead	C++
 	services_network_network_service.dll!network::URLLoader::ReadMore	C++
 	services_network_network_service.dll!network::URLLoader::DidRead	C++
 	services_network_network_service.dll!network::URLLoader::OnReadCompleted	C++
 	net.dll!net::URLRequest::NotifyReadCompleted	C++
 	net.dll!net::URLRequestJob::SourceStreamReadComplete	C++
*/
void URLLoader::OnReadCompleted(net::URLRequest* request, int bytes_read) {
  bool into_slop_bucket = false;
  DidRead(bytes_read, /*completed_synchronously=*/false, into_slop_bucket);
}

void URLLoader::SendResponseToClient() {
  // response_->emitted_extra_info = emitted_devtools_raw_request_;

  url_loader_client_->OnReceiveResponse(
      response_, // response_->Clone(),  TODO:
      std::move(consumer_handle_) /*, std::nullopt*/);
}

void URLLoader::NotifyCompleted(int error_code) {
  URLLoaderCompletionStatus status;
  status.error_code = error_code;
  url_loader_client_->OnComplete(status);

  DeleteSelf();
}

void URLLoader::DeleteSelf() {
  // void CorsURLLoaderFactory::DestroyURLLoader(URLLoader* loader) {
  // std::move(delete_callback_).Run(this);
}

}