#ifndef URL_GURL_H_
#define URL_GURL_H_

#include <string>

namespace url {
  enum SpecialPort { PORT_UNSPECIFIED = -1, PORT_INVALID = -2 };
}

class GURL {
public:

  // Creates an empty, invalid URL.
  GURL();
  GURL(std::string_view url_string);

  bool is_valid() const;

  bool has_scheme() const;
  std::string GetScheme() const;
  std::string_view scheme() const {
    return std::string_view(scheme_);
  }

  bool has_host() const;
  std::string GetHost() const;
  std::string_view host() const {
    return std::string_view(host_);
  }

  int GetPort() const;
  
  // Returns a parsed version of the port. Can also be any of the special
  // values defined in Parsed for ExtractPort.
  int IntPort() const;

  // Returns the port number of the URL, or the default port number.
  // If the scheme has no concept of port (or unknown default) returns
  // PORT_UNSPECIFIED.
  int EffectiveIntPort() const;

  std::string GetPath() const;
  std::string GetQuery() const;
  std::string GetFragment() const;
  std::string GetUserInfo() const;

  void InitCanonical(std::string_view input_spec, bool trim_path_end);

  // Returns true if the given parameter (should be lower-case ASCII to match
  // the canonicalized scheme) is the scheme for this URL. Do not include a
  // colon.
  bool SchemeIs(std::string_view lower_ascii_scheme) const;

  // Returns true if the scheme is "http" or "https".
  bool SchemeIsHTTPOrHTTPS() const;

private:
  void ParseURL(std::string_view spec);
  bool ParseScheme(const char*& current, size_t* out_len);
  void ParseUserInfo(const char* userinfo, std::string* out_userinfo);
  void ParsePort(const char* port_str, int* out_port);
  std::string DecodeURLComponent(std::string s);

  // The actual text of the URL, in canonical ASCII form.
  std::string spec_;

  // Parsed components
  bool is_valid_;
  std::string scheme_;
  std::string host_;
  int port_ = url::PORT_UNSPECIFIED;
  std::string path_;
  std::string query_;
  std::string fragment_;
  std::string user_info_;
};

#endif
