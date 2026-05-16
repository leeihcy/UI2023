#include "url/origin.h"
#include "url/gurl.h"
#include <assert.h>

namespace url {

  
Origin Origin::Create(const GURL& url) {
  if (!url.is_valid())
    return Origin();

  // SchemeHostPort tuple;

  // if (url.SchemeIsFileSystem()) {
  //   tuple = SchemeHostPort(*url.inner_url());
  // } else if (url.SchemeIsBlob()) {
  //   tuple = SchemeHostPort(GURL(url.GetContent()));
  // } else {
  //   tuple = SchemeHostPort(url);
  // }

  // if (!tuple.IsValid())
  //   return Origin();
  // return Origin(std::move(tuple));
  assert(false);
  return Origin();
}


bool Origin::IsSameOriginWith(const Origin &other) const {
  return *this == other;
}

bool Origin::IsSameOriginWith(const GURL &url) const {
  return IsSameOriginWith(url::Origin::Create(url));
}

}