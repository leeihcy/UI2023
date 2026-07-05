#ifndef NET_HTTP_HTTP_REQUEST_HEADERS_H_
#define NET_HTTP_HTTP_REQUEST_HEADERS_H_

#include <string>
#include <vector>

namespace net {

class HttpRequestHeaders {
public:
  static const char kGetMethod[];

  struct HeaderKeyValuePair {
    HeaderKeyValuePair();
    HeaderKeyValuePair(std::string_view key, std::string_view value);
    HeaderKeyValuePair(std::string_view key, std::string &&value);
    // Inline to take advantage of the std::string_view constructor being
    // constexpr.
    HeaderKeyValuePair(std::string_view key, const char *value)
        : HeaderKeyValuePair(key, std::string_view(value)) {}

    bool operator==(const HeaderKeyValuePair &other) const = default;

    std::string key;
    std::string value;
  };
  typedef std::vector<HeaderKeyValuePair> HeaderVector;

  class Iterator {
   public:
    explicit Iterator(const HttpRequestHeaders& headers);

    Iterator(const Iterator&) = delete;
    Iterator& operator=(const Iterator&) = delete;

    ~Iterator();

    // Advances the iterator to the next header, if any.  Returns true if there
    // is a next header.  Use name() and value() methods to access the resultant
    // header name and value.
    bool GetNext();

    // These two accessors are only valid if GetNext() returned true.
    const std::string& name() const { return curr_->key; }
    const std::string& value() const { return curr_->value; }

   private:
    bool started_ = false;
    HttpRequestHeaders::HeaderVector::const_iterator curr_;
    const HttpRequestHeaders::HeaderVector::const_iterator end_;
  };

  // Clears all the headers.
  void Clear();

  void SetHeader(std::string_view key, std::string_view value);
  void SetHeader(std::string_view key, std::string&& value);
  // Inline to take advantage of the std::string_view constructor being
  // constexpr.
  void SetHeader(std::string_view key, const char* value) {
    SetHeader(key, std::string_view(value));
  }

private:
  HeaderVector::iterator FindHeader(std::string_view key);
  HeaderVector::const_iterator FindHeader(std::string_view key) const;

  void SetHeaderInternal(std::string_view key, std::string&& value);

private:
  HeaderVector headers_;
};

}
#endif