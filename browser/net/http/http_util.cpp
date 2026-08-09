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


// Helper used by AssembleRawHeaders, to skip past leading LWS.
static std::string_view RemoveLeadingNonLWS(std::string_view str) {
  for (size_t i = 0; i < str.size(); i++) {
    if (!HttpUtil::IsLWS(str[i]))
      return str.substr(i);
  }
  return std::string_view();  // Remove everything.
}

std::string HttpUtil::AssembleRawHeaders(std::string_view input) {
  std::string raw_headers;
  raw_headers.reserve(input.size());
#if 0
  // Skip any leading slop, since the consumers of this output
  // (HttpResponseHeaders) don't deal with it.
  size_t status_begin_offset =
      LocateStartOfStatusLine(base::as_byte_span(input));
  if (status_begin_offset != std::string::npos)
    input.remove_prefix(status_begin_offset);

  // Copy the status line.
  size_t status_line_end = FindStatusLineEnd(input);
  raw_headers.append(input.data(), status_line_end);
  input.remove_prefix(status_line_end);

  // After the status line, every subsequent line is a header line segment.
  // Should a segment start with LWS, it is a continuation of the previous
  // line's field-value.

  // TODO(ericroman): is this too permissive? (delimits on [\r\n]+)
  base::StringViewTokenizer lines(input, "\r\n");

  // This variable is true when the previous line was continuable.
  bool prev_line_continuable = false;

  while (lines.GetNext()) {
    std::string_view line = lines.token();

    if (prev_line_continuable && IsLWS(line[0])) {
      // Join continuation; reduce the leading LWS to a single SP.
      base::StrAppend(&raw_headers, {" ", RemoveLeadingNonLWS(line)});
    } else {
      // Terminate the previous line and copy the raw data to output.
      base::StrAppend(&raw_headers, {"\n", line});

      // Check if the current line can be continued.
      prev_line_continuable = IsLineSegmentContinuable(line);
    }
  }

  raw_headers.append("\n\n", 2);

  // Use '\0' as the canonical line terminator. If the input already contained
  // any embeded '\0' characters we will strip them first to avoid interpreting
  // them as line breaks.
  std::erase(raw_headers, '\0');

  std::replace(raw_headers.begin(), raw_headers.end(), '\n', '\0');
#else
  raw_headers = input;
#endif
  return raw_headers;
}

} // namespace net
