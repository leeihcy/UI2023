#ifndef URL_ORIGIN_H_
#define URL_ORIGIN_H_

class GURL;
namespace url {

class Origin {
public:
  static Origin Create(const GURL& url);
  
  bool IsSameOriginWith(const Origin &other) const;
  bool IsSameOriginWith(const GURL &url) const;

  friend bool operator==(const Origin& left, const Origin& right) = default;
};
}

#endif