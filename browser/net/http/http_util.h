#ifndef NET_HTTP_HTTP_UTIL_H_
#define NET_HTTP_HTTP_UTIL_H_

#include <string>
#include <span>
#include "url/gurl.h"

// This is a macro to support extending this string literal at compile time.
// Please excuse me polluting your global namespace!
#define HTTP_LWS " \t"

namespace net {

class HttpResponseHeaders;

class HttpUtil {
 public:
  // Generates a request line that is used for text-based HTTP messaging.
  static std::string GenerateRequestLine(std::string_view method,
                                         const GURL& url);

  // Returns the start of the status line, or std::string::npos if no status
  // line was found. This allows for 4 bytes of junk to precede the status line
  // (which is what Mozilla does too).
  static size_t LocateStartOfStatusLine(std::span<const uint8_t> buf);

  // Returns index beyond the end-of-headers marker or std::string::npos if not
  // found.  RFC 2616 defines the end-of-headers marker as a double CRLF;
  // however, some servers only send back LFs (e.g., Unix-based CGI scripts
  // written using the ASIS Apache module).  This function therefore accepts the
  // pattern LF[CR]LF as end-of-headers (just like Mozilla). The first line of
  // |buf| is considered the status line, even if empty. The parameter |i| is
  // the offset within |buf| to begin searching from.
  static size_t LocateEndOfHeaders(std::span<const uint8_t> buf, size_t i = 0);

  // Same as |LocateEndOfHeaders|, but does not expect a status line, so can be
  // used on multi-part responses or HTTP/1.x trailers.  As a result, if |buf|
  // starts with a single [CR]LF,  it is considered an empty header list, as
  // opposed to an empty status line above a header list.
  static size_t LocateEndOfAdditionalHeaders(std::span<const uint8_t> buf,
                                             size_t i = 0);

  inline static bool IsLWS(char c) {
    constexpr std::string_view kWhiteSpaceCharacters(HTTP_LWS);
    // Clang performs this optimization automatically at -O3, but Android is
    // compiled at -Oz, so we need to do it by hand.
    static_assert(kWhiteSpaceCharacters == " \t");
    return c == ' ' || c == '\t';
  }
  static std::string AssembleRawHeaders(std::string_view buf);
};
}

#endif
