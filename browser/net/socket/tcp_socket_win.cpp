#include "net/socket/tcp_socket_win.h"

#include "base/win/object_watcher.h"
#include "net/base/sockaddr_storage.h"
#include "net/base/net_errors.h"
#include "net/base/winsock_init.h"
#include "net/base/ip_endpoint.h"

#include <winsock2.h>
#include <ws2tcpip.h>
// #pragma comment(lib, "Ws2_32.lib")

#include <mstcpip.h>

namespace net {


class TCPSocketDefaultWin : public TCPSocketWin {
public:
  class CoreImpl;
private:
  CoreImpl& GetCoreImpl();
};

class TCPSocketDefaultWin::CoreImpl : public TCPSocketWin::Core {
public:
  CoreImpl(TCPSocketDefaultWin* socket) :
      read_event_(WSACreateEvent()),
      socket_(socket),
      reader_(this) {
  }
  void Detach() override {

  }
  HANDLE GetConnectEvent() override {
      // `read_event_` is used to watch for connect.
  return read_event_;
  }
  void WatchForConnect() override {
    // `read_event_` is used to watch for connect.
    WatchForRead();
  }

  // Start watching for the end of a read or write operation.
  void WatchForRead() {
    read_watcher_.StartWatchingOnce(read_event_, &reader_);
  }
  // void WatchForWrite() {
  //   write_watcher_.StartWatchingOnce(write_overlapped_.hEvent, &writer_);
  // }
  void OnRead(HANDLE object) {
  // if (core_->socket_->connect_callback_) {
  //   core_->socket_->DidCompleteConnect();
  // } else {
  //   core_->socket_->DidSignalRead();
  // }
    assert(false);
  }

  // Event handle for monitoring connect and read events through WSAEventSelect.
  HANDLE read_event_;
  
  // The socket that created this object.
  TCPSocketDefaultWin* socket_;

  
private:
  class ReadDelegate : public base::win::ObjectWatcher::Delegate {
  public:
    explicit ReadDelegate(CoreImpl *core) : core_(core) {}
    void OnObjectSignaled(HANDLE object) override { core_->OnRead(object); }

  private:
    CoreImpl *core_;
  };

  // |reader_| handles the signals from |read_watcher_|.
  ReadDelegate reader_;
  // |writer_| handles the signals from |write_watcher_|.
  // WriteDelegate writer_;

  // |read_watcher_| watches for events from Connect() and Read().
  base::win::ObjectWatcher read_watcher_;
  // |write_watcher_| watches for events from Write();
  base::win::ObjectWatcher write_watcher_;
};

TCPSocketDefaultWin::CoreImpl& TCPSocketDefaultWin::GetCoreImpl() {
  return *(static_cast<TCPSocketDefaultWin::CoreImpl*>(core_.get()));
}


// 通过 TCPSocket::Create() 调用
// static 
std::unique_ptr<TCPSocketWin> TCPSocketWin::Create() {
  // if (base::FeatureList::IsEnabled(features::kTcpSocketIoCompletionPortWin)) {
  //   return std::make_unique<TcpSocketIoCompletionPortWin>(
  //       std::move(socket_performance_watcher), net_log, source);
  // }
  return std::make_unique<TCPSocketDefaultWin>(
      /*std::move(socket_performance_watcher), net_log, source*/);
}

UINT_PTR CreatePlatformSocket(int family, int type, int protocol) {
// #if BUILDFLAG(IS_WIN)
  EnsureWinsockInit();
  SOCKET result = ::WSASocket(family, type, protocol, nullptr, 0,
                                        WSA_FLAG_OVERLAPPED);
  if (result != INVALID_SOCKET && family == AF_INET6) {
    DWORD value = 0;
    if (setsockopt(result, IPPROTO_IPV6, IPV6_V6ONLY,
                   reinterpret_cast<const char*>(&value), sizeof(value))) {
      closesocket(result);
      return INVALID_SOCKET;
    }
  }
  return result;
// #elif BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)
//   SocketDescriptor result = ::socket(family, type, protocol);
//   return result;
// #endif  // BUILDFLAG(IS_WIN)
}


bool SetNonBlocking(int fd) {
  unsigned long nonblocking = 1;
  if (ioctlsocket(static_cast<SOCKET>(fd), static_cast<long>(FIONBIO),
                  &nonblocking) == 0) {
    return true;
  }
  return false;
}

bool SetNonBlockingAndGetError(int fd, int* os_error) {
  bool ret = /*base::*/SetNonBlocking(fd);
  *os_error = WSAGetLastError();

  return ret;
}

int TCPSocketWin::Open(AddressFamily family) {
  // DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  // DCHECK_EQ(socket_, INVALID_SOCKET);

  socket_ = CreatePlatformSocket(ConvertAddressFamily(family), SOCK_STREAM,
                                 IPPROTO_TCP);
  int os_error = WSAGetLastError();
  if (socket_ == INVALID_SOCKET) {
    // PLOG(ERROR) << "CreatePlatformSocket() returned an error";
    return MapSystemError(os_error);
  }

  if (!SetNonBlockingAndGetError((int)socket_, &os_error)) {
    int result = MapSystemError(os_error);
    Close();
    return result;
  }

  // return OK;
  return 0;
}

void TCPSocketWin::Close() {
  if (socket_ != INVALID_SOCKET) {
    shutdown(socket_, SD_SEND);

    // This cancels any pending IO.
    closesocket(socket_);
    socket_ = INVALID_SOCKET;
  }
}

//
// 设置值为 1：禁用 Nagle 算法
// 设置值为 0：启用 Nagle 算法（默认行为）
// Nagle 算法会将多个小数据包合并为一个大的数据包发送，旨在提高网络利用率、减少小包数量，但会增加延迟。
//
int SetTCPNoDelay(SocketDescriptor fd, bool no_delay) {
// #if BUILDFLAG(IS_WIN)
  BOOL on = no_delay ? TRUE : FALSE;
// #elif BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)
//   int on = no_delay ? 1 : 0;
// #endif
  int rv = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
                      reinterpret_cast<const char*>(&on), sizeof(on));
  return rv == -1 ? MapSystemError(errno) : OK;
}

// Disable Nagle.
// Enable TCP Keep-Alive to prevent NAT routers from timing out TCP
// connections. See http://crbug.com/27400 for details.
bool SetTCPKeepAlive(SOCKET socket, BOOL enable, int delay_secs) {
  unsigned delay = delay_secs * 1000;
  struct tcp_keepalive keepalive_vals = {
      enable ? 1u : 0u,  // TCP keep-alive on.
      delay,  // Delay seconds before sending first TCP keep-alive packet.
      delay,  // Delay seconds between sending TCP keep-alive packets.
  };
  DWORD bytes_returned = 0xABAB;
  int rv = WSAIoctl(socket, SIO_KEEPALIVE_VALS, &keepalive_vals,
                    sizeof(keepalive_vals), nullptr, 0, &bytes_returned,
                    nullptr, nullptr);
  int os_error = WSAGetLastError();
  return rv == 0;
}


void TCPSocketWin::SetDefaultOptionsForClient() {
  SetTCPNoDelay(socket_, /*no_delay=*/true);
  SetTCPKeepAlive(socket_, true, 45/*kTCPKeepAliveSeconds*/);
}

int TCPSocketWin::Connect(const IPEndPoint& address/*, CompletionOnceCallback callback*/) {
  peer_address_ = std::make_unique<IPEndPoint>(address);
  int rv = DoConnect();
  return rv;
}


int TCPSocketWin::DoConnect() {
  // core_ = CreateCore();
  core_ =  std::make_shared<TCPSocketDefaultWin::CoreImpl>(static_cast<TCPSocketDefaultWin*>(this));

  WSAEventSelect(socket_, core_->GetConnectEvent(), FD_CONNECT);

  SockaddrStorage storage;
  if (!peer_address_->ToSockAddr(storage.addr(), &storage.addr_len)) {
    return ERR_ADDRESS_INVALID;
  }

  // Set option to choose a random port, if the socket is not already bound.
  // Ignore failures, which may happen if the socket was already bound.
  // if (UseTcpPortRandomization()) {
    BOOL randomize_port = TRUE;
    setsockopt(socket_, SOL_SOCKET, SO_RANDOMIZE_PORT,
               reinterpret_cast<const char*>(&randomize_port),
               sizeof(randomize_port));
  // }

  int ret = connect(socket_, storage.addr(), storage.addr_len);
  core_->WatchForConnect();

  return ERR_IO_PENDING;
}
}