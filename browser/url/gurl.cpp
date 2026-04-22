#include "url/gurl.h"
#include <cctype>
#include <cstring>

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
  SkipWhitespace(current, &len);
  if (len == 0) {
    return;
  }

  // Parse scheme
  size_t scheme_len = 0;
  if (ParseScheme(current, &scheme_len)) {
    scheme_ = std::string(spec.data(), scheme_len);
  }

  // Check for // after scheme (authority indicator)
  bool has_authority = false;
  if (scheme_len >= 2 && current[scheme_len - 1] == ':' && current[scheme_len] == '/' && current[scheme_len + 1] == '/') {
    has_authority = true;
    current += scheme_len + 2;
    len -= scheme_len + 2;
  } else if (scheme_len >= 1 && current[scheme_len - 1] == ':' && current[scheme_len] == '/') {
    // Single / after scheme (e.g., file://)
    has_authority = false;
    current += scheme_len;
    len -= scheme_len;
  } else if (scheme_len > 0 && current[scheme_len - 1] == ':') {
    // No // after scheme, treat rest as path
    has_authority = false;
    current += scheme_len;
    len -= scheme_len;
  } else {
    // No scheme found
    has_authority = false;
    current = spec.data();
    len = spec.length();
  }

  // Skip whitespace after scheme
  SkipWhitespace(current, &len);

  // Parse authority [userinfo@]host[:port]
  if (has_authority && len >= 2 && current[0] == '/' && current[1] == '/') {
    const char* auth_start = current + 2;
    size_t auth_len = len - 2;
    const char* auth_end = current + len;

    // Parse userinfo@host[:port]
    size_t at_pos = auth_end - auth_start;
    size_t at_found = 0;
    while (at_found < at_pos && auth_start[at_found] != '@') {
      at_found++;
    }

    if (at_found < at_pos) {
      // Has userinfo
      ParseUserInfo(auth_start, &user_info_);
      current = auth_start + at_found + 1;
      len -= at_found + 1;
    }

    // Find port
    size_t colon_pos = 0;
    while (colon_pos < len && current[colon_pos] != ':') {
      colon_pos++;
    }

    if (colon_pos < len && current[colon_pos] == ':') {
      ParsePort(current + colon_pos + 1, &port_);
    } else {
      host_ = std::string(current, len);
    }
    current += len;
    len = 0;
  } else if (len > 0 && current[0] == '@') {
    // Implicit userinfo@host
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

  // Parse path
  if (len > 0 && current[0] != '?' && current[0] != '#') {
    size_t path_end = len;
    while (path_end > 0 && (current[path_end - 1] == '?' || current[path_end - 1] == '#')) {
      path_end--;
    }

    if (path_end > 0) {
      std::string path_str(current, path_end);
      path_ = DecodeURLComponent(path_str);
      if (path_.empty() || path_ == "?") {
        path_ = "/";
      }
      current += path_end;
      len -= path_end;
    }
  }

  // Parse query
  if (len > 0 && current[0] == '?') {
    size_t query_len = len - 1;
    query_ = std::string(current + 1, query_len);
    current += len;
    len = 0;
  }

  // Parse fragment
  if (len > 0 && current[0] == '#') {
    size_t frag_len = len - 1;
    fragment_ = DecodeURLComponent(std::string(current + 1, frag_len));
    current += len;
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

  user_info_ = user;
  if (!pass.empty()) {
    user_info_ = user + ":" + pass;
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

  try {
    int port = std::stoi(port_str_s);
    if (port >= 1 && port <= 65535) {
      *out_port = port;
    }
  } catch (...) {
    return;
  }
}

