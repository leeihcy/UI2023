#include "net/base/io_buffer.h"

namespace net {

IOBuffer::IOBuffer() = default;

IOBuffer::IOBuffer(std::span<char> span)
    : IOBuffer(std::span((uint8_t*)span.data(), span.size())) {}

IOBuffer::IOBuffer(std::span<uint8_t> span) : span_(span) {
}

IOBuffer::~IOBuffer() = default;

void IOBuffer::SetSpan(std::span<uint8_t> span) {
  span_ = span;
}

void IOBuffer::ClearSpan() {
  span_ = std::span<uint8_t>();
}

// ---------------------------------------------------------

IOBufferWithSize::IOBufferWithSize() = default;

IOBufferWithSize::IOBufferWithSize(size_t buffer_size)
    : storage_(base::HeapArray<uint8_t>::Uninit(buffer_size)) {
  SetSpan(storage_);
}

IOBufferWithSize::~IOBufferWithSize() {
  // Clear pointer before this destructor makes it dangle.
  ClearSpan();
}

// ---------------------------------------------------------


VectorIOBuffer::VectorIOBuffer(std::vector<uint8_t> vector)
    : vector_(std::move(vector)) {
  SetSpan(vector_);
}

VectorIOBuffer::VectorIOBuffer(std::span<const uint8_t> span)
    : VectorIOBuffer(std::vector(span.begin(), span.end())) {}

VectorIOBuffer::~VectorIOBuffer() {
  // Clear pointer before this destructor makes it dangle.
  ClearSpan();
}

// ---------------------------------------------------------


StringIOBuffer::StringIOBuffer(std::string s) : string_data_(std::move(s)) {
  // Can't pass `s.data()` directly to IOBuffer constructor since moving
  // from `s` may invalidate it. This is especially true for libc++ short
  // string optimization where the data may be held in the string variable
  // itself, instead of in a movable backing store.
  SetSpan(std::span((uint8_t*)string_data_.c_str(), string_data_.length()));
}

StringIOBuffer::~StringIOBuffer() {
  // Clear pointer before this destructor makes it dangle.
  ClearSpan();
}

// ---------------------------------------------------------


DrainableIOBuffer::DrainableIOBuffer(std::shared_ptr<IOBuffer> base, size_t size)
    : IOBuffer(base->first(size)), base_(std::move(base)) {}

void DrainableIOBuffer::DidConsume(int bytes) {
  SetOffset(used_ + bytes);
}

int DrainableIOBuffer::BytesRemaining() const {
  return size();
}

// Returns the number of consumed bytes.
int DrainableIOBuffer::BytesConsumed() const {
  return used_;
}

void DrainableIOBuffer::SetOffset(int bytes) {
  // CHECK_GE(bytes, 0);
  // Length from the start of `base_` to the end of the buffer passed in to the
  // constructor isn't stored anywhere, so need to calculate it.
  int length = size() + used_;
  // CHECK_LE(bytes, length);
  used_ = bytes;
  SetSpan(base_->span().subspan(size_t(used_),
                                size_t(length - bytes)));
}

DrainableIOBuffer::~DrainableIOBuffer() {
  // Clear ptr before this destructor destroys the |base_| instance,
  // making it dangle.
  ClearSpan();
}

// ---------------------------------------------------------

GrowableIOBuffer::GrowableIOBuffer() = default;

void GrowableIOBuffer::SetCapacity(int capacity) {
  // CHECK_GE(capacity, 0);

  // The span will be set again in `set_offset()`. Need to clear raw pointers to
  // the data before reallocating the buffer.
  ClearSpan();

  // realloc will crash if it fails.
  real_data_.reset(
      static_cast<uint8_t*>(realloc(real_data_.release(), capacity)));

  capacity_ = capacity;
  if (offset_ > capacity)
    set_offset(capacity);
  else
    set_offset(offset_);  // The pointer may have changed.
}

void GrowableIOBuffer::set_offset(int offset) {
  // CHECK_GE(offset, 0);
  // CHECK_LE(offset, capacity_);
  offset_ = offset;

  SetSpan(std::span<uint8_t>(
      real_data_.get() + offset, static_cast<size_t>(capacity_ - offset)));
}

void GrowableIOBuffer::DidConsume(int bytes) {
  // CHECK_LE(0, bytes);
  // CHECK_LE(bytes, size());
  set_offset(offset_ + bytes);
}

int GrowableIOBuffer::RemainingCapacity() {
  return size();
}

std::span<uint8_t> GrowableIOBuffer::everything() {
  return std::span((uint8_t*)real_data_.get(), size_t(capacity_));
}

std::span<const uint8_t> GrowableIOBuffer::everything() const {
  return std::span((const uint8_t*)real_data_.get(), size_t(capacity_));
}

std::span<uint8_t> GrowableIOBuffer::span_before_offset() {
  return everything().first(size_t(offset_));
}

std::span<const uint8_t> GrowableIOBuffer::span_before_offset() const {
  return everything().first(size_t(offset_));
}

GrowableIOBuffer::~GrowableIOBuffer() {
  ClearSpan();
}

// ---------------------------------------------------------


}