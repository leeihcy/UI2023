#ifndef NET_HTTP_HTTP_STREAM_FACTORY_H_
#define NET_HTTP_HTTP_STREAM_FACTORY_H_

#include "net/http/http_stream_request.h"
#include "net/socket/client_socket_handle.h"
#include "net/http/http_stream.h"
#include <memory>
#include <set>

namespace net {
class HttpNetworkSession;

class HttpStreamFactory {
public:
   class Job {
   public:
     Job(HttpNetworkSession* session);

     void Start(/*HttpStreamRequest::StreamType stream_type*/);
     void DoInitConnection();
     void DoCreateStream();
   
   private:
     HttpNetworkSession* m_session;

     // 负责 “传输层”连接。它代表一个底层的、已建立的 TCP 或 SSL/TLS Socket 连接，是流式数据的通道，但本身不理解 HTTP 协议。
     std::unique_ptr<ClientSocketHandle> m_connection;
     // 负责 “应用层”逻辑。它依赖于 connection_ 提供的 Socket，在连接之上发送“GET /index.html HTTP/1.1”这样的 HTTP 请求，解析响应的格式，处理 chunked 编码、keep-alive 等 HTTP 协议细节。
     std::unique_ptr<HttpStream> m_stream;
   };

   class JobFactory {
   public:
      std::unique_ptr<Job> CreateJob(HttpNetworkSession* session) const {
         return std::make_unique<Job>(session);
      }
   };

   class JobController {
   public:
      JobController(HttpNetworkSession* session, const JobFactory* job_factory) : 
         m_session(session),
         m_job_factory(job_factory) {}

      std::unique_ptr<HttpStreamRequest> Start();
      void DoResolveProxy();
      void DoCreateJobs();

   private:
      HttpNetworkSession* m_session;
      const JobFactory* m_job_factory;
      HttpStreamRequest* m_request = nullptr;

      std::unique_ptr<Job> m_main_job;
      // std::unique_ptr<Job> alternative_job_;
      // std::unique_ptr<Job> dns_alpn_h3_job_;
   };


public:
   HttpStreamFactory(HttpNetworkSession* session);
   std::unique_ptr<HttpStreamRequest> RequestStream();

 private:
   HttpNetworkSession* m_session;
   std::unique_ptr<JobFactory> m_job_factory;

   // JobController的生命周期与URLRequest不一致。可能在URLRequest完成后，仍然存活一段时间，
   // 因此由HttpStreamFactory负责销毁。
   using JobControllerSet = std::set<std::unique_ptr<JobController>>;
   JobControllerSet job_controller_set_;
};
}

#endif