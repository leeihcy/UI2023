#include "services/network/public/cpp/simple_url_loader.h"

#include <memory>
#include <span>
#include <assert.h>
#include "net/base/net_errors.h"
#include "mojo/public/cpp/system/handle_signals_state.h"
#include "mojo/public/cpp/system/simple_watcher.h"

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

  BodyReader(Delegate *delegate, int64_t max_body_size)
      : delegate_(delegate), max_body_size_(max_body_size) {}

  void Start(mojo::ScopedDataPipeConsumerHandle body_data_pipe) {
    body_data_pipe_ = std::move(body_data_pipe);
    handle_watcher_ = std::make_unique<mojo::SimpleWatcher>(
        mojo::SimpleWatcher::ArmingPolicy::MANUAL);
    handle_watcher_->Watch(
        body_data_pipe_.get(),
        MOJO_HANDLE_SIGNAL_READABLE | MOJO_HANDLE_SIGNAL_PEER_CLOSED,
        MOJO_WATCH_CONDITION_SATISFIED,
        std::bind(&BodyReader::MojoReadyCallback,
                            this, std::placeholders::_1, std::placeholders::_2));

    ReadData();
  }
  void MojoReadyCallback(MojoResult result,
                         const mojo::HandleSignalsState& state) {
    ReadData();
  }

  void ReadData() {
    while (true) {
      if (pending_error_) {
        ClosePipe();
        // This call may delete the BodyReader.
        delegate_->OnDone(pending_error_, total_bytes_read_);
        return;
      }

      std::span<const uint8_t> body;
      MojoResult result =
          body_data_pipe_->BeginReadData(MOJO_READ_DATA_FLAG_NONE, body);
      if (result == MOJO_RESULT_SHOULD_WAIT) {
        // handle_watcher_->ArmOrNotify();
        return;
      }

      // If the pipe was closed, unclear if it was an error or success. Notify
      // the consumer of how much data was received.
      if (result != MOJO_RESULT_OK) {
        ClosePipe();
        // This call may delete the BodyReader.
        // delegate_->OnDone(net::OK, total_bytes_read_);
        return;
      }

       // Check size against the limit.
      size_t copy_size = body.size();
      size_t limit = (size_t)(max_body_size_ - total_bytes_read_);
      if (copy_size > limit) {
        copy_size = limit;
      }

      total_bytes_read_ += copy_size;

      if (copy_size < body.size()) {
        pending_error_ = net::ERR_INSUFFICIENT_RESOURCES;
      }

      mojo::ScopedDataPipeConsumerHandle body_data_pipe =
          std::move(body_data_pipe_);

      // This call may delete the BodyReader.
      net::Error error = delegate_->OnDataRead(body.first(copy_size));
      body_data_pipe->EndReadData(body.size());
      // if (!weak_this) {
      //   // This object was deleted, so nothing else to do.
      //   return;
      // }

      body_data_pipe_ = std::move(body_data_pipe);

      // Wait for Resume() on net::ERR_IO_PENDING.
      if (error == net::ERR_IO_PENDING)
        return;

      if (error != net::OK)
        pending_error_ = error;
    }
  }
  void ClosePipe() {
    // handle_watcher_.reset();
    body_data_pipe_.reset();
  }

private:
  mojo::ScopedDataPipeConsumerHandle body_data_pipe_;
  std::unique_ptr<mojo::SimpleWatcher> handle_watcher_;

  Delegate* delegate_ = nullptr;

  net::Error pending_error_ = net::OK;

  const int64_t max_body_size_;
  int64_t total_bytes_read_ = 0;
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

  // BodyHandler implementation:
  void OnStartLoadingResponseBody(mojo::ScopedDataPipeConsumerHandle body_data_pipe) override {
    body_ = std::string();
    body_reader_ = std::make_unique<BodyReader>(this, max_body_size_/*,
                                                url_loader_created_from_*/);
    body_reader_->Start(std::move(body_data_pipe));
  }
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

  // BodyReader::Delegate implementation.
  net::Error OnDataRead(std::span<const uint8_t> data) override {
    body_->append((const char*)data.data());
    // ReportProgress(body_reader_->total_bytes_read());
    return net::OK;
  }
  void OnDone(net::Error error, int64_t total_bytes) override {
    // DCHECK_EQ(body_->size(), static_cast<size_t>(total_bytes));
    // simple_url_loader()->OnBodyHandlerDone(error, total_bytes);
  }

private:
  const int64_t max_body_size_;

  std::optional<std::string> body_;
  SimpleURLLoader::BodyAsStringCallback body_as_string_callback_;

  std::unique_ptr<BodyReader> body_reader_;
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
      ::network::mojom::URLResponseHead* head, 
      ::mojo::ScopedDataPipeConsumerHandle body/*, 
      std::optional<::mojo_base::BigBuffer> cached_metadata*/) {
  body_handler_->OnStartLoadingResponseBody(std::move(body));
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