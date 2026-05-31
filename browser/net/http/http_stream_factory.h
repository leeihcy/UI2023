#ifndef NET_HTTP_HTTP_STREAM_FACTORY_H_
#define NET_HTTP_HTTP_STREAM_FACTORY_H_

#include "net/http/http_stream_request.h"
#include "net/socket/client_socket_handle.h"
#include "net/http/http_stream.h"
#include "net/http/http_request_info.h"
#include <memory>
#include <set>
#include "url/gurl.h"

namespace net {
class HttpNetworkSession;

class HttpStreamFactory {
public:
   class Job {
   public:
     class Delegate {
     public:
       virtual ~Delegate() = default;
       virtual void OnStreamReady(Job* job) = 0;
     };
     
     Job(Delegate* delegate, HttpNetworkSession* session, url::SchemeHostPort destination);

     void Start(/*HttpStreamRequest::StreamType stream_type*/);
     int DoInitConnection();
     void DoCreateStream();
   
     void OnIOComplete(int result);
     void OnStreamReadyCallback();

   private:
     Delegate* delegate_;
     HttpNetworkSession* m_session;

      // The host we are going to connect to, could be that of the origin or of the
      // alternative service. The scheme of this is always HTTP or HTTPS, even for
      // websockets requests. The original destination can be found in
      // `request_info_`, which is used for the purposes of encryption.
      const url::SchemeHostPort destination_;
  

     // 负责 “传输层”连接。它代表一个底层的、已建立的 TCP 或 SSL/TLS Socket 连接，
     // 是流式数据的通道，但本身不理解 HTTP 协议。
     std::unique_ptr<ClientSocketHandle> m_connection;
     // 负责 “应用层”逻辑。它依赖于 connection_ 提供的 Socket，
     // 在连接之上发送“GET /index.html HTTP/1.1”这样的 HTTP 请求，
     // 解析响应的格式，处理 chunked 编码、keep-alive 等 HTTP 协议细节。
     std::unique_ptr<HttpStream> m_stream;
   };

   class JobFactory {
   public:
      std::unique_ptr<Job> CreateJob(HttpStreamFactory::Job::Delegate* delegate, HttpNetworkSession* session, url::SchemeHostPort destination) const {
         return std::make_unique<Job>(delegate, session, std::move(destination));
      }
   };

   struct StreamRequestInfo {
      StreamRequestInfo(const HttpRequestInfo& http_request_info) : url(http_request_info.url) {

      }

      GURL url;
   };

   class JobController : public HttpStreamFactory::Job::Delegate {
   public:
      JobController(HttpNetworkSession* session, const JobFactory* job_factory, const HttpRequestInfo& http_request_info) : 
         m_session(session),
         m_job_factory(job_factory),
         request_info_(http_request_info) {}

      std::unique_ptr<HttpStreamRequest> Start();
      void DoResolveProxy();
      void DoCreateJobs();

      void OnStreamReady(Job* job) override;
   private:
      HttpNetworkSession* m_session;
      const JobFactory* m_job_factory;
      HttpStreamRequest* m_request = nullptr;

      const StreamRequestInfo request_info_;

      std::unique_ptr<Job> m_main_job;
      // std::unique_ptr<Job> alternative_job_;
      // std::unique_ptr<Job> dns_alpn_h3_job_;
   };


public:
   HttpStreamFactory(HttpNetworkSession* session);
   std::unique_ptr<HttpStreamRequest> RequestStream(const HttpRequestInfo& request_info);

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