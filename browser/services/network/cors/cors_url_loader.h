#ifndef SERVICES_NETWORK_CORS_CORSURLLOADER_H_
#define SERVICES_NETWORK_CORS_CORSURLLOADER_H_

namespace network {

// 负责执行CORS 算法。在请求发起前和收到响应后，检查是否符合 CORS 规则，只允许安全的跨域请求通过
class CorsURLLoader {
public:
  CorsURLLoader();

  void Start();
  void SetCorsFlagIfNeeded();
  bool HasSpecialAccessToDestination() const;

  void StartRequest();
  void StartNetworkRequest();

private:
  bool fetch_cors_flag_ = false;
};

}

#endif