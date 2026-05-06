#ifndef NET_SOCKET_CONNECT_JOB_H_
#define NET_SOCKET_CONNECT_JOB_H_


namespace net {

class ConnectJob {
public:
  int Connect() {
    return ConnectInternal();
  }

protected:
  virtual int ConnectInternal() = 0;
};


}

#endif