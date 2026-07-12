#include "net/socket/tcp_client_socket.h"

#include <assert.h>

namespace net {

TCPClientSocket::TCPClientSocket(const AddressList& addresses) : addresses_(addresses) {
  socket_ = TCPSocket::Create();
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

  // 禁用Naggle、启用KeepAlive
  socket_->SetDefaultOptionsForClient();
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

  //return ConnectInternal(endpoint);
  return socket_->Connect(endpoint, std::bind(&TCPClientSocket::DidCompleteConnect, this, std::placeholders::_1));
}

int TCPClientSocket::DoConnectComplete(int result) { return 0; }

//
// >	net.dll!net::HttpStreamPool::TcpBasedAttempt::OnAttemptComplete	C++
// >	net.dll!net::TlsStreamAttempt::OnIOComplete	C++
// >	net.dll!net::TcpStreamAttempt::OnIOComplete	C++
//  	net.dll!net::TCPClientSocket::DidCompleteConnect	C++
//   	net.dll!net::TCPSocketWin::DidCompleteConnect	C++
//
void TCPClientSocket::DidCompleteConnect(int result) {
  assert(next_connect_state_ == CONNECT_STATE_CONNECT_COMPLETE);
  assert(result != ERR_IO_PENDING);
  assert(connect_callback_);

  result = DoConnectLoop(result);
  if (result != ERR_IO_PENDING) {
    connect_callback_(result);
  }
}

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
  return ReadCommon(buf, buf_len, std::move(callback), /*read_if_ready=*/false);
}
int TCPClientSocket::ReadIfReady(IOBuffer *buf, int buf_len,
                                 CompletionOnceCallback callback) {
  return ReadCommon(buf, buf_len, std::move(callback), /*read_if_ready=*/true);
}

int TCPClientSocket::ReadCommon(IOBuffer* buf,
    int buf_len,
    CompletionOnceCallback callback,
    bool read_if_ready) {
    // if (was_disconnected_on_suspend_)
    //     return ERR_NETWORK_IO_SUSPENDED;

    // |socket_| is owned by |this| and the callback won't be run once |socket_|
    // is gone/closed. Therefore, it is safe to use base::Unretained() here.
    CompletionOnceCallback complete_read_callback =
        std::bind(&TCPClientSocket::DidCompleteRead, this, std::placeholders::_1);
    int result =
        read_if_ready
        ? socket_->ReadIfReady(buf, buf_len,
            std::move(complete_read_callback))
        : socket_->Read(buf, buf_len, std::move(complete_read_callback));
    if (result == ERR_IO_PENDING) {
        read_callback_ = std::move(callback);
    }
    else if (result > 0) {
        // was_ever_used_ = true;
        // total_received_bytes_ += result;
    }

    return result;
}

int TCPClientSocket::CancelReadIfReady() { return 0; }
int TCPClientSocket::Write(IOBuffer *buf, int buf_len,
                           CompletionOnceCallback callback) {
  // if (was_disconnected_on_suspend_)
  //   return ERR_NETWORK_IO_SUSPENDED;

  // // |socket_| is owned by this class and the callback won't be run once
  // // |socket_| is gone. Therefore, it is safe to use base::Unretained() here.
  CompletionOnceCallback complete_write_callback = std::bind(
      &TCPClientSocket::DidCompleteWrite, this, std::placeholders::_1);
  int result = socket_->Write(buf, buf_len, std::move(complete_write_callback));
  // if (result == ERR_IO_PENDING) {
  //   write_callback_ = std::move(callback);
  // } else if (result > 0) {
  //   was_ever_used_ = true;
  // }

  return result;
}
int TCPClientSocket::SetReceiveBufferSize(int32_t size) { return 0; }
int TCPClientSocket::SetSendBufferSize(int32_t size) { return 0; }

void TCPClientSocket::DidCompleteWrite(int result) {
  DidCompleteReadWrite(std::move(write_callback_), result);
}
void TCPClientSocket::DidCompleteReadWrite(CompletionOnceCallback callback,
                                           int result) {
  // if (result > 0)
  //   was_ever_used_ = true;
  std::move(callback)(result);
}

void TCPClientSocket::DidCompleteRead(int result) {
  // if (result > 0)
  //   total_received_bytes_ += result;
  DidCompleteReadWrite(std::move(read_callback_), result);
}

} // namespace net