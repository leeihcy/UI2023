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

  void NotifyConsumerOfCompletion(bool destroy_results) override {
    // body_reader_.reset();
    std::optional<std::string> body =
        destroy_results ? std::nullopt : std::move(body_);
    body_ = std::nullopt;

    if (body_as_string_callback_) {
      body_as_string_callback_(std::move(body));
      body_as_string_callback_ = nullptr;
    }
  }

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
void SimpleURLLoader::OnComplete(const ::network::URLLoaderCompletionStatus& status) {
  // Wait to receive any pending data on the data pipe before reporting the
  // failure.
  MaybeComplete();
}

void SimpleURLLoader::MaybeComplete() {
  FinishWithResult(/*request_state_->net_error*/0);
}

void SimpleURLLoader::FinishWithResult(int net_error) {
// If it's a partial download or an error was received, erase the body.
  // bool destroy_results =
  //     request_state_->net_error != net::OK && !allow_partial_results_;
  body_handler_->NotifyConsumerOfCompletion(/*destroy_results*/true);
}

} // namespace network