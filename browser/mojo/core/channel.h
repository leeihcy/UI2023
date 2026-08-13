
#ifndef MOJO_CORE_CHANNEL_H_
#define MOJO_CORE_CHANNEL_H_

#include <memory>

namespace mojo::core {

class Channel {
public:
  struct Message {};
  using MessagePtr = std::unique_ptr<Message>;
};

} // namespace mojo::core

#endif