#ifndef URL_GURL_H_
#define URL_GURL_H_

#include <string>

class GURL {
public:
  // Creates an empty, invalid URL.
  GURL();
  GURL(std::string_view url_string);

  bool has_scheme() const;
  std::string GetScheme() const;

  bool has_host() const;
  std::string GetHost() const;
  int GetPort() const;
  std::string GetPath() const;
  std::string GetQuery() const;
  std::string GetFragment() const;
  std::string GetUserInfo() const;

  void InitCanonical(std::string_view input_spec, bool trim_path_end);

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
  int port_ = -1;
  std::string path_;
  std::string query_;
  std::string fragment_;
  std::string user_info_;
};

#endif
