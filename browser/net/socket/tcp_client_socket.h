#ifndef NET_SOCKET_TCP_CLIENT_SOCKET_H_
#define NET_SOCKET_TCP_CLIENT_SOCKET_H_

#include "net/base/address_list.h"
#include "net/socket/tcp_socket.h"
#include "net/socket/transport_client_socket.h"

namespace net {

// A client socket that uses TCP as the transport layer.
class TCPClientSocket : public TransportClientSocket {
public:
  int Connect(CompletionOnceCallback callback) override;
  void Disconnect() override;
  bool IsConnected() const override;
  bool IsConnectedAndIdle() const override;
  int OpenSocket(AddressFamily family);

private:
  // State machine used by Connect().
  int DoConnectLoop(int result);
  int DoConnect();
  int DoConnectComplete(int result);

  void OnConnectAttemptTimeout();

private:
  // External callbacks; called when corresponding operations are complete.
  // Cleared when no such operation is pending.
  CompletionOnceCallback connect_callback_;
  CompletionOnceCallback read_callback_;
  CompletionOnceCallback write_callback_;

  // State machine for connecting the socket.
  enum ConnectState {
    CONNECT_STATE_CONNECT,
    CONNECT_STATE_CONNECT_COMPLETE,
    CONNECT_STATE_NONE,
  };

  // The next state for the Connect() state machine.
  ConnectState next_connect_state_ = CONNECT_STATE_NONE;
  
  std::unique_ptr<TCPSocket> socket_;

  // The list of addresses we should try in order to establish a connection.
  // DNS解析会返回多个IP地址，如负载均衡、IPv4/IPv6混合网络
  AddressList addresses_;

  // Where we are in above list. Set to -1 if uninitialized.
  int current_address_index_ = 0;
};

} // namespace net

#endif