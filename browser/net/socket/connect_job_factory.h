#ifndef NET_SOCKET_CONNECT_JOB_FACTORY_H_
#define NET_SOCKET_CONNECT_JOB_FACTORY_H_

#include "net/socket/transport_connect_job.h"
#include <memory>


namespace net {
class ConnectJob;

class ConnectJobFactory {
public:
  ConnectJobFactory();

  std::unique_ptr<ConnectJob> CreateConnectJob();

  // 用途：处理 HTTP 代理（包括 CONNECT 方法）的连接建立。
  // 场景：当配置了 HTTP 代理，并且需要与目标服务器建立隧道（例如 HTTPS 请求）时使用。
  // 它会先与代理服务器建立连接，发送 CONNECT 请求，然后处理代理的响应，最终建立到目标服务器的隧道。
  // std::unique_ptr<HttpProxyConnectJob::Factory> http_proxy_connect_job_factory_;

  // 用途：处理 SOCKS 代理（通常为 SOCKS4/SOCKS5）的连接建立。
  // 场景：当配置了 SOCKS 代理（如 SOCKS4 或 SOCKS5）时使用。它会按照 SOCKS 协议与代理服务器握手、
  // 进行身份验证（如果需要），并请求代理连接到目标主机。
  // std::unique_ptr<SOCKSConnectJob::Factory> socks_connect_job_factory_;


  // 用途：处理 直接通过 TLS/SSL 加密的连接（即 HTTPS 直连，不经过代理）。
  // 场景：当目标 URL 是 HTTPS，且没有使用代理（或代理是透传模式）时使用。它在建立 TCP 连接后，
  // 立即开始 TLS 握手（ClientHello 等）。
  // std::unique_ptr<SSLConnectJob::Factory> ssl_connect_job_factory_;

  // 用途：处理 普通 TCP 连接（明文传输，无代理，无加密）。
  // 场景：当目标 URL 是 HTTP（非加密）且没有使用任何代理时使用。
  // 它只负责完成 TCP 三次握手和可选的连接建立后设置（如 TCP Fast Open）。
  std::unique_ptr<TransportConnectJob::Factory> m_transport_connect_job_factory;
};


}

#endif