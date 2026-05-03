#ifndef NET_HTTP_HTTP_NETWORK_SESSION_H_
#define NET_HTTP_HTTP_NETWORK_SESSION_H_

#include "net/http/http_stream_factory.h"
#include "net/socket/client_socket_pool_manager_impl.h"
#include "net/socket/client_socket_pool.h"
#include <memory>

namespace net {

//
// HttpNetworkSession 最主要的作用就是管理各种网络连接池，以提高网络请求的效率
//
// HttpNetworkSession 的生命周期通常与 URLRequestContext 绑定。
// 当一个 Profile（用户配置文件，对应一个浏览器实例）被创建时，它会被初始化；
// 当该 Profile 被销毁时，它也随之销毁。
//
class HttpNetworkSession {
public:
  HttpNetworkSession();

  HttpStreamFactory* http_stream_factory() {
    return m_http_stream_factory.get();
  }

  ClientSocketPool* GetSocketPool();

private:
  // std::unique_ptr<HttpStreamPool> http_stream_pool_;

  std::unique_ptr<ClientSocketPoolManager> m_normal_socket_pool_manager;

  std::unique_ptr<HttpStreamFactory> m_http_stream_factory;
};

}

#endif