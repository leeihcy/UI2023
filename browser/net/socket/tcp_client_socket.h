#ifndef NET_SOCKET_TCP_CLIENT_SOCKET_H_
#define NET_SOCKET_TCP_CLIENT_SOCKET_H_

#include "net/base/address_list.h"
#include "net/socket/tcp_socket.h"
#include "net/socket/transport_client_socket.h"

namespace net {

// A client socket that uses TCP as the transport layer.
class TCPClientSocket : public TransportClientSocket {
public:
   TCPClientSocket(const AddressList& addresses);

  int Connect(CompletionOnceCallback callback) override;
  void Disconnect() override;
  bool IsConnected() const override;
  bool IsConnectedAndIdle() const override;
  int OpenSocket(AddressFamily family);

  // TransportClientSocket implementation.
  int Bind(const IPEndPoint& address) override;
  bool SetKeepAlive(bool enable, int delay) override;
  bool SetNoDelay(bool no_delay) override;
  
  // StreamSocket implementation.
  void SetBeforeConnectCallback(
      const BeforeConnectCallback& before_connect_callback) override;
  int GetPeerAddress(IPEndPoint* address) const override;
  int GetLocalAddress(IPEndPoint* address) const override;
  // const NetLogWithSource& NetLog() const override;
  bool WasEverUsed() const override;
  NextProto GetNegotiatedProtocol() const override;
  bool GetSSLInfo(SSLInfo* ssl_info) override;
  int64_t GetTotalReceivedBytes() const override;
  void ApplySocketTag(const SocketTag& tag) override;

  // Socket implementation.
  int Read(IOBuffer* buf,
           int buf_len,
           CompletionOnceCallback callback) override;
  int ReadIfReady(IOBuffer* buf,
                  int buf_len,
                  CompletionOnceCallback callback) override;
  int CancelReadIfReady() override;
  int Write(IOBuffer* buf,
            int buf_len,
            CompletionOnceCallback callback) override;
  int SetReceiveBufferSize(int32_t size) override;
  int SetSendBufferSize(int32_t size) override;

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