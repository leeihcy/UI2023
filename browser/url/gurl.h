#ifndef URL_GURL_H_
#define URL_GURL_H_

#include <string>
#include "url/third_party/mozilla/url_parse.h"

class GURL {
public:
  // Creates an empty, invalid URL.
  GURL();
  GURL(std::string_view url_string);

 template <typename T, typename CharT = typename T::value_type>
  void InitCanonical(T input_spec, bool trim_path_end);

private:
  // The actual text of the URL, in canonical ASCII form.
  std::string spec_;

  // Set when the given URL is valid. Otherwise, we may still have a spec and
  // components, but they may not identify valid resources (for example, an
  // invalid port number, invalid characters in the scheme, etc.).
  bool is_valid_;

  // Identified components of the canonical spec.
  url::Parsed parsed_;

  // Used for nested schemes [currently only filesystem:].
  // std::unique_ptr<GURL> inner_url_;
};

#endif