#ifndef NET_HTTP_HTTP_STREAM_H_
#define NET_HTTP_HTTP_STREAM_H_

#include "net/http/http_stream_request.h"
#include <memory>
#include <set>

namespace net {

class HttpStreamFactory {
public:
   class JobController {
   public:
      std::unique_ptr<HttpStreamRequest> Start();
   };

public:
   std::unique_ptr<HttpStreamRequest> RequestStream();

private:
  // JobController的生命周期与URLRequest不一致。可能在URLRequest完成后，仍然存活一段时间，
  // 因此由HttpStreamFactory负责销毁。
   using JobControllerSet = std::set<std::unique_ptr<JobController>>;
   JobControllerSet job_controller_set_;
};

}

#endif