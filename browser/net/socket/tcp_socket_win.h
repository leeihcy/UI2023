#ifndef NET_SOCKET_TCP_SOCKET_WIN_H_
#define NET_SOCKET_TCP_SOCKET_WIN_H_

#include <winsock2.h>

#include <functional>
#include <stdint.h>
#include <memory>

#include "net/base/address_family.h"


namespace net {
class IPEndPoint;
class IOBuffer;

// #if BUILDFLAG(IS_WIN)
typedef UINT_PTR SocketDescriptor;
// const SocketDescriptor kInvalidSocket = (SocketDescriptor)(~0);
// #elif BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)
// typedef int SocketDescriptor;
// const SocketDescriptor kInvalidSocket = -1;
// #endif

typedef std::function<void(int)> CompletionOnceCallback;

class TCPSocketWin {
public:
  static std::unique_ptr<TCPSocketWin> Create();

  int Open(AddressFamily family);
  void Close();
  void SetDefaultOptionsForClient();

  bool IsValid() const { return socket_ != INVALID_SOCKET; }

  int Connect(const IPEndPoint &address, CompletionOnceCallback callback);
  virtual int Read(IOBuffer* buf,
                   int buf_len,
                   CompletionOnceCallback callback) = 0;
  virtual int ReadIfReady(IOBuffer* buf,
                          int buf_len,
                          CompletionOnceCallback callback) = 0;
  virtual int Write(IOBuffer *buf, int buf_len,
                    CompletionOnceCallback callback) = 0;

  class Core {
  public:
    Core() {}
    virtual ~Core() {}
    // Invoked when the socket is closed. Clears any reference from the `Core`
    // to its parent socket.
    virtual void Detach() = 0;

    // Returns the event to use for watching the completion of a connect()
    // operation.
    virtual HANDLE GetConnectEvent() = 0;

    // Must be invoked after initiating a connect() operation. Will invoke
    // `DidCompleteConnect()` when the connect() operation is complete.
    virtual void WatchForConnect() = 0;
  };

public:
  void DidCompleteConnect();
  void DidSignalRead();

private:
  int DoConnect();
  
protected:
  SOCKET socket_ = INVALID_SOCKET;
  std::unique_ptr<IPEndPoint> peer_address_;

  // The core of the socket that can live longer than the socket itself. We pass
  // resources to the Windows async IO functions and we have to make sure that
  // they are not destroyed while the OS still references them.
  std::shared_ptr<Core> core_;

  CompletionOnceCallback connect_callback_;
};

}

#endif