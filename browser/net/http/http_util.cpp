#include "net/http/http_util.h"

#include "base/strings/strcat.h"
#include <algorithm>
#include <cctype>
#include <string_view>


namespace net {

std::string HttpUtil::GenerateRequestLine(std::string_view method,
                                          const GURL &url) {
  static constexpr char kSuffix[] = " HTTP/1.1\r\n";
  return base::StrCat({method, " ", url.PathForRequest(), kSuffix});
}

// Find the "http" substring in a status line. This allows for
// some slop at the start. If the "http" string could not be found
// then returns std::string::npos.
// static
size_t HttpUtil::LocateStartOfStatusLine(std::span<const uint8_t> buf) {
  const size_t slop = 4;
  const size_t http_len = 4;

  if (buf.size() >= http_len) {
    size_t i_max = std::min(buf.size() - http_len, slop);
    for (size_t i = 0; i <= i_max; ++i) {
      if (http_len == 4 &&
          std::equal(buf.data() + i, buf.data() + i + http_len, "http",
                     [](unsigned char a, unsigned char b) {
                       return std::tolower(a) == std::tolower(b);
                     })) {
        return i;
      }
    }
  }
  return std::string::npos; // Not found
}


static size_t LocateEndOfHeadersHelper(std::span<const uint8_t> buf,
                                       size_t i,
                                       bool accept_empty_header_list) {
  char last_c = '\0';
  bool was_lf = false;
  if (accept_empty_header_list) {
    // Normally two line breaks signal the end of a header list. An empty header
    // list ends with a single line break at the start of the buffer.
    last_c = '\n';
    was_lf = true;
  }

  for (; i < buf.size(); ++i) {
    char c = buf[i];
    if (c == '\n') {
      if (was_lf)
        return i + 1;
      was_lf = true;
    } else if (c != '\r' || last_c != '\n') {
      was_lf = false;
    }
    last_c = c;
  }
  return std::string::npos;
}

size_t HttpUtil::LocateEndOfAdditionalHeaders(std::span<const uint8_t> buf,
                                              size_t i) {
  return LocateEndOfHeadersHelper(buf, i, true);
}

size_t HttpUtil::LocateEndOfHeaders(std::span<const uint8_t> buf, size_t i) {
  return LocateEndOfHeadersHelper(buf, i, false);
}


} // namespace net
