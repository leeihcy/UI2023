#ifndef SERVICES_NETWORK_PUBLIC_CPP_SIMPLEURLLOADER_H_
#define SERVICES_NETWORK_PUBLIC_CPP_SIMPLEURLLOADER_H_

#include <memory>
#include "service/network/public/cpp/resource_request.h"

namespace network {

//
// 对外使用接口，不要直接调用UrlLoader。
//
class SimpleUrlLoader {
public:
  static std::unique_ptr<SimpleUrlLoader> Create(
    std::unique_ptr<ResourceRequest> resource_request
  );
};

}

#endif