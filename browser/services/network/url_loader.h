#ifndef SERVICES_NETWORK_URL_LOADER_H_
#define SERVICES_NETWORK_URL_LOADER_H_

#include <memory>
#include "net/url_request/url_request.h"
#include "net/url_request/url_request.h"
#include "net/base/io_buffer.h"
#include "services/network/public/mojom/url_loader.mojom.h"

namespace network {
class URLLoaderContext;
class ResourceRequest;

class URLLoader : public net::URLRequest::Delegate {
public:
  URLLoader(URLLoaderContext &context, const ResourceRequest &request,
            mojom::URLLoaderClient* url_loader_client);
  void ScheduleStart();

  // net::URLRequest::Delegate
  int OnConnected(net::URLRequest *request, const net::TransportInfo &info) override;
  void OnResponseStarted(net::URLRequest* request, int net_error) override;

private:
  void StartReading();
  void ReadMore();
  void DidRead(int num_bytes, bool completed_synchronously,
               bool into_slop_bucket);
  void SendResponseToClient();

private:
   std::unique_ptr<net::URLRequest> url_request_;
   std::unique_ptr<net::GrowableIOBuffer> pending_write_;

   mojom::URLLoaderClient* url_loader_client_ = nullptr;
};

}

#endif