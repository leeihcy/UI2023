#ifndef NET_URL_REQUEST_URL_REQUEST_JOB_FACTORY_H_
#define NET_URL_REQUEST_URL_REQUEST_JOB_FACTORY_H_

#include <memory>
#include <map>
#include <string>
#include "net/url_request/url_request_job.h"

class GURL;
namespace net {
class URLRequestJob;
class URLRequest;

class ProtocolHandler {
public:
  virtual ~ProtocolHandler();

  // Creates a URLRequestJob for the particular protocol. Never returns
  // nullptr.
  virtual std::unique_ptr<URLRequestJob>
  CreateJob(URLRequest *request) const = 0;

  // Indicates if it should be safe to redirect to |location|. Should handle
  // protocols handled by MaybeCreateJob().
  virtual bool IsSafeRedirectTarget(const GURL &location) const;
};

class URLRequestJobFactory {
public:
  URLRequestJobFactory();
  bool SetProtocolHandler(const std::string& scheme, std::unique_ptr<ProtocolHandler> protocol_handler);
  std::unique_ptr<URLRequestJob> CreateJob(URLRequest* request) const;

private:
  std::map<std::string, std::unique_ptr<ProtocolHandler>> m_protocol_handler_map;
};

}


#endif