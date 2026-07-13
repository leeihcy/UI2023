#include "net/socket/tcp_socket_win.h"

#include "base/win/object_watcher.h"
#include "net/base/io_buffer.h"
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

  int Write(IOBuffer *buf, int buf_len,
            CompletionOnceCallback callback) override;
  int Read(IOBuffer *buf, int buf_len,
           CompletionOnceCallback callback) override;
  int ReadIfReady(IOBuffer *buf, int buf_len,
                  CompletionOnceCallback callback) override;

  void RetryRead(int);
  void DidSignalRead();

private:
  CoreImpl& GetCoreImpl();

  // External callback; called when read is complete.
  CompletionOnceCallback read_callback_;

  // Non-null if a ReadIfReady() is to be completed asynchronously. This is an
  // external callback if user used ReadIfReady() instead of Read(), but a
  // wrapped callback on top of RetryRead() if Read() is used.
  CompletionOnceCallback read_if_ready_callback_;

  // External callback; called when write is complete.
  CompletionOnceCallback write_callback_;
};

class TCPSocketDefaultWin::CoreImpl : public TCPSocketWin::Core {
public:
  CoreImpl(TCPSocketDefaultWin* socket) :
      read_event_(WSACreateEvent()),
      socket_(socket),
      reader_(this) {
    write_overlapped_.hEvent = WSACreateEvent();
  }
  ~CoreImpl() {
    WSACloseEvent(write_overlapped_.hEvent);
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

  // Event handle for monitoring connect and read events through WSAEventSelect.
  HANDLE read_event_;
  
    // OVERLAPPED variable for overlapped writes.
  // TODO(mmenke): Can writes be switched to WSAEventSelect as well? That would
  // allow removing this class. The only concern is whether that would have a
  // negative perf impact.
  OVERLAPPED write_overlapped_ = {};

  // The socket that created this object.
  TCPSocketDefaultWin* socket_;
  
  // The buffers used in Read() and Write().
  IOBuffer* read_iobuffer_ = nullptr;
  IOBuffer* write_iobuffer_ = nullptr;
  int read_buffer_length_ = 0;
  int write_buffer_length_ = 0;

  bool non_blocking_reads_initialized_ = false;

private:
  class ReadDelegate : public base::win::ObjectWatcher::Delegate {
  public:
    explicit ReadDelegate(CoreImpl *core) : core_(core) {}
    void OnObjectSignaled(HANDLE object) override {
      if (core_->socket_->connect_callback_) {
        core_->socket_->DidCompleteConnect();
      } else {
        core_->socket_->DidSignalRead();
      }
    }

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

int TCPSocketWin::Connect(const IPEndPoint& address, CompletionOnceCallback callback) {
  connect_callback_ = callback;
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


int MapConnectError(int os_error) {
  switch (os_error) {
    // connect fails with WSAEACCES when Windows Firewall blocks the
    // connection.
    case WSAEACCES:
      return ERR_NETWORK_ACCESS_DENIED;
    case WSAETIMEDOUT:
      return ERR_CONNECTION_TIMED_OUT;
    default: {
      int net_error = MapSystemError(os_error);
      if (net_error == ERR_FAILED)
        return ERR_CONNECTION_FAILED;  // More specific than ERR_FAILED.

      // Give a more specific error when the user is offline.
      // if (net_error == ERR_ADDRESS_UNREACHABLE &&
      //     NetworkChangeNotifier::IsOffline()) {
      //   return ERR_INTERNET_DISCONNECTED;
      // }

      return net_error;
    }
  }
}

void TCPSocketWin::DidCompleteConnect() {
  int result;

  WSANETWORKEVENTS events;
  int rv = WSAEnumNetworkEvents(socket_, core_->GetConnectEvent(), &events);
  int os_error = WSAGetLastError();
  if (rv == SOCKET_ERROR) {
    result = MapSystemError(os_error);
  } else if (events.lNetworkEvents & FD_CONNECT) {
    os_error = events.iErrorCode[FD_CONNECT_BIT];
    result = MapConnectError(os_error);
  } else {
     result = ERR_UNEXPECTED;
  }

  // 清空connect_callback_，这样下次ReadDelegate::OnObjectSignaled时就能走Read逻辑
  connect_callback_(result);
  connect_callback_ = nullptr;
}

void TCPSocketDefaultWin::DidSignalRead() {
  CoreImpl& core = GetCoreImpl();

  int os_error = 0;
  WSANETWORKEVENTS network_events;
  int rv = WSAEnumNetworkEvents(socket_, core.read_event_, &network_events);
  os_error = WSAGetLastError();

  if (rv == SOCKET_ERROR) {
    rv = MapSystemError(os_error);
  } else if (network_events.lNetworkEvents) {
    rv = OK;
  } else {
    core.WatchForRead();
    return;
  }

  assert(rv != ERR_IO_PENDING);
  std::move(read_if_ready_callback_)(rv);
}

static bool ResetEventIfSignaled(WSAEVENT hEvent) {
  DWORD wait_rv = WaitForSingleObject(hEvent, 0);
  if (wait_rv == WAIT_TIMEOUT)
    return false;  // The event object is not signaled.
  DCHECK_EQ(wait_rv, static_cast<DWORD>(WAIT_OBJECT_0));
  BOOL ok = WSAResetEvent(hEvent);
  DCHECK(ok);
  return true;
}

int TCPSocketDefaultWin::Write(
    IOBuffer* buf,
    int buf_len,
    CompletionOnceCallback callback) {
  CoreImpl& core = GetCoreImpl();

  WSABUF write_buffer;
  write_buffer.len = buf_len;
  write_buffer.buf = buf->data();

  DWORD num;
  int rv = WSASend(socket_, &write_buffer, 1, &num, 0, &core.write_overlapped_,
                   nullptr);
  int os_error = WSAGetLastError();
  if (rv == 0) {
    if (ResetEventIfSignaled(core.write_overlapped_.hEvent)) {
      rv = static_cast<int>(num);
      if (rv > buf_len || rv < 0) {
        return ERR_WINSOCK_UNEXPECTED_WRITTEN_BYTES;
      }
      return rv;
    }
  } else {
    if (os_error != WSA_IO_PENDING) {
      int net_error = MapSystemError(os_error);
      return net_error;
    }
  }
  write_callback_ = std::move(callback);
  core.write_iobuffer_ = buf;
  core.write_buffer_length_ = buf_len;
  // core.WatchForWrite();
  return ERR_IO_PENDING;
}


int TCPSocketDefaultWin::Read(IOBuffer* buf,
                              int buf_len,
                              CompletionOnceCallback callback) {
  CoreImpl& core = GetCoreImpl();
  // base::Unretained() is safe because RetryRead() won't be called when |this|
  // is gone.
  int rv = ReadIfReady(
      buf, buf_len,
      std::bind(&TCPSocketDefaultWin::RetryRead, this, std::placeholders::_1));
  if (rv != ERR_IO_PENDING)
    return rv;
  read_callback_ = std::move(callback);
  core.read_iobuffer_ = buf;
  core.read_buffer_length_ = buf_len;
  return ERR_IO_PENDING;
}

int TCPSocketDefaultWin::ReadIfReady(IOBuffer* buf,
                                     int buf_len,
                                     CompletionOnceCallback callback) {
  CoreImpl& core = GetCoreImpl();
  if (!core.non_blocking_reads_initialized_) {
    // 初始化异步read/close监听，用event来关联异步读取事件。
    WSAEventSelect(socket_, core.read_event_, FD_READ | FD_CLOSE);
    core.non_blocking_reads_initialized_ = true;
  }

  int rv = recv(socket_, buf->data(), buf_len, 0);
  int os_error = WSAGetLastError();
  if (rv == SOCKET_ERROR) {
    if (os_error != WSAEWOULDBLOCK) {
      int net_error = MapSystemError(os_error);
      return net_error;
    }
  } else {
    return rv;
  }

  // 目前还没有可读数据，等待==>TCPSocketDefaultWin::DidSignalRead
  read_if_ready_callback_ = std::move(callback);
  core.WatchForRead();
  return ERR_IO_PENDING;
}


void TCPSocketDefaultWin::RetryRead(int rv) {
  CoreImpl& core = GetCoreImpl();

  if (rv == OK) {
    // base::Unretained() is safe because RetryRead() won't be called when
    // |this| is gone.
    rv = ReadIfReady(core.read_iobuffer_, core.read_buffer_length_,
                     std::bind(&TCPSocketDefaultWin::RetryRead,
                                    this, std::placeholders::_1));
    if (rv == ERR_IO_PENDING)
      return;
  }
  core.read_iobuffer_ = nullptr;
  core.read_buffer_length_ = 0;
  std::move(read_callback_)(rv);
}

}