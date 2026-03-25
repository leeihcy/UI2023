#include "services/network/public/cpp/simple_url_loader.h"
#include <memory>

namespace network {

// static
std::unique_ptr<SimpleURLLoader>
SimpleURLLoader::Create(std::unique_ptr<ResourceRequest> resource_request) {
  return std::make_unique<SimpleURLLoader>(std::move(resource_request));
}

SimpleURLLoader::SimpleURLLoader(std::unique_ptr<ResourceRequest> resource_request) 
  : m_resource_request(std::move(resource_request))
{

}

void SimpleURLLoader::Start(URLLoaderFactory* url_loader_factory) {
  StartRequest(url_loader_factory);
}

void SimpleURLLoader::StartRequest(URLLoaderFactory* url_loader_factory) {
  url_loader_factory->CreateLoaderAndStart(*m_resource_request);

  // TODO:
  // 1. 超时回调
  // 2. 重试次数
}


} // namespace network