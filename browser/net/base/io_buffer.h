#ifndef _NET_BASE_IO_BUFFER_H_
#define _NET_BASE_IO_BUFFER_H_

#include <cstddef>
#include <span>
#include <memory>
#include <string>
#include <vector>

#include "base/containers/heap_array.h"
#include "base/memory/free_deleter.h"

namespace net {

class IOBuffer {
public:
  int size() const {
    return static_cast<int>(span_.size());
  }

  char* data() { return reinterpret_cast<char*>(bytes()); }
  const char* data() const { return reinterpret_cast<const char*>(bytes()); }

  uint8_t* bytes() { return span_.data(); }
  const uint8_t* bytes() const { return span_.data(); }

  std::span<uint8_t> span() { return span_; }
  std::span<const uint8_t> span() const {
    // Converts a const std::span<uint8_t> to a std::span<const uint8_t>.
    return std::span((const uint8_t*)span_.data(), span_.size());
  }

  // Convenience methods for accessing the buffer as a span.
  std::span<uint8_t> first(size_t count) { return span().first(count); }
  std::span<const uint8_t> first(size_t count) const {
    return span().first(count);
  }

 protected:
  IOBuffer();
  explicit IOBuffer(std::span<char> span);
  explicit IOBuffer(std::span<uint8_t> span);

  virtual ~IOBuffer();

  // Sets `span_` to `span`. CHECKs if its size is too big to fit in an int.
  void SetSpan(std::span<uint8_t> span);

  // Like SetSpan(std::span<uint8_t>()), but without a size check. Particularly
  // useful to call in the destructor of subclasses, to avoid failing raw
  // reference checks.
  void ClearSpan();

 private:
  std::span<uint8_t> span_;
};


// Class which owns its buffer and manages its destruction.
class IOBufferWithSize : public IOBuffer {
 public:
  IOBufferWithSize();
  explicit IOBufferWithSize(size_t size);

 protected:
  ~IOBufferWithSize() override;

 private:
  base::HeapArray<uint8_t> storage_;
};


// This is like IOBufferWithSize, except its constructor takes a vector.
// IOBufferWithSize uses a HeapArray instead of a vector so that it can avoid
// initializing its data. VectorIOBuffer is primarily useful useful for writing
// data, while IOBufferWithSize is primarily useful for reading data.
class VectorIOBuffer : public IOBuffer {
 public:
  explicit VectorIOBuffer(std::vector<uint8_t> vector);
  explicit VectorIOBuffer(std::span<const uint8_t> span);

 private:
  ~VectorIOBuffer() override;

  std::vector<uint8_t> vector_;
};

// This is a read only IOBuffer.  The data is stored in a string and
// the IOBuffer interface does not provide a proper way to modify it.
class StringIOBuffer : public IOBuffer {
 public:
  explicit StringIOBuffer(std::string s);

 private:
  ~StringIOBuffer() override;

  std::string string_data_;
};


// This version wraps an existing IOBuffer and provides convenient functions
// to progressively read all the data. The values returned by size() and bytes()
// are updated as bytes are consumed from the buffer.
//
// DrainableIOBuffer is useful when you have an IOBuffer that contains data
// to be written progressively, and Write() function takes an IOBuffer rather
// than char*. DrainableIOBuffer can be used as follows:
//
// // payload is the IOBuffer containing the data to be written.
// buf = base::MakeRefCounted<DrainableIOBuffer>(payload, payload_size);
//
// while (buf->BytesRemaining() > 0) {
//   // Write() takes an IOBuffer. If it takes char*, we could
//   // simply use the regular IOBuffer like payload->data() + offset.
//   int bytes_written = Write(buf, buf->BytesRemaining());
//   buf->DidConsume(bytes_written);
// }
//
class DrainableIOBuffer : public IOBuffer {
 public:
  // `base` should be treated as exclusively owned by the DrainableIOBuffer as
  // long as the latter exists. Specifically, the span pointed to by `base`,
  // including its size, must not change, as the `DrainableIOBuffer` maintains a
  // copy of them internally.
  DrainableIOBuffer(std::shared_ptr<IOBuffer> base, size_t size);

  // DidConsume() changes the |data_| pointer so that |data_| always points
  // to the first unconsumed byte.
  void DidConsume(int bytes);

  // Returns the number of unconsumed bytes.
  int BytesRemaining() const;

  // Returns the number of consumed bytes.
  int BytesConsumed() const;

  // Seeks to an arbitrary point in the buffer. The notion of bytes consumed
  // and remaining are updated appropriately.
  void SetOffset(int bytes);

 private:
  ~DrainableIOBuffer() override;

  std::shared_ptr<IOBuffer> base_;
  int used_ = 0;
};



// This version provides a resizable buffer and a changeable offset. The values
// returned by size() and bytes() are updated whenever the offset of the buffer
// is set, or the buffer's capacity is changed.
//
// GrowableIOBuffer is useful when you read data progressively without
// knowing the total size in advance. GrowableIOBuffer can be used as
// follows:
//
// buf = base::MakeRefCounted<GrowableIOBuffer>();
// buf->SetCapacity(1024);  // Initial capacity.
//
// while (!some_stream->IsEOF()) {
//   // Double the capacity if the remaining capacity is empty.
//   if (buf->RemainingCapacity() == 0)
//     buf->SetCapacity(buf->capacity() * 2);
//   int bytes_read = some_stream->Read(buf, buf->RemainingCapacity());
//   buf->set_offset(buf->offset() + bytes_read);
// }
//
class GrowableIOBuffer : public IOBuffer {
 public:
  GrowableIOBuffer();

  // realloc memory to the specified capacity.
  void SetCapacity(int capacity);
  int capacity() { return capacity_; }

  // `offset` moves the `data_` pointer, allowing "seeking" in the data.
  void set_offset(int offset);
  int offset() { return offset_; }

  // Advances the offset by `bytes`. It's equivalent to `set_offset(offset() +
  // bytes)`, though does not accept negative values, as they likely indicate a
  // bug.
  void DidConsume(int bytes);

  int RemainingCapacity();

  // Returns the entire buffer, including the bytes before the `offset()`.
  //
  // The `span()` method in the base class only gives the part of the buffer
  // after `offset()`.
  std::span<uint8_t> everything();
  std::span<const uint8_t> everything() const;

  // Return a span before the `offset()`.
  std::span<uint8_t> span_before_offset();
  std::span<const uint8_t> span_before_offset() const;

 private:
  ~GrowableIOBuffer() override;

  // TODO(329476354): Convert to std::vector, use reserve()+resize() to make
  // exact reallocs, and remove `capacity_`. Possibly with an allocator the
  // default-initializes, if it's important to not initialize the new memory?
  std::unique_ptr<uint8_t, base::FreeDeleter> real_data_;
  int capacity_ = 0;
  int offset_ = 0;
};

}

#endif