#include "net/http/http_cache.h"

namespace  net {

std::unique_ptr<HttpTransaction> HttpCache::CreateTransaction() {

 auto new_transaction =
      std::make_unique<HttpCache::Transaction>(/*priority, this*/);

  return new_transaction;
}

}