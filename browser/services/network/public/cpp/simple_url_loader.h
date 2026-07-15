#ifndef SERVICES_NETWORK_PUBLIC_CPP_SIMPLEURLLOADER_H_
#define SERVICES_NETWORK_PUBLIC_CPP_SIMPLEURLLOADER_H_

#include <memory>
#include <functional>
#include "services/network/public/cpp/resource_request.h"
#include "services/network/url_loader_factory.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"

namespace network {
class SimpleURLLoader;

class BodyHandler {
 public:
  // A raw pointer is safe, since |simple_url_loader| owns the BodyHandler.
  BodyHandler(SimpleURLLoader* simple_url_loader,
              bool want_download_progress)
      : simple_url_loader_(simple_url_loader),
        want_download_progress_(want_download_progress) {}

  BodyHandler(const BodyHandler&) = delete;
  BodyHandler& operator=(const BodyHandler&) = delete;

  virtual ~BodyHandler() = default;

  virtual bool RequiresBodyDataPipe() { return true; }
  virtual void OnStartLoadingResponseBody(/*mojo::ScopedDataPipeConsumerHandle body_data_pipe*/) { }
  virtual void NotifyConsumerOfCompletion(bool destroy_results) { }
  // virtual void PrepareToRetry(base::OnceClosure retry_callback) { }

 protected:
  const SimpleURLLoader* simple_url_loader() { return simple_url_loader_; }

  // void ReportProgress(int64_t total_downloaded) {
  //   if (!want_download_progress_)
  //     return;
  //   simple_url_loader_->OnBodyHandlerProgress(total_downloaded);
  // }

 private:
  const SimpleURLLoader* simple_url_loader_;
  bool const want_download_progress_;
};

//
// 对外使用接口，不要直接调用UrlLoader。
//
class SimpleURLLoader {
public:
  using BodyAsStringCallback =
      std::function<void(std::optional<std::string> response_body)>;

  static std::unique_ptr<SimpleURLLoader> Create(
    std::unique_ptr<ResourceRequest> resource_request
  );

  SimpleURLLoader(std::unique_ptr<ResourceRequest> resource_request);

  void DownloadToString(mojom::URLLoaderFactory *url_loader_factory,
                        BodyAsStringCallback body_as_string_callback,
                        size_t max_body_size);
private:
  void Start(mojom::URLLoaderFactory* url_loader_factory);
  void StartRequest(mojom::URLLoaderFactory* url_loader_factory);

private:
  std::unique_ptr<ResourceRequest> m_resource_request;
  std::unique_ptr<BodyHandler> body_handler_;
};
}

#endif