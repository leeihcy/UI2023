
#include "net/http/http_response_headers.h"
#include "base/log.h"
#include "base/strings/string_util.h"
#include "base/strings/string_number_conversions.h"
#include "net/http/http_util.h"

#include <charconv>
#include <string_view>
#include <system_error>

namespace {
bool HasEmbeddedNulls(std::string_view str) {
  return str.find('\0') != std::string::npos;
}
} // namespace

namespace net {

HttpResponseHeaders::HttpResponseHeaders(HttpVersion version) : http_version_(version) { 

}

HttpResponseHeaders::HttpResponseHeaders(std::string_view raw_input)
    : response_code_(-1) {
  base::Assert(0);
  // Parse(raw_input);
}

std::shared_ptr<HttpResponseHeaders> HttpResponseHeaders::TryToCreate(
    std::string_view headers) {
  // Reject strings with nulls.
  if (HasEmbeddedNulls(headers) ||
      headers.size() > std::numeric_limits<int>::max()) {
    return nullptr;
  }

  return std::make_shared<HttpResponseHeaders>(
      HttpUtil::AssembleRawHeaders(headers));
}

bool HttpResponseHeaders::IsChunkEncoded() const {
  // Ignore spurious chunked responses from HTTP/1.0 servers and proxies.
  return GetHttpVersion() >= HttpVersion(1, 1) &&
         HasHeaderValue("Transfer-Encoding", "chunked");
}

std::optional<std::string>
HttpResponseHeaders::GetNormalizedHeader(std::string_view name) const {
  base::Assert(false);
  return std::optional<std::string>();
}

bool HttpResponseHeaders::HasHeaderValue(std::string_view name,
                                         std::string_view value) const {
  // The value has to be an exact match.  This is important since
  // 'cache-control: no-cache' != 'cache-control: no-cache="foo"'
  size_t iter = 0;
  std::optional<std::string_view> temp;
  while ((temp = EnumerateHeader(&iter, name))) {
    if (base::EqualsCaseInsensitiveASCII(value, *temp)) {
      return true;
    }
  }
  return false;
}


size_t HttpResponseHeaders::FindHeader(size_t from,
                                       std::string_view search) const {
  for (size_t i = from; i < parsed_.size(); ++i) {
    if (parsed_[i].is_continuation())
      continue;
    auto name = header_name(parsed_[i]);
    if (base::EqualsCaseInsensitiveASCII(search, name))
      return i;
  }

  return std::string::npos;
}

std::optional<std::string_view> HttpResponseHeaders::EnumerateHeader(
    size_t* iter,
    std::string_view name) const {
  size_t i;
  if (!iter || !*iter) {
    i = FindHeader(0, name);
  } else {
    i = *iter;
    if (i >= parsed_.size()) {
      i = std::string::npos;
    } else if (!parsed_[i].is_continuation()) {
      i = FindHeader(i, name);
    }
  }

  if (i == std::string::npos) {
    return std::nullopt;
  }

  if (iter)
    *iter = i + 1;
  return header_value(parsed_[i]);
}


std::string_view HttpResponseHeaders::header_name(
    const ParsedHeader& parsed) const {
  return subrange(parsed.name_begin, parsed.name_end);
}

std::string_view HttpResponseHeaders::header_value(
    const ParsedHeader& parsed) const {
  return subrange(parsed.value_begin, parsed.value_end);
}

std::string_view HttpResponseHeaders::subrange(size_t begin, size_t end) const {
  return std::string_view(raw_headers_).substr(begin, end - begin);
}

bool HttpResponseHeaders::EnumerateHeader(size_t* iter,
                                          std::string_view name,
                                          std::string* value) const {
  std::optional<std::string_view> result = EnumerateHeader(iter, name);
  if (!result) {
    value->clear();
    return false;
  }
  value->assign(*result);
  return true;
}

static bool StringToInt64(std::string_view str, int64_t& value) {
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    return ec == std::errc() && ptr == str.data() + str.size();
}

std::optional<int64_t> HttpResponseHeaders::GetInt64HeaderValue(
    std::string_view header) const {
  size_t iter = 0;
  std::optional<std::string_view> content_length =
      EnumerateHeader(&iter, header);
  if (!content_length || content_length->empty()) {
    return std::nullopt;
  }

  if ((*content_length)[0] == '+') {
    return std::nullopt;
  }

  int64_t result;
  bool ok = StringToInt64(*content_length, result);
  if (!ok || result < 0) {
    return std::nullopt;
  }

  return result;
}

// From RFC 2616:
// Content-Length = "Content-Length" ":" 1*DIGIT
std::optional<int64_t> HttpResponseHeaders::GetContentLength() const {
  std::optional<int64_t> result = GetInt64HeaderValue("content-length");
  if (result.has_value()) {
    return result; //base::ByteCount(result.value());
  }
  return std::nullopt;
}


// We accept the first value of "close" or "keep-alive" in a Connection or
// Proxy-Connection header, in that order. Obeying "keep-alive" in HTTP/1.1 or
// "close" in 1.0 is not strictly standards-compliant, but we'd like to
// avoid looking at the Proxy-Connection header whenever it is reasonable to do
// so.
// TODO(ricea): Measure real-world usage of the "Proxy-Connection" header,
// with a view to reducing support for it in order to make our Connection header
// handling more RFC 7230 compliant.
bool HttpResponseHeaders::IsKeepAlive() const {
  // NOTE: It is perhaps risky to assume that a Proxy-Connection header is
  // meaningful when we don't know that this response was from a proxy, but
  // Mozilla also does this, so we'll do the same.
  static const char* const kConnectionHeaders[] = {"connection",
                                                   "proxy-connection"};
  struct KeepAliveToken {
    const char* const token;
    bool keep_alive;
  };
  static const KeepAliveToken kKeepAliveTokens[] = {{"keep-alive", true},
                                                    {"close", false}};

  if (http_version_ < HttpVersion(1, 0))
    return false;

  for (const char* header : kConnectionHeaders) {
    size_t iterator = 0;
    std::optional<std::string_view> token;
    while ((token = EnumerateHeader(&iterator, header))) {
      for (const KeepAliveToken& keep_alive_token : kKeepAliveTokens) {
        if (base::EqualsCaseInsensitiveASCII(*token, keep_alive_token.token)) {
          return keep_alive_token.keep_alive;
        }
      }
    }
  }
  return http_version_ != HttpVersion(1, 0);
}

} // namespace net