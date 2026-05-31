#include "net/http/http_basic_stream.h"

namespace net {

HttpBasicState::HttpBasicState(std::unique_ptr<StreamSocketHandle> connection) : connection_(std::move(connection)) {

}

HttpBasicStream::HttpBasicStream(std::unique_ptr<StreamSocketHandle> connection) : state_(std::move(connection)) {

}
  
}