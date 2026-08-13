#include "mojo/core/ports/node.h"

#include <random>

namespace mojo {
namespace core {
namespace ports {

  namespace {
  
bool CanAcceptMoreMessages(const Port* port) {
  // Have we already doled out the last message (i.e., do we expect to NOT
  // receive further messages)?
#if 0
  uint64_t next_sequence_num = port->message_queue.next_sequence_num();
  if (port->state == Port::kClosed) {
    return false;
  }
  if (port->peer_closed || port->remove_proxy_on_last_message) {
    if (port->peer_lost_unexpectedly) {
      return port->message_queue.HasNextMessage();
    }
    if (port->last_sequence_num_to_receive == next_sequence_num - 1) {
      return false;
    }
  }
#endif
  return true;
}
 
void GenerateRandomPortName(PortName* name) {
  // 玩具版：直接用随机数填满两个64位字段
  // 在真实代码中绝对不要这样用！！
  
  // 用标准库的随机数生成器（Mersenne Twister）
  static std::random_device rd;           // 获取随机种子
  static std::mt19937_64 gen(rd());       // 64位随机数生成器
  static std::uniform_int_distribution<uint64_t> dist;  // 均匀分布
  
  name->v1 = dist(gen);
  name->v2 = dist(gen);
}
 }

int Node::CreateUninitializedPort(PortRef* port_ref) {
  PortName port_name;
  GenerateRandomPortName(&port_name);

  std::shared_ptr<Port> port(new Port(kInitialSequenceNum, kInitialSequenceNum));
  int rv = AddPortWithName(port_name, port);
  if (rv != OK) {
    return rv;
  }

  *port_ref = PortRef(port_name, std::move(port));
  return OK;
}

int Node::InitializePort(const PortRef& port_ref,
                         const NodeName& peer_node_name,
                         const PortName& peer_port_name,
                         const NodeName& prev_node_name,
                         const PortName& prev_port_name) {
  {
    auto port = port_ref.port_ptr();
    if (port->state != Port::kUninitialized) {
      return ERROR_PORT_STATE_UNEXPECTED;
    }

    port->state = Port::kReceiving;
    UpdatePortPeerAddress(port_ref.name(), port, peer_node_name,
                          peer_port_name);

    port->prev_node_name = prev_node_name;
    port->prev_port_name = prev_port_name;
  }

  delegate_->PortStatusChanged(port_ref);

  return OK;
}

int Node::CreatePortPair(PortRef* port0_ref, PortRef* port1_ref) {
  int rv;

  rv = CreateUninitializedPort(port0_ref);
  if (rv != OK) {
    return rv;
  }

  rv = CreateUninitializedPort(port1_ref);
  if (rv != OK) {
    return rv;
  }

  rv = InitializePort(*port0_ref, name_, port1_ref->name(), name_,
                      port1_ref->name());
  if (rv != OK) {
    return rv;
  }

  rv = InitializePort(*port1_ref, name_, port0_ref->name(), name_,
                      port0_ref->name());
  if (rv != OK) {
    return rv;
  }

  return OK;
}

int Node::AddPortWithName(const PortName& port_name, std::shared_ptr<Port> port) {
  if (port->peer_port_name != kInvalidPortName) {
    peer_port_maps_[port->peer_node_name][port->peer_port_name].emplace(
        port_name, PortRef(port_name, port));
  }
  ports_.emplace(port_name, std::move(port));
  return OK;
}



void Node::UpdatePortPeerAddress(const PortName& local_port_name,
                                 std::shared_ptr<Port> local_port,
                                 const NodeName& new_peer_node,
                                 const PortName& new_peer_port) {
  RemoveFromPeerPortMap(local_port_name, local_port.get());
  local_port->peer_node_name = new_peer_node;
  local_port->peer_port_name = new_peer_port;
  local_port->next_control_sequence_num_to_send = kInitialSequenceNum;

  if (new_peer_port != kInvalidPortName) {
    peer_port_maps_[new_peer_node][new_peer_port].emplace(
        local_port_name,
        PortRef(local_port_name, local_port));
  }
}


void Node::RemoveFromPeerPortMap(const PortName& local_port_name,
                                 Port* local_port) {
  if (local_port->peer_port_name == kInvalidPortName) {
    return;
  }

  auto node_iter = peer_port_maps_.find(local_port->peer_node_name);
  if (node_iter == peer_port_maps_.end()) {
    return;
  }

  auto& node_peer_port_map = node_iter->second;
  auto ports_iter = node_peer_port_map.find(local_port->peer_port_name);
  if (ports_iter == node_peer_port_map.end()) {
    return;
  }

  auto& local_ports_with_this_peer = ports_iter->second;
  local_ports_with_this_peer.erase(local_port_name);
  if (local_ports_with_this_peer.empty()) {
    node_peer_port_map.erase(ports_iter);
  }
  if (node_peer_port_map.empty()) {
    peer_port_maps_.erase(node_iter);
  }
}


int Node::SetUserData(const PortRef& port_ref,
                      std::shared_ptr<UserData> user_data) {
  // SinglePortLocker locker(&port_ref);
  // auto* port = locker.port();
  auto* port = port_ref.port();
  if (port->state == Port::kClosed) {
    return ERROR_PORT_STATE_UNEXPECTED;
  }

  // port->user_data = std::move(user_data);

  return OK;
}

int Node::GetUserData(const PortRef& port_ref,
                      std::shared_ptr<UserData>* user_data) {
  // SinglePortLocker locker(&port_ref);
  // auto* port = locker.port();
  auto* port = port_ref.port();
  if (port->state == Port::kClosed) {
    return ERROR_PORT_STATE_UNEXPECTED;
  }

  // *user_data = port->user_data;

  return OK;
}

int Node::GetStatus(const PortRef& port_ref, PortStatus* port_status) {
  // SinglePortLocker locker(&port_ref);
  // auto* port = locker.port();
  auto* port = port_ref.port();
  if (port->state != Port::kReceiving) {
    return ERROR_PORT_STATE_UNEXPECTED;
  }

  // port_status->has_messages = port->message_queue.HasNextMessage();
  // port_status->receiving_messages = CanAcceptMoreMessages(port);
  // port_status->peer_closed = port->peer_closed;
  // port_status->peer_remote = port->peer_node_name != name_;
  // port_status->queued_message_count =
  //     port->message_queue.queued_message_count();
  // port_status->queued_num_bytes = port->message_queue.queued_num_bytes();
  // port_status->unacknowledged_message_count =
  //     port->next_sequence_num_to_send - port->last_sequence_num_acknowledged -
  //     1;

  return OK;
}


int Node::SendUserMessage(const PortRef& port_ref,
                          std::unique_ptr<UserMessageEvent> message) {
  int rv = SendUserMessageInternal(port_ref, &message);

  if (rv != OK) {
    // If send failed, close all carried ports. Note that we're careful not to
    // close the sending port itself if it happened to be one of the encoded
    // ports (an invalid but possible condition.)
    for (size_t i = 0; i < message->num_ports(); ++i) {
      if (message->ports()[i] == port_ref.name()) {
        continue;
      }

      PortRef port;
      if (GetPort(message->ports()[i], &port) == OK) {
        ClosePort(port);
      }
    }
  }

  return rv;
}


int Node::SendUserMessageInternal(const PortRef& port_ref,
                                  std::unique_ptr<UserMessageEvent>* message) {
  std::unique_ptr<UserMessageEvent>& m = *message;

  m->set_from_port(port_ref.name());

  for (size_t i = 0; i < m->num_ports(); ++i) {
    if (m->ports()[i] == port_ref.name()) {
      return ERROR_PORT_CANNOT_SEND_SELF;
    }
  }

  NodeName target_node;
  int rv = PrepareToForwardUserMessage(port_ref, Port::kReceiving,
                                       false /* ignore_closed_peer */, m.get(),
                                       &target_node);
  if (rv != OK) {
    return rv;
  }

  // Beyond this point there's no sense in returning anything but OK. Even if
  // message forwarding or acceptance fails, there's nothing the embedder can
  // do to recover. Assume that failure beyond this point must be treated as a
  // transport failure.

  // DCHECK_NE(kInvalidNodeName, target_node);
  if (target_node != name_) {
    delegate_->ForwardEvent(target_node, std::move(m));
    return OK;
  }
  int accept_result = AcceptEvent(name_, std::move(m));
  if (accept_result != OK) {
    // See comment above for why we don't return an error in this case.
    // DVLOG(2) << "AcceptEvent failed: " << accept_result;
  }
  return OK;
}

int Node::PrepareToForwardUserMessage(const PortRef &forwarding_port_ref,
                                      Port::State expected_port_state,
                                      bool ignore_closed_peer,
                                      UserMessageEvent *message,
                                      NodeName *forward_to_node) {
  return OK;
}


int Node::AcceptEvent(const NodeName& from_node, ScopedEvent event) {
  PortRef port_ref;
  GetPort(event->port_name(), &port_ref);
  return AcceptEventInternal(port_ref, from_node, std::move(event));
}


int Node::AcceptEventInternal(const PortRef& port_ref,
                              const NodeName& from_node,
                              ScopedEvent event) {
  switch (event->type()) {
    case Event::Type::kUserMessage:
      return OnUserMessage(port_ref, from_node,
                           Event::Cast<UserMessageEvent>(&event));
    // case Event::Type::kPortAccepted:
    //   return OnPortAccepted(port_ref, Event::Cast<PortAcceptedEvent>(&event));
    // case Event::Type::kObserveProxy:
    //   return OnObserveProxy(port_ref, Event::Cast<ObserveProxyEvent>(&event));
    // case Event::Type::kObserveProxyAck:
    //   return OnObserveProxyAck(port_ref,
    //                            Event::Cast<ObserveProxyAckEvent>(&event));
    // case Event::Type::kObserveClosure:
    //   return OnObserveClosure(port_ref,
    //                           Event::Cast<ObserveClosureEvent>(&event));
    // case Event::Type::kMergePort:
    //   return OnMergePort(port_ref, from_node,
    //                      Event::Cast<MergePortEvent>(&event));
    // case Event::Type::kUserMessageReadAckRequest:
    //   return OnUserMessageReadAckRequest(
    //       port_ref, Event::Cast<UserMessageReadAckRequestEvent>(&event));
    // case Event::Type::kUserMessageReadAck:
    //   return OnUserMessageReadAck(port_ref,
    //                               Event::Cast<UserMessageReadAckEvent>(&event));
    // case Event::Type::kUpdatePreviousPeer:
    //   return OnUpdatePreviousPeer(port_ref,
    //                               Event::Cast<UpdatePreviousPeerEvent>(&event));
  }
  return (ERROR_NOT_IMPLEMENTED);
}


int Node::OnUserMessage(const PortRef& port_ref,
                        const NodeName& from_node,
                        std::unique_ptr<UserMessageEvent> message) {
  if (from_node != name_) {
    for (size_t i = 0; i < message->num_ports(); ++i) {
      Event::PortDescriptor& descriptor = message->port_descriptors()[i];
      int rv = AcceptPort(message->ports()[i], descriptor);
      if (rv != OK) {
        return rv;
      }
    }
  }

  bool has_next_message = false;
  bool message_accepted = false;
  bool should_forward_messages = false;

  if (port_ref.is_valid()) {
    // SinglePortLocker locker(&port_ref);
    // auto* port = locker.port();
    auto* port = port_ref.port();

    // Reject spurious messages if we've already received the last expected
    // message.
    if (CanAcceptMoreMessages(port)) {
      message_accepted = true;
      port->message_queue.AcceptMessage(std::move(message), &has_next_message);

      if (port->state == Port::kBuffering) {
        has_next_message = false;
      } else if (port->state == Port::kProxying) {
        has_next_message = false;
        should_forward_messages = true;
      }
    }
  }

  // if (should_forward_messages) {
  //   int rv = ForwardUserMessagesFromProxy(port_ref);
  //   if (rv != OK) {
  //     return rv;
  //   }
  //   TryRemoveProxy(port_ref);
  // }

  if (!message_accepted) {
    // DVLOG(2) << "Message not accepted!\n";
    // Close all newly accepted ports as they are effectively orphaned.
    for (size_t i = 0; i < message->num_ports(); ++i) {
      PortRef attached_port_ref;
      if (GetPort(message->ports()[i], &attached_port_ref) == OK) {
        ClosePort(attached_port_ref);
      } else {
        // DLOG(WARNING) << "Cannot close non-existent port!\n";
      }
    }
  } else if (has_next_message) {
    delegate_->PortStatusChanged(port_ref);
  }

  return OK;
}

int Node::AcceptPort(const PortName& port_name,
                     const Event::PortDescriptor& port_descriptor) {
#if 0
  scoped_refptr<Port> port =
      base::MakeRefCounted<Port>(port_descriptor.next_sequence_num_to_send,
                                 port_descriptor.next_sequence_num_to_receive);
  port->state = Port::kReceiving;
  port->peer_node_name = port_descriptor.peer_node_name;
  port->peer_port_name = port_descriptor.peer_port_name;
  port->next_control_sequence_num_to_send = kInitialSequenceNum;
  port->next_control_sequence_num_to_receive = kInitialSequenceNum;
  port->prev_node_name = port_descriptor.referring_node_name;
  port->prev_port_name = port_descriptor.referring_port_name;
  port->last_sequence_num_to_receive =
      port_descriptor.last_sequence_num_to_receive;
  port->peer_closed = port_descriptor.peer_closed;


  // A newly accepted port is not signalable until the message referencing the
  // new port finds its way to the consumer (see GetMessage).
  port->message_queue.set_signalable(false);

  int rv = AddPortWithName(port_name, std::move(port));
  if (rv != OK) {
    return rv;
  }

  // Allow referring port to forward messages.
  delegate_->ForwardEvent(port_descriptor.referring_node_name,
                          std::make_unique<PortAcceptedEvent>(
                              port_descriptor.referring_port_name,
                              kInvalidPortName, kInvalidSequenceNum));
#endif
  return OK;
}


int Node::GetPort(const PortName& port_name, PortRef* port_ref) {
  // PortLocker::AssertNoPortsLockedOnCurrentThread();
  // base::AutoLock lock(ports_lock_);
  auto iter = ports_.find(port_name);
  if (iter == ports_.end()) {
    return ERROR_PORT_UNKNOWN;
  }

  *port_ref = PortRef(port_name, iter->second);
  return OK;
}

int Node::ClosePort(const PortRef& port_ref) {
  std::vector<std::unique_ptr<UserMessageEvent>> undelivered_messages;
  NodeName peer_node_name;
  PortName peer_port_name;
  uint64_t sequence_num = 0;
  uint64_t last_sequence_num = 0;
  bool was_initialized = false;
  {
    // SinglePortLocker locker(&port_ref);
    // auto* port = locker.port();
    auto* port = port_ref.port();
    switch (port->state) {
      case Port::kUninitialized:
        break;

      case Port::kReceiving:
        was_initialized = true;
        port->state = Port::kClosed;
#if 0
        // We pass along the sequence number of the last message sent from this
        // port to allow the peer to have the opportunity to consume all inbound
        // messages before notifying the embedder that this port is closed.
        last_sequence_num = port->next_sequence_num_to_send - 1;

        peer_node_name = port->peer_node_name;
        peer_port_name = port->peer_port_name;

        sequence_num = port->next_control_sequence_num_to_send++;

        // If the port being closed still has unread messages, then we need to
        // take care to close those ports so as to avoid leaking memory.
        port->message_queue.TakeAllMessages(&undelivered_messages);
        port->TakePendingMessages(undelivered_messages);
#endif
        break;

      default:
        return ERROR_PORT_STATE_UNEXPECTED;
    }
  }

  ErasePort(port_ref.name());

  if (was_initialized) {
    // DVLOG(2) << "Sending ObserveClosure from " << port_ref.name() << "@"
    //          << name_ << " to " << peer_port_name << "@" << peer_node_name;
    // delegate_->ForwardEvent(
    //     peer_node_name,
    //     std::make_unique<ObserveClosureEvent>(peer_port_name, port_ref.name(),
    //                                           sequence_num, last_sequence_num));
    for (const auto& message : undelivered_messages) {
      for (size_t i = 0; i < message->num_ports(); ++i) {
        PortRef ref;
        if (GetPort(message->ports()[i], &ref) == OK) {
          ClosePort(ref);
        }
      }
    }
  }
  return OK;
}

void Node::ErasePort(const PortName& port_name) {
#if 0
  PortLocker::AssertNoPortsLockedOnCurrentThread();
  scoped_refptr<Port> port;
  {
    base::AutoLock lock(ports_lock_);
    auto it = ports_.find(port_name);
    if (it == ports_.end()) {
      return;
    }
    port = std::move(it->second);
    ports_.erase(it);

    RemoveFromPeerPortMap(port_name, port.get());
  }
  // NOTE: We are careful not to release the port's messages while holding any
  // locks, since they may run arbitrary user code upon destruction.
  std::vector<std::unique_ptr<UserMessageEvent>> messages;
  {
    PortRef port_ref(port_name, std::move(port));
    SinglePortLocker locker(&port_ref);
    locker.port()->message_queue.TakeAllMessages(&messages);
  }
  DVLOG(2) << "Deleted port " << port_name << "@" << name_;
#endif  
}

}  // namespace ports
}  // namespace core
}  // namespace mojo
