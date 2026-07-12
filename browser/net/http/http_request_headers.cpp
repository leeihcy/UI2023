#include "net/http/http_request_headers.h"

#include "base/strings/string_util.h"
#include "base/strings/strcat.h"

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


std::string HttpRequestHeaders::ToString() const {
  static constexpr std::string_view kColon = ": ";
  static constexpr std::string_view kCrNl = "\r\n";

  // As of January 2024, 99% of of HttpRequestHeaders objects had 27 headers or
  // less. Allow space for 128 string pieces without heap allocation as it is a
  // nice round number.
  std::vector<std::string_view> pieces;
  const size_t expected_size = headers_.size() * 4 + 1;

  pieces.reserve(expected_size);
  for (const auto& header : headers_) {
    pieces.insert(pieces.end(), {header.key, kColon, header.value, kCrNl});
  }
  pieces.push_back(kCrNl);
  return base::StrCat(pieces);
}

}