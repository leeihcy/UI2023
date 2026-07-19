#include "services/network/public/cpp/simple_url_loader.h"

#include <memory>
#include <span>
#include <assert.h>
#include "net/base/net_errors.h"

namespace network {

// static
std::unique_ptr<SimpleURLLoader>
SimpleURLLoader::Create(std::unique_ptr<ResourceRequest> resource_request) {
  return std::make_unique<SimpleURLLoader>(std::move(resource_request));
}

SimpleURLLoader::SimpleURLLoader(std::unique_ptr<ResourceRequest> resource_request) 
  : m_resource_request(std::move(resource_request))
{

}

class BodyReader {
 public:
  class Delegate {
   public:
    Delegate() {}

    Delegate(const Delegate&) = delete;
    Delegate& operator=(const Delegate&) = delete;
    virtual net::Error OnDataRead(std::span<const uint8_t> data) { return net::OK; }
    virtual void OnDone(net::Error error, int64_t total_bytes) { }
   protected:
    virtual ~Delegate() = default;
  };
};

// BodyHandler implementation for consuming the response as a string.
class SaveToStringBodyHandler : public BodyHandler,
                                public BodyReader::Delegate {
public:
  SaveToStringBodyHandler(
      SimpleURLLoader *simple_url_loader, bool want_download_progress,
      SimpleURLLoader::BodyAsStringCallback body_as_string_callback,
      int64_t max_body_size)
      : BodyHandler(simple_url_loader, want_download_progress),
        max_body_size_(max_body_size),
        body_as_string_callback_(std::move(body_as_string_callback)) {}

private:
  const int64_t max_body_size_;

  std::optional<std::string> body_;
  SimpleURLLoader::BodyAsStringCallback body_as_string_callback_;
};

void SimpleURLLoader::DownloadToString(
    mojom::URLLoaderFactory *url_loader_factory,
    BodyAsStringCallback body_as_string_callback, size_t max_body_size) {
  body_handler_ = std::make_unique<SaveToStringBodyHandler>(
      this, false, std::move(body_as_string_callback), max_body_size);
  Start(url_loader_factory);
}

void SimpleURLLoader::Start(mojom::URLLoaderFactory* url_loader_factory) {
  StartRequest(url_loader_factory);
}

void SimpleURLLoader::StartRequest(
    mojom::URLLoaderFactory *url_loader_factory) {
  url_loader_factory->CreateLoaderAndStart(
      *m_resource_request, static_cast<mojom::URLLoaderClient *>(this));

  // TODO:
  // 1. 超时回调
  // 2. 重试次数
}

void SimpleURLLoader::OnReceiveResponse(
      ::network::mojom::URLResponseHead* head/*, 
      ::mojo::ScopedDataPipeConsumerHandle body, 
      std::optional<::mojo_base::BigBuffer> cached_metadata*/) {
  assert(false);
}

} // namespace network