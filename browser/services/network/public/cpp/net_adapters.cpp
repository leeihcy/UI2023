#include "services/network/public/cpp/net_adapters.h"


namespace network {
namespace {
constexpr size_t kMaxBufSize = 64 * 1024;
}

NetToMojoPendingBuffer::NetToMojoPendingBuffer(
    mojo::ScopedDataPipeProducerHandle handle,
    std::span<char> buffer)
    : handle_(std::move(handle)), buffer_(buffer) {
}


MojoResult NetToMojoPendingBuffer::BeginWrite(
    mojo::ScopedDataPipeProducerHandle* handle,
    std::shared_ptr<NetToMojoPendingBuffer>* pending) {
  std::span<uint8_t> buf;
  MojoResult result =
      (*handle)->BeginWriteData(kMaxBufSize, MOJO_WRITE_DATA_FLAG_NONE, buf);
  if (result != MOJO_RESULT_OK) {
    *pending = nullptr;
    return result;
  }
  if (buf.size() > kMaxBufSize) {
    buf = buf.first(kMaxBufSize);
  }
  *pending = std::make_shared<NetToMojoPendingBuffer>(std::move(*handle),
                                        std::span<char>((char*)buf.data(), buf.size()));
  return MOJO_RESULT_OK;
}

mojo::ScopedDataPipeProducerHandle NetToMojoPendingBuffer::Complete(
    uint32_t num_bytes) {
  handle_->EndWriteData(num_bytes);
  buffer_ = std::span<char>();
  return std::move(handle_);
}


// --------------

NetToMojoIOBuffer::NetToMojoIOBuffer(
    std::shared_ptr<NetToMojoPendingBuffer> pending_buffer, size_t offset)
    : net::WrappedIOBuffer(std::span((const char *)(pending_buffer->buffer()),
                                     pending_buffer->size())
                               .subspan(offset)),
      pending_buffer_(std::move(pending_buffer)) {}

NetToMojoIOBuffer::~NetToMojoIOBuffer() {
  // Avoid dangling ptr should this destructor remove the last reference
  // to `pending_buffer_`.
  ClearSpan();
}
}