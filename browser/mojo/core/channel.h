
#ifndef MOJO_CORE_CHANNEL_H_
#define MOJO_CORE_CHANNEL_H_

#include <memory>
#include <span>
#include <vector>
#include "mojo/core/platform_handle_in_transit.h"

namespace mojo::core {

const size_t kChannelMessageAlignment = 8;

constexpr bool IsAlignedForChannelMessage(size_t n) {
  return n % kChannelMessageAlignment == 0;
}

class Channel {
public:
  
  struct Message;
  using MessagePtr = std::unique_ptr<Message>;
  using AlignedBuffer = std::vector<char>;

  struct Message {
    virtual ~Message() {}

    enum class MessageType : uint16_t {
      NORMAL_LEGACY = 0,
// #if BUILDFLAG(IS_IOS)
//       HANDLES_SENT,
//       HANDLES_SENT_ACK,
// #endif
      NORMAL,
      UPGRADE_OFFER,
      UPGRADE_ACCEPT,
      UPGRADE_REJECT,
    };

#pragma pack(push, 1)
    // Old message wire format for ChromeOS and Android, used by NORMAL_LEGACY
    // messages.
    struct LegacyHeader {
      // Message size in bytes, including the header.
      uint32_t num_bytes;

      // Number of attached handles.
      uint16_t num_handles;

      MessageType message_type;
    };
     struct Header {
      // Message size in bytes, including the header.
      uint32_t num_bytes;

      // Total size of header, including extra header data (i.e. HANDLEs on
      // windows).
      uint16_t num_header_bytes;

      MessageType message_type;

      // Number of attached handles. May be less than the reserved handle
      // storage size in this message on platforms that serialise handles as
      // data (i.e. HANDLEs on Windows, Mach ports on OSX).
      uint16_t num_handles;

      char padding[6];
    };
     struct HandleEntry {
      uint32_t handle;
    };
#pragma pack(pop)

    size_t num_handles() const;

    // Returns true iff the LegacyHeader is in use for this message.
    virtual bool is_legacy_message() const;
    LegacyHeader* legacy_header();
    const LegacyHeader* legacy_header() const;

    virtual Header* header();
    virtual const Header* header() const;

    const void* data() const { return data_span().data(); }
    void* mutable_data() { return mutable_data_span().data(); }
    virtual std::span<const char> data_span() const = 0;
    virtual std::span<char> mutable_data_span() = 0;

    size_t data_num_bytes() const { return size_; }

    void* mutable_payload();
    std::span<char> mutable_payload_span();
    const void* payload() const;
    std::span<const char> payload_span() const;
    size_t payload_size() const;

    // Note: SetHandles() and TakeHandles() invalidate any previous value of
    // handles().
    virtual void SetHandles(std::vector<PlatformHandle> new_handles) = 0;
    virtual void SetHandles(
        std::vector<PlatformHandleInTransit> new_handles) = 0;
    virtual std::vector<PlatformHandleInTransit> TakeHandles() = 0;

    virtual size_t capacity() const = 0;

    const void* extra_header() const;
    void* mutable_extra_header();
    std::span<char> mutable_extra_header_span();
    size_t extra_header_size() const;
    
    // Allocates and owns a buffer for message data with enough capacity for
    // |payload_size| bytes plus a header, plus |max_handles| platform handles.
    static MessagePtr CreateMessage(size_t payload_size, size_t max_handles);
    static MessagePtr CreateMessage(size_t payload_size,
                                    size_t max_handles,
                                    MessageType message_type);
    static MessagePtr CreateMessage(size_t capacity,
                                    size_t payload_size,
                                    size_t max_handles);
    static MessagePtr CreateMessage(size_t capacity,
                                    size_t max_handles,
                                    size_t payload_size,
                                    MessageType message_type);

    size_t size_ = 0;
  };

  class Delegate {
   public:
    virtual ~Delegate() = default;
#if 0
    // Indicates whether the listener on this Channel is an ipcz transport.
    virtual bool IsIpczTransport() const;

    // Notify of a received message. |payload| is not owned and must not be
    // retained; it will be null if |payload_size| is 0. |handles| are
    // transferred to the callee.
    virtual void OnChannelMessage(
        const void* payload,
        size_t payload_size,
        std::vector<PlatformHandle> handles,
        scoped_refptr<ipcz_driver::Envelope> envelope) = 0;

    // Notify that an error has occured and the Channel will cease operation.
    virtual void OnChannelError(Error error) = 0;

    // Notify that the Channel is about to be destroyed and will definitely not
    // call into the Delegate again. Only called for Channels that back an ipcz
    // transport.
    virtual void OnChannelDestroyed();
#endif
  };
};

} // namespace mojo::core

#endif