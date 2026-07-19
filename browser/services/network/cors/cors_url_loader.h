#ifndef SERVICES_NETWORK_CORS_CORSURLLOADER_H_
#define SERVICES_NETWORK_CORS_CORSURLLOADER_H_

#include <memory>
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/url_loader_completion_status.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "services/network/public/mojom/url_loader.mojom.h"

namespace network {
class NetworkContext;
class URLLoaderFactory;

// 负责执行CORS 算法。在请求发起前和收到响应后，检查是否符合 CORS 规则，只允许安全的跨域请求通过
/*
`CorsURLLoader` 是 Chromium 网络栈中负责**跨域资源共享（CORS，Cross-Origin
Resource
Sharing）核心校验**的类。它相当于一个"守门员"，在资源请求发出前和响应返回时，确保其符合浏览器的同源策略和安全规范。

具体来说，其主要作用体现在以下三个方面：

1.  **执行 CORS 协议校验**
    它是 CORS 逻辑的核心执行者。对于跨域请求，它负责检查请求的头部信息（如
`Origin`）是否符合规范，并处理**预检请求**（Preflight，即 OPTIONS
请求），确保服务器明确允许跨域访问。同时，它还会根据跨域规则设置请求的**"染色"状态**（例如标记为
`opaque`），供渲染进程后续处理。

2.  **管理请求的拦截与转发**
    在资源加载流程中，`CorsURLLoader`
充当"中间件"的角色。它会拦截由上层发起的网络请求，在应用跨域规则校验后，再将合法的请求转发给底层的
`URLLoader`（实际网络请求执行者）。响应返回时，它同样会先进行跨域检查，再决定是否将数据传递给请求发起方。

3.  **实现 Fetch 标准中的 TAO 检查**
    `CorsURLLoader` 还负责执行 **TAO（Timing-Allow-Origin）**
检查。这个检查决定了是否允许跨域请求暴露详细的资源计时信息（如通过
`PerformanceResourceTiming`
API）。只有当服务器的响应头允许时，开发者才能获取到精确的网络性能数据，否则相关时间信息会被限制或置零。

> **补充说明**：搜索结果中提到的 TAO 检查，是在 `CorsURLLoader` 的
`Timing-Allow-Origin` 逻辑中实现的。
*/

class CorsURLLoader : public mojom::URLLoaderClient {
public:
  CorsURLLoader(const ResourceRequest &resource_request,
                mojom::URLLoaderClient *client, URLLoaderFactory *factory);
  ~CorsURLLoader();

  void Start();
  void SetCorsFlagIfNeeded();
  bool HasSpecialAccessToDestination() const;

  void StartRequest();
  void StartNetworkRequest();

  void OnReceiveResponse(mojom::URLResponseHead* response_head) override;
  void OnComplete(const URLLoaderCompletionStatus& status) override;

  void HandleComplete(URLLoaderCompletionStatus status);

  static std::optional<std::string>
  GetHeaderString(const mojom::URLResponseHead &response,
                  const std::string &header_name);

private:
  bool fetch_cors_flag_ = false;
  URLLoaderFactory* m_network_loader_factory = nullptr;
  ResourceRequest request_;

  // mojo::Remote<mojom::URLLoaderClient> forwarding_client_;
  mojom::URLLoaderClient* forwarding_client_ = nullptr;;

};

}

#endif