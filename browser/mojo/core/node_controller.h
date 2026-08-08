#ifndef MOJO_CORE_NODE_CONTROLLER_H_
#define MOJO_CORE_NODE_CONTROLLER_H_

#include "base/memory/writable_shared_memory_region.h"
#include "mojo/mojo_export.h"
#include "mojo/core/ports/node.h"
#include "mojo/core/ports/user_data.h"

namespace mojo {
namespace core {

class MOJO_EXPORT NodeController : public ports::NodeDelegate {
public:
  NodeController();

  class PortObserver : public ports::UserData {
   public:
    virtual void OnPortStatusChanged() = 0;

   protected:
    ~PortObserver() override = default;
  };

    // Creates a new shared buffer for use in the current process.
  base::WritableSharedMemoryRegion CreateSharedBuffer(size_t num_bytes);

  const ports::NodeName& name() const { return name_; }
  ports::Node* node() const { return node_.get(); }
  
  // ports::NodeDelegate:
  void ForwardEvent(const ports::NodeName& node,
                    ports::ScopedEvent event) override;
  void BroadcastEvent(ports::ScopedEvent event) override;
  void PortStatusChanged(const ports::PortRef& port) override;

    void SetPortObserver(const ports::PortRef& port,
                       std::shared_ptr<PortObserver> observer);

private:
  const ports::NodeName name_;
  const std::unique_ptr<ports::Node> node_;
};

}  // namespace core
}  // namespace mojo


#endif