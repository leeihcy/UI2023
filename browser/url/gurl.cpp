#include "url/gurl.h"
#include <cctype>
#include <charconv>
#include <cstring>
#include "url/url_constants.h"

GURL::GURL() : is_valid_(false), port_(-1), host_(), user_info_() {}

GURL::GURL(std::string_view url_string) {
  InitCanonical(url_string, true);
}

void GURL::InitCanonical(std::string_view input_spec, bool trim_path_end) {
  spec_ = std::string(input_spec);
  is_valid_ = !spec_.empty();

  if (is_valid_) {
    ParseURL(spec_);
  }
}

bool GURL::is_valid() const { return is_valid_; }

bool GURL::has_scheme() const {
  return is_valid_ && !scheme_.empty();
}

std::string GURL::GetScheme() const {
  return scheme_;
}

bool GURL::has_host() const {
  return is_valid_ && !host_.empty();
}

std::string GURL::GetHost() const {
  return host_;
}

int GURL::GetPort() const {
  return port_;
}

int GURL::IntPort() const {
  if (port_ > 65535 || port_ < url::PORT_INVALID)
    return url::PORT_INVALID; // Out of range.
  
  return port_;
}

static int DefaultPortForScheme(std::string_view scheme) {
  switch (scheme.length()) {
    case 4:
      if (scheme == url::kHttpScheme) {
        return 80;
      }
      break;
    case 5:
      if (scheme == url::kHttpsScheme) {
        return 443;
      }
      break;
    case 3:
      if (scheme == url::kFtpScheme) {
        return 21;
      } else if (scheme == url::kWssScheme) {
        return 443;
      }
      break;
    case 2:
      if (scheme == url::kWsScheme) {
        return 80;
      }
      break;
  }
  return url::PORT_UNSPECIFIED;
}

int GURL::EffectiveIntPort() const {
  int int_port = IntPort();
  if (int_port == url::PORT_UNSPECIFIED /*&& IsStandard()*/)
    return DefaultPortForScheme(scheme());
  return int_port;
}

std::string GURL::GetPath() const {
  return path_;
}

std::string GURL::GetQuery() const {
  return query_;
}

std::string GURL::GetFragment() const {
  return fragment_;
}

std::string GURL::GetUserInfo() const {
  return user_info_;
}

static void SkipWhitespace(const char*& it, size_t* len) {
  while (*it != '\0' && std::isspace(static_cast<unsigned char>(*it))) {
    ++it;
  }
  *len = strlen(it);
}

static bool IsSchemeChar(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) || c == '+' || c == '-' || c == '.';
}

static bool IsHostChar(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) || c == '.' || c == '-' || c == '_' || c == '~';
}

static bool IsPathChar(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' ||
         c == '~' || c == '/' || c == ':' || c == '@' || c == '!' || c == '$' || c == '&' ||
         c == '\'' || c == '(' || c == ')' || c == '*' || c == '+' || c == ',' || c == ';' ||
         c == '=' || c == '?' || c == '%' || c == '~';
}

static char PercentDecode(char hex1, char hex2) {
  auto FromHex = [](char h) -> int {
    if (h >= '0' && h <= '9') return h - '0';
    if (h >= 'A' && h <= 'F') return h - 'A' + 10;
    if (h >= 'a' && h <= 'f') return h - 'a' + 10;
    return 0;
  };
  return (FromHex(hex1) << 4) | FromHex(hex2);
}

std::string GURL::DecodeURLComponent(std::string s) {
  std::string result;
  size_t i = 0;

  while (i < s.length()) {
    if (s[i] == '%' && i + 2 < s.length()) {
      char hex1 = s[i + 1];
      char hex2 = s[i + 2];

      if (std::isxdigit(static_cast<unsigned char>(hex1)) &&
          std::isxdigit(static_cast<unsigned char>(hex2))) {
        result += PercentDecode(hex1, hex2);
        i += 3;
        continue;
      }
    }

    if (s[i] == '+') {
      result += ' ';
      i++;
      continue;
    }

    result += s[i];
    i++;
  }

  return result;
}

void GURL::ParseURL(std::string_view spec) {
  size_t len = spec.length();
  const char* current = spec.data();

  // Skip leading whitespace
  const char* url_start = current;
  SkipWhitespace(url_start, &len);
  if (len == 0) {
    return;
  }

  // Parse scheme. ParseScheme advances url_start past the scheme chars,
  // so it will point at the ':' (if present) after the call.
  const char* scheme_start = url_start;
  size_t scheme_len = 0;
  if (ParseScheme(url_start, &scheme_len)) {
    scheme_ = std::string(scheme_start, scheme_len);
  }

  // After ParseScheme, url_start points right after the scheme characters.
  // Check for ://, :/, or : at url_start[0].
  bool has_authority = false;
  if (scheme_len > 0 && url_start[0] == ':') {
    if (url_start[1] == '/' && url_start[2] == '/') {
      // scheme://authority/path...
      has_authority = true;
      url_start += 3;
      len -= scheme_len + 3;
    } else if (url_start[1] == '/') {
      // scheme:/path... (rare)
      url_start += 2;
      len -= scheme_len + 2;
    } else {
      // scheme:path... (e.g. mailto:)
      url_start += 1;
      len -= scheme_len + 1;
    }
  } else if (scheme_len == 0) {
    // No scheme found, treat as relative URL or path
    url_start = spec.data();
    len = spec.length();
  }
  // else: scheme found but no colon after it — unusual, keep url_start as-is

  current = url_start;

  // Skip whitespace after scheme separator
  SkipWhitespace(current, &len);

  // Parse authority [userinfo@]host[:port]
  if (has_authority) {
    // current points to the start of the authority (host or userinfo@host).
    // Find the end of the authority segment (delimited by /, ?, #, or end).
    const char* auth_end = current;
    while (auth_end < current + len && *auth_end != '/' && *auth_end != '?' && *auth_end != '#') {
      auth_end++;
    }
    size_t auth_len = auth_end - current;

    // Look for @ to separate userinfo from host
    const char* at_sign = current;
    while (at_sign < current + auth_len && *at_sign != '@') {
      at_sign++;
    }

    if (at_sign < current + auth_len) {
      // Has userinfo: "user:pass@host"
      std::string userinfo_str(current, at_sign - current);
      ParseUserInfo(userinfo_str.c_str(), &user_info_);
      current = at_sign + 1;
      auth_len = auth_end - current;
    }

    // Now current..auth_end is host[:port]
    if (auth_len > 0 && current[0] == '[') {
      // IPv6 literal: [::1] or [::1]:port
      const char* bracket_end = current + 1;
      while (bracket_end < auth_end && *bracket_end != ']') {
        bracket_end++;
      }
      if (bracket_end < auth_end) {
        host_ = std::string(current, bracket_end - current + 1);
        if (bracket_end + 1 < auth_end && *(bracket_end + 1) == ':') {
          ParsePort(bracket_end + 2, &port_);
        }
      }
    } else {
      // Regular host or host:port
      const char* colon = current;
      while (colon < auth_end && *colon != ':') {
        colon++;
      }
      if (colon < auth_end) {
        host_ = std::string(current, colon - current);
        ParsePort(colon + 1, &port_);
      } else {
        host_ = std::string(current, auth_len);
      }
    }

    // Advance past the authority
    len -= auth_end - current;
    current = auth_end;
  } else if (len > 0 && current[0] == '@') {
    // Implicit userinfo@host (no scheme://)
    size_t at_pos = 1;
    while (at_pos < len && current[at_pos] != '/') {
      at_pos++;
    }
    ParseUserInfo(current + 1, &user_info_);
    current += at_pos;
    len -= at_pos;
  } else if (len > 0 && current[0] == '/') {
    // Path starts with /
    current++;
    len--;
  } else if (len > 0 && current[0] == '?') {
    // Query only
    current++;
    len--;
  } else if (len > 0 && current[0] == '#') {
    // Fragment only
    current++;
    len--;
  }

  // Parse path — scan forward to the first ? or #
  if (len > 0 && current[0] != '?' && current[0] != '#') {
    size_t path_end = 0;
    while (path_end < len && current[path_end] != '?' && current[path_end] != '#') {
      path_end++;
    }

    std::string path_str(current, path_end);
    path_ = DecodeURLComponent(path_str);
    if (path_.empty()) {
      path_ = "/";
    }
    current += path_end;
    len -= path_end;
  }

  // Parse query — from ? up to # or end
  if (len > 0 && current[0] == '?') {
    current++;
    len--;
    size_t query_end = 0;
    while (query_end < len && current[query_end] != '#') {
      query_end++;
    }
    query_ = std::string(current, query_end);
    current += query_end;
    len -= query_end;
  }

  // Parse fragment
  if (len > 0 && current[0] == '#') {
    current++;
    len--;
    fragment_ = DecodeURLComponent(std::string(current, len));
    len = 0;
  }

  is_valid_ = true;
}

bool GURL::ParseScheme(const char*& current, size_t* out_len) {
  // Skip leading + (for relative URLs like +path)
  if (*current == '+') {
    return false;
  }

  // Scheme must start with letter
  if (*current == '\0' || !std::isalpha(static_cast<unsigned char>(*current))) {
    return false;
  }

  const char* start = current;
  while (*current != '\0' && IsSchemeChar(*current)) {
    current++;
  }

  size_t len = current - start;
  if (len == 0) {
    return false;
  }

  *out_len = len;
  return true;
}

void GURL::ParseUserInfo(const char* userinfo, std::string* out_userinfo) {
  size_t len = strlen(userinfo);
  size_t colon_pos = 0;
  while (colon_pos < len && userinfo[colon_pos] != ':') {
    colon_pos++;
  }

  std::string user, pass;

  if (colon_pos < len) {
    user = std::string(userinfo, colon_pos);
    std::string pass_str(userinfo + colon_pos + 1);
    if (!pass_str.empty() && pass_str[0] != '@') {
      pass = pass_str;
    }
  } else {
    user = std::string(userinfo);
  }

  user = DecodeURLComponent(user);
  pass = DecodeURLComponent(pass);

  *out_userinfo = user;
  if (!pass.empty()) {
    *out_userinfo = user + ":" + pass;
  }
}

void GURL::ParsePort(const char* port_str, int* out_port) {
  if (port_str == nullptr) {
    return;
  }

  std::string port_str_s(port_str);
  if (port_str_s.empty()) {
    return;
  }

  int port = 0;
  auto res = std::from_chars(port_str_s.data(), port_str_s.data() + port_str_s.size(), port);
  if (res.ec == std::errc{} && port >= 1 && port <= 65535) {
    *out_port = port;
  }
}

