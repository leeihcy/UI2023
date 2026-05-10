#include "net/socket/tcp_client_socket.h"

#include <assert.h>

namespace net {

TCPClientSocket::TCPClientSocket(const AddressList& addresses) {

}

int TCPClientSocket::Connect(CompletionOnceCallback callback) {
  // We will try to connect to each address in addresses_. Start with the
  // first one in the list.
  next_connect_state_ = CONNECT_STATE_CONNECT;
  current_address_index_ = 0;

  int rv = DoConnectLoop(OK);
  if (rv == ERR_IO_PENDING) {
    connect_callback_ = std::move(callback);
  } else {
    // socket_->EndLoggingMultipleConnectAttempts(rv);
  }

  return rv;
}

void TCPClientSocket::Disconnect() {
  assert(false);
}

bool TCPClientSocket::IsConnected() const {
  assert(false);
  return false;
}

bool TCPClientSocket::IsConnectedAndIdle() const {
  assert(false);
  return false;
}
  

int TCPClientSocket::DoConnectLoop(int result) {
  DCHECK_NE(next_connect_state_, CONNECT_STATE_NONE);

  int rv = result;
  do {
    ConnectState state = next_connect_state_;
    next_connect_state_ = CONNECT_STATE_NONE;
    switch (state) {
    case CONNECT_STATE_CONNECT:
      DCHECK_EQ(OK, rv);
      rv = DoConnect();
      break;
    case CONNECT_STATE_CONNECT_COMPLETE:
      rv = DoConnectComplete(rv);
      break;
    default:
      NOTREACHED(); //  << "bad state " << state;
    }
  } while (rv != ERR_IO_PENDING && next_connect_state_ != CONNECT_STATE_NONE);

  return rv;
}


int TCPClientSocket::OpenSocket(AddressFamily family) {
  DCHECK(!socket_->IsValid());

  int result = socket_->Open(family);
  if (result != OK)
    return result;

  // if (network_ != handles::kInvalidNetworkHandle) {
  //   result = socket_->BindToNetwork(network_);
  //   if (result != OK) {
  //     socket_->Close();
  //     return result;
  //   }
  // }

  // socket_->SetDefaultOptionsForClient();

  return OK;
}

int TCPClientSocket::DoConnect() {
  const IPEndPoint &endpoint = addresses_[current_address_index_];

  next_connect_state_ = CONNECT_STATE_CONNECT_COMPLETE;

  if (!socket_->IsValid()) {
  int result = OpenSocket(endpoint.GetFamily());
  // if (result != OK)
  //   return result;

  // if (bind_address_) {
  //   result = socket_->Bind(*bind_address_);
  //   if (result != OK) {
  //     socket_->Close();
  //     return result;
  //   }
  // }
  }
  return 0;
}

int TCPClientSocket::DoConnectComplete(int result) { return 0; }

// TransportClientSocket implementation.
int TCPClientSocket::Bind(const IPEndPoint &address) { return 0; };
bool TCPClientSocket::SetKeepAlive(bool enable, int delay) { return false; };
bool TCPClientSocket::SetNoDelay(bool no_delay) { return false; };

void TCPClientSocket::SetBeforeConnectCallback(
    const BeforeConnectCallback &before_connect_callback) {
  return;
}
int TCPClientSocket::GetPeerAddress(IPEndPoint *address) const { return 0; }
int TCPClientSocket::GetLocalAddress(IPEndPoint *address) const { return 0; }
// const TCPClientSocket::NetLogWithSource& NetLog() const { return
// NetLogWithSource(); }
bool TCPClientSocket::WasEverUsed() const { return false; }
NextProto TCPClientSocket::GetNegotiatedProtocol() const {
  return NextProto::kProtoHTTP11;
}
bool TCPClientSocket::GetSSLInfo(SSLInfo *ssl_info) { return false; }
int64_t TCPClientSocket::GetTotalReceivedBytes() const { return 0; }
void TCPClientSocket::ApplySocketTag(const SocketTag &tag) {}

int TCPClientSocket::Read(IOBuffer *buf, int buf_len,
                          CompletionOnceCallback callback) {
  return 0;
}
int TCPClientSocket::ReadIfReady(IOBuffer *buf, int buf_len,
                                 CompletionOnceCallback callback) {
  return 0;
}
int TCPClientSocket::CancelReadIfReady() { return 0; }
int TCPClientSocket::Write(IOBuffer *buf, int buf_len,
                           CompletionOnceCallback callback) {
  return 0;
}
int TCPClientSocket::SetReceiveBufferSize(int32_t size) { return 0; }
int TCPClientSocket::SetSendBufferSize(int32_t size) { return 0; }

} // namespace net