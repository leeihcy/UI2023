#include "mojo/core/ports/node.h"

#include <random>

namespace mojo {
namespace core {
namespace ports {
constexpr uint64_t kInitialSequenceNum = 1;

static void GenerateRandomPortName(PortName* name) {
  // 玩具版：直接用随机数填满两个64位字段
  // 在真实代码中绝对不要这样用！！
  
  // 用标准库的随机数生成器（Mersenne Twister）
  static std::random_device rd;           // 获取随机种子
  static std::mt19937_64 gen(rd());       // 64位随机数生成器
  static std::uniform_int_distribution<uint64_t> dist;  // 均匀分布
  
  name->v1 = dist(gen);
  name->v2 = dist(gen);
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
    auto* port = port_ref.port();
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
                                 Port* local_port,
                                 const NodeName& new_peer_node,
                                 const PortName& new_peer_port) {
  RemoveFromPeerPortMap(local_port_name, local_port);
  local_port->peer_node_name = new_peer_node;
  local_port->peer_port_name = new_peer_port;
  local_port->next_control_sequence_num_to_send = kInitialSequenceNum;

  if (new_peer_port != kInvalidPortName) {
    peer_port_maps_[new_peer_node][new_peer_port].emplace(
        local_port_name,
        PortRef(local_port_name, std::shared_ptr<Port>(local_port)));
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

  port->user_data = std::move(user_data);

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

  *user_data = port->user_data;

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

}  // namespace ports
}  // namespace core
}  // namespace mojo
