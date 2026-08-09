#ifndef NET_HTTP_HTTP_RESPONSE_HEADERS_H_
#define NET_HTTP_HTTP_RESPONSE_HEADERS_H_

#include <string>
#include <optional>
#include <functional>
#include <memory>
#include <vector>
#include "net/http/http_version.h"

namespace net {
  

class HttpResponseHeaders {
public:
  // The members of this structure point into raw_headers_.
  struct ParsedHeader {
    // A header "continuation" contains only a subsequent value for the
    // preceding header.  (Header values are comma separated.)
    bool is_continuation() const { return name_begin == name_end; }

    // Provided for use in HttpResponseHeaders::StrictlyEquals(), which is only
    // used in testing.
    bool operator==(const ParsedHeader &) const = default;

    size_t name_begin;
    size_t name_end;
    size_t value_begin;
    size_t value_end;
  };

  typedef std::vector<ParsedHeader> HeaderList;

public:
  HttpResponseHeaders(HttpVersion version);
  explicit HttpResponseHeaders(std::string_view raw_headers);

  static std::shared_ptr<HttpResponseHeaders> TryToCreate(
      std::string_view headers);

  std::optional<std::string_view> EnumerateHeader(size_t* iter,
                                                  std::string_view name) const;
  size_t FindHeader(size_t from, std::string_view name) const;
  bool EnumerateHeader(size_t *iter, std::string_view name,
                       std::string *value) const;
  bool HasHeaderValue(std::string_view name, std::string_view value) const;
  
  // Returns the name/value using `raw_headers_` and indices from `parsed`.
  std::string_view header_name(const ParsedHeader& parsed) const;
  std::string_view header_value(const ParsedHeader& parsed) const;

  // Returns true if the response is chunk-encoded.
  bool IsChunkEncoded() const;
  bool IsKeepAlive() const;

  const std::string& raw_headers() const { return raw_headers_; }
  std::string_view subrange(size_t begin, size_t end) const;

  // Returns the value of the Content-Length header or nullopt if there is no
  // such header in the response.
  std::optional<int64_t> GetContentLength() const;
  std::optional<int64_t> GetInt64HeaderValue(std::string_view header) const;

  // Get the HTTP version of the normalized status line.
  HttpVersion GetHttpVersion() const { return http_version_; }

  int response_code() const { return response_code_; }

  [[nodiscard]] std::optional<std::string> GetNormalizedHeader(
      std::string_view name) const;

private:
  HeaderList parsed_;
  
  std::string raw_headers_;

  int response_code_ = -1;
  HttpVersion http_version_;
};

using ResponseHeadersCallback =
    std::function<void(const HttpResponseHeaders*)>;

}

#endif