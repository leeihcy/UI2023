#include "net/url_request/url_request_job.h"

#include <assert.h>
#include "net/url_request/url_request.h"
#include "net/base/net_errors.h"

namespace net {

URLRequestJob::URLRequestJob(URLRequest* request) : request_(request) {}

int URLRequestJob::NotifyConnected(const TransportInfo& info/*,
                      CompletionOnceCallback callback*/) {
  return request_->NotifyConnected(info/*, std::move(callback)*/);
}

void URLRequestJob::NotifyHeadersComplete() {
  NotifyFinalHeadersReceived();
  // |this| may be destroyed at this point.
}

void URLRequestJob::NotifyFinalHeadersReceived() {
  request_->NotifyResponseStarted(net::OK);
  // |this| may be destroyed at this point.
}

int URLRequestJob::Read(IOBuffer* buf, int buf_size) {
  // pending_read_buffer_ = buf;

  // int result = source_stream_->Read(
  //     buf, buf_size,
  //     base::BindOnce(&URLRequestJob::SourceStreamReadComplete,
  //                    weak_factory_.GetWeakPtr(), false));
  int result = ReadRawData(buf, buf_size);
  if (result == ERR_IO_PENDING)
    return ERR_IO_PENDING;

  SourceStreamReadComplete(true, result);
  return result;
}

void URLRequestJob::ReadRawDataComplete(int result) {
  SourceStreamReadComplete(false, result);
}

void URLRequestJob::SourceStreamReadComplete(bool synchronous, int result) {
  OnDone(OK, false /* notify_done */);
}

void URLRequestJob::OnDone(int net_error, bool notify_done) {
  NotifyDone();
  assert(false);
}

void URLRequestJob::NotifyDone() {

}

} // namespace net