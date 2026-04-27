
#ifndef NET_HTTP_HTTP_TRANSACTION_H_
#define NET_HTTP_HTTP_TRANSACTION_H_

namespace net {

class HttpTransaction {
public:
  virtual int Start() = 0;
};

}

#endif