
#include "net/http/http_response_headers.h"
#include "base/log.h"
#include "base/strings/string_util.h"
#include "base/strings/string_number_conversions.h"
#include "net/http/http_util.h"
#include "net/http/http_status_code.h"

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
  Parse(raw_input);
}

void HttpResponseHeaders::Parse(std::string_view raw_input) {
  raw_headers_.reserve(raw_input.size());

  size_t line_end = raw_input.find('\0');

  bool has_headers =
      (line_end != std::string::npos && (line_end + 1) != raw_input.size() &&
       raw_input[line_end + 1] != '\0');
  ParseStatusLine(raw_input.substr(0, line_end), has_headers);
  raw_headers_.push_back('\0');  // Terminate status line with a null.

   if (line_end == std::string::npos) {
    raw_headers_.push_back('\0');  // Ensure the headers end with a double null.
    return;
  }
}

static bool startsWithIgnoreCase(const std::string_view& str, const std::string_view& prefix) {
    if (str.size() < prefix.size()) return false;
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(str[i])) != 
            std::tolower(static_cast<unsigned char>(prefix[i]))) {
            return false;
        }
    }
    return true;
}

namespace {
  
bool StringToInt64(std::string_view str, int64_t& value) {
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    return ec == std::errc() && ptr == str.data() + str.size();
}
bool StringToInt(std::string_view str, int& value) {
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    return ec == std::errc() && ptr == str.data() + str.size();
}

void RemoveLeadingSpaces(std::string_view* s) {
  s->remove_prefix(std::min(s->find_first_not_of(' '), s->size()));
}
int ParseStatus(std::string_view status, std::string& append_to) {
  // Skip whitespace. Tabs are not skipped, for backwards compatibility.
  RemoveLeadingSpaces(&status);

  auto first_non_digit = std::ranges::find_if(
      status, [](char c) { return !base::IsAsciiDigit(c); });

  if (first_non_digit == status.begin()) {
    append_to.append(" 200");
    return HTTP_OK;
  }

  append_to.push_back(' ');
  append_to.append(status.begin(), first_non_digit);
  int response_code = -1;
  // For backwards compatibility, overlarge response codes are permitted.
  // base::StringToInt will clamp the value to INT_MAX.
  StringToInt(std::string_view(status.begin(), first_non_digit),
                    response_code);

  status.remove_prefix(first_non_digit - status.begin());

  // Skip whitespace. Tabs are not skipped, as before.
  RemoveLeadingSpaces(&status);

  // Trim trailing whitespace. Tabs are not trimmed.
  const size_t last_non_space_pos = status.find_last_not_of(' ');
  if (last_non_space_pos != std::string_view::npos) {
    status.remove_suffix(status.size() - last_non_space_pos - 1);
  }

  if (status.empty()) {
    return response_code;
  }

  append_to.push_back(' ');
  append_to.append(status);
  return response_code;
}

}
HttpVersion HttpResponseHeaders::ParseVersion(std::string_view line) {
  size_t p = 0;

  // RFC9112 Section 2.3:
  // HTTP-version  = HTTP-name "/" DIGIT "." DIGIT
  // HTTP-name     = %s"HTTP"

  if (!startsWithIgnoreCase(line, "http")) {
    return HttpVersion();
  }

  p += 4;

  if (p >= line.size() || line[p] != '/') {
    return HttpVersion();
  }

  size_t dot = line.find('.', p);
  if (dot == std::string_view::npos || dot + 1 == line.size()) {
    return HttpVersion();
  }

  ++p;  // from / to first digit.
  ++dot;  // from . to second digit.

  if (!(base::IsAsciiDigit(line[p]) && base::IsAsciiDigit(line[dot]))) {
    return HttpVersion();
  }

  uint16_t major = line[p] - '0';
  uint16_t minor = line[dot] - '0';

  return HttpVersion(major, minor);
}

// Note: this implementation implicitly assumes that line_end points at a valid
// sentinel character (such as '\0').
void HttpResponseHeaders::ParseStatusLine(std::string_view line,
                                          bool has_headers) {
  // Extract the version number
  HttpVersion parsed_http_version = ParseVersion(line);

  // Clamp the version number to one of: {0.9, 1.0, 1.1, 2.0}
  if (parsed_http_version == HttpVersion(0, 9) && !has_headers) {
    http_version_ = HttpVersion(0, 9);
    raw_headers_ = "HTTP/0.9";
  } else if (parsed_http_version == HttpVersion(2, 0)) {
    http_version_ = HttpVersion(2, 0);
    raw_headers_ = "HTTP/2.0";
  } else if (parsed_http_version >= HttpVersion(1, 1)) {
    http_version_ = HttpVersion(1, 1);
    raw_headers_ = "HTTP/1.1";
  } else {
    // Treat everything else like HTTP 1.0
    http_version_ = HttpVersion(1, 0);
    raw_headers_ = "HTTP/1.0";
  }
  if (parsed_http_version != http_version_) {
  }

  // TODO(eroman): this doesn't make sense if ParseVersion failed.
  size_t space = line.find(' ');

  if (space == std::string_view::npos) {
    raw_headers_.append(" 200 OK");
    response_code_ = HTTP_OK;
    return;
  }

  response_code_ = ParseStatus(line.substr(space + 1), raw_headers_);
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