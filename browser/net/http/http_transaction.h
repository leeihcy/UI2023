
#ifndef NET_HTTP_HTTP_TRANSACTION_H_
#define NET_HTTP_HTTP_TRANSACTION_H_

namespace net {

/*
在 Chromium 的网络栈中，HttpTransaction
接口及其实现扮演着单一HTTP请求/响应交互的总控制器角色。它位于网络请求处理流程的中间层，对上层的
URLRequest
屏蔽了底层网络通信、缓存、认证等复杂细节，对下层则负责协调和管理一个完整HTTP事务的整个生命周期。
*/
class HttpTransaction {
public:
  virtual int Start() = 0;
};

} // namespace net

#endif