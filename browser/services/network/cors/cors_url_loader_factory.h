#ifndef SERVICES_NETWORK_CORS_CORS_URL_LOADER_FACTORY_H_
#define SERVICES_NETWORK_CORS_CORS_URL_LOADER_FACTORY_H_

#include <memory>

namespace network::cors {

class CorsURLLoaderFactory {
 public:
  CorsURLLoaderFactory();
  ~CorsURLLoaderFactory();

  void CreateLoaderAndStart();

 private:
  // 禁止拷贝
  CorsURLLoaderFactory(const CorsURLLoaderFactory&) = delete;
  CorsURLLoaderFactory& operator=(const CorsURLLoaderFactory&) = delete;
};

}  // namespace network::cors

#endif  // SERVICES_NETWORK_CORS_CORS_URL_LOADER_FACTORY_H_
