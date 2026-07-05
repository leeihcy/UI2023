#include "net/http/http_request_headers.h"

#include "base/strings/string_util.h"

namespace net {

const char HttpRequestHeaders::kGetMethod[] = "GET";



HttpRequestHeaders::HeaderKeyValuePair::HeaderKeyValuePair() = default;

HttpRequestHeaders::HeaderKeyValuePair::HeaderKeyValuePair(
    std::string_view key,
    std::string_view value)
    : HeaderKeyValuePair(key, std::string(value)) {}

HttpRequestHeaders::HeaderKeyValuePair::HeaderKeyValuePair(std::string_view key,
                                                           std::string&& value)
    : key(key), value(std::move(value)) {}

HttpRequestHeaders::Iterator::Iterator(const HttpRequestHeaders& headers)
    : curr_(headers.headers_.begin()), end_(headers.headers_.end()) {}

HttpRequestHeaders::Iterator::~Iterator() = default;

bool HttpRequestHeaders::Iterator::GetNext() {
  if (!started_) {
    started_ = true;
    return curr_ != end_;
  }

  if (curr_ == end_)
    return false;

  ++curr_;
  return curr_ != end_;
}


void HttpRequestHeaders::Clear() {
  headers_.clear();
}

void HttpRequestHeaders::SetHeader(std::string_view key,
                                   std::string_view value) {
  SetHeader(key, std::string(value));
}

void HttpRequestHeaders::SetHeader(std::string_view key, std::string&& value) {
  // Invalid header names or values could mean clients can attach
  // browser-internal headers.
  // CHECK(HttpUtil::IsValidHeaderName(key)) << key;
  // CHECK(HttpUtil::IsValidHeaderValue(value)) << key << " has invalid value.";

  SetHeaderInternal(key, std::move(value));
}

HttpRequestHeaders::HeaderVector::iterator HttpRequestHeaders::FindHeader(
    std::string_view key) {
  for (auto it = headers_.begin(); it != headers_.end(); ++it) {
    if (base::EqualsCaseInsensitiveASCII(key, it->key))
      return it;
  }

  return headers_.end();
}

HttpRequestHeaders::HeaderVector::const_iterator HttpRequestHeaders::FindHeader(
    std::string_view key) const {
  for (auto it = headers_.begin(); it != headers_.end(); ++it) {
    if (base::EqualsCaseInsensitiveASCII(key, it->key))
      return it;
  }

  return headers_.end();
}

void HttpRequestHeaders::SetHeaderInternal(std::string_view key,
                                           std::string&& value) {
  auto it = FindHeader(key);
  if (it != headers_.end())
    it->value = std::move(value);
  else
    headers_.emplace_back(key, std::move(value));
}

}