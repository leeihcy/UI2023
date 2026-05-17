#ifndef URL_ORIGIN_H_
#define URL_ORIGIN_H_
#include "url/scheme_host_port.h"

class GURL;
namespace url {

class Origin {
public:
  static Origin Create(const GURL& url);
  
  bool IsSameOriginWith(const Origin &other) const;
  bool IsSameOriginWith(const GURL &url) const;

  // An ASCII serialization of the Origin as per Section 6.2 of RFC 6454, with
  // the addition that all Origins with a 'file' scheme serialize to "file://".
  std::string Serialize() const;

  bool opaque() const { return false; /*nonce_.has_value(); */ }

    // For opaque origins, these return ("", "", 0).
  const std::string scheme() const {
    return !opaque() ? tuple_.scheme() : "";
  }

  friend bool operator==(const Origin& left, const Origin& right) = default;

  // The tuple is used for both tuple origins (e.g. https://example.com:80), as
  // well as for opaque origins, where it tracks the tuple origin from which
  // the opaque origin was initially derived (we call this the "precursor"
  // origin).
  SchemeHostPort tuple_;

  // The nonce is used for maintaining identity of an opaque origin. This
  // nonce is preserved when an opaque origin is copied or moved. An Origin
  // is considered opaque if and only if |nonce_| holds a value.
  // std::optional<Nonce> nonce_;
};

}

#endif