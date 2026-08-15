#ifndef MOJO_CORE_PORTS_NODE_H_
#define MOJO_CORE_PORTS_NODE_H_

#include <map>

#include "mojo/mojo_export.h"
#include "mojo/core/ports/port.h"
#include "mojo/core/ports/event.h"
#include "mojo/core/ports/user_data.h"

namespace mojo {
namespace core {
namespace ports {

enum : int {
  OK = 0,
  ERROR_PORT_UNKNOWN = -10,
  ERROR_PORT_EXISTS = -11,
  ERROR_PORT_STATE_UNEXPECTED = -12,
  ERROR_PORT_CANNOT_SEND_SELF = -13,
  ERROR_PORT_PEER_CLOSED = -14,
  ERROR_PORT_CANNOT_SEND_PEER = -15,
  ERROR_NOT_IMPLEMENTED = -100,
};

struct PortStatus {
  bool has_messages;
  bool receiving_messages;
  bool peer_closed;
  bool peer_remote;
  size_t queued_message_count;
  size_t queued_num_bytes;
  size_t unacknowledged_message_count;
};

struct PendingUpdatePreviousPeer {
  NodeName receiver;
  PortName port;
  PortName from_port;
  uint64_t sequence_num;
  NodeName new_prev_node;
  PortName new_prev_port;
};

class MOJO_EXPORT NodeDelegate {
 public:
  virtual ~NodeDelegate() = default;

  virtual void ForwardEvent(const NodeName& node, ScopedEvent event) = 0;
  virtual void BroadcastEvent(ScopedEvent event) = 0;
  virtual void PortStatusChanged(const PortRef& port_ref) = 0;
};


class MOJO_EXPORT Node {
public:
  Node(const NodeName& name, NodeDelegate* delegate)
    : name_(name), delegate_(delegate) {}

  // Creates a port on this node. Before the port can be used, it must be
  // initialized using InitializePort. This method is useful for bootstrapping
  // a connection between two nodes. Generally, ports are created using
  // CreatePortPair instead.
  int CreateUninitializedPort(PortRef* port_ref);

  // Initializes a newly created port.
  int InitializePort(const PortRef& port_ref,
                     const NodeName& peer_node_name,
                     const PortName& peer_port_name,
                     const NodeName& prev_node_name,
                     const PortName& prev_port_name);

  // Generates a new connected pair of ports bound to this node. These ports
  // are initialized and ready to go.
  int CreatePortPair(PortRef* port0_ref, PortRef* port1_ref);

  int AddPortWithName(const PortName& port_name, std::shared_ptr<Port> port);

  void UpdatePortPeerAddress(const PortName &local_port_name, std::shared_ptr<Port> local_port,
                             const NodeName &new_peer_node,
                             const PortName &new_peer_port);
  // Removes an entry from |peer_port_map_| corresponding to |local_port|'s peer
  // address, if valid.
  void RemoveFromPeerPortMap(const PortName& local_port_name, Port* local_port);

    // User data associated with the port.
  int SetUserData(const PortRef& port_ref, std::shared_ptr<UserData> user_data);
  int GetUserData(const PortRef& port_ref, std::shared_ptr<UserData>* user_data);

  int GetStatus(const PortRef& port_ref, PortStatus* port_status);

  int SendUserMessage(const PortRef &port_ref,
                      std::unique_ptr<UserMessageEvent> message);

  int GetPort(const PortName& port_name, PortRef* port_ref);
  int ClosePort(const PortRef& port_ref);
  void ErasePort(const PortName& port_name);

  int AcceptEvent(const NodeName& from_node, ScopedEvent event);
  int PrepareToForwardUserMessage(const PortRef& forwarding_port_ref,
                                  Port::State expected_port_state,
                                  bool ignore_closed_peer,
                                  UserMessageEvent* message,
                                  NodeName* forward_to_node);
  int GetMessage1(const PortRef& port_ref,
                 std::unique_ptr<UserMessageEvent>* message,
                 MessageFilter* filter);
private:
  int AcceptEventInternal(const PortRef& port_ref,
                              const NodeName& from_node,
                              ScopedEvent event);
  int SendUserMessageInternal(const PortRef& port_ref,
                              std::unique_ptr<UserMessageEvent>* message);
  int OnUserMessage(const PortRef& port_ref,
                    const NodeName& from_node,
                    std::unique_ptr<UserMessageEvent> message);
  int AcceptPort(const PortName& port_name,
                     const Event::PortDescriptor& port_descriptor);
  void ConvertToProxy(std::shared_ptr<Port> port,
                      const NodeName& to_node_name,
                      PortName* port_name,
                      Event::PortDescriptor* port_descriptor,
                      PendingUpdatePreviousPeer* pending_update);
private:
  const NodeName name_;
  NodeDelegate* delegate_;

  // Just to clarify readability of the types below.
  using LocalPortName = PortName;
  using PeerPortName = PortName;

  std::map<LocalPortName, std::shared_ptr<Port>> ports_;

  using PeerPortMap = std::map<PeerPortName, std::map<LocalPortName, PortRef>>;
  std::map<NodeName, PeerPortMap> peer_port_maps_;
};

}  // namespace ports
}  // namespace core
}  // namespace mojo

#endif  // MOJO_CORE_PORTS_NODE_H_