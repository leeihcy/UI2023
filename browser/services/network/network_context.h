#ifndef _SERVICES_NETWORK_NETWORK_CONTEXT_H_
#define _SERVICES_NETWORK_NETWORK_CONTEXT_H_

#include <vector>
#include <memory>
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/prefetch_matching_url_loader_factory.h"
#include "net/url_request/url_request_context.h"

namespace network {
class PrefetchMatchingURLLoaderFactory;

// 代表一个独立的网络请求上下文，拥有自己的存储（如 Cookie 和缓存）
//
// 生命周期：
// 1. 由NetworkService单例负责创建，因此它生存在network进程中。
// 2. 
//
// NetworkContext 负责创建 URLLoaderFactory 对象。
//
//   关键特性
// 
//   1. 独立存储
//     - Cookie 管理器 (CookieManager)
//     - HTTP 缓存 (HttpCache)
//     - DNS 缓存 (HostResolver)
//     - HTTP Auth 缓存
//     - 共享字典 (SharedDictionaryManager)
//     - 传输安全状态 (HSTS)
//   2. Mojo 接口隔离
//     - NetworkContext 通过 Mojo 接口与 NetworkService 通信
//     - 允许多进程架构下的网络请求处理
//     - 只有特权进程（浏览器进程）可以访问

class NetworkContext {
public:
  NetworkContext() {}
  ~NetworkContext() {}

  mojom::URLLoaderFactory* CreateURLLoaderFactory();
  net::URLRequestContext* url_request_context() { return &m_url_request_context; }


private:
  std::vector<std::unique_ptr<PrefetchMatchingURLLoaderFactory>> m_url_loader_factories;

  // URLRequestContext 是整个网络栈的“核心引擎”，包含了发起网络请求所需的所有基础设施；
  // 而 NetworkContext 则是这个引擎在 Chromium 现代化架构（网络服务化）下的“对外接口”和“容器”。
  net::URLRequestContext m_url_request_context;
};

}

#endif
