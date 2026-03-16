#include "url/gurl.h"
#include "url/url_canon_stdstring.h"

GURL::GURL() : is_valid_(false) {}


GURL::GURL(std::string_view url_string) {
  InitCanonical(url_string, true);
}


template <typename T, typename CharT>
void GURL::InitCanonical(T input_spec, bool trim_path_end) {
  url::StdStringCanonOutput output(&spec_);
  is_valid_ =
      url::Canonicalize(input_spec, trim_path_end, nullptr, &output, &parsed_);

  output.Complete();  // Must be done before using string.
  // if (is_valid_ && SchemeIsFileSystem()) {
  //   inner_url_ =
  //       std::make_unique<GURL>(ParsedSpecView(), *parsed_.inner_parsed(), true);
  // }
  // Valid URLs always have non-empty specs.
  DCHECK(!is_valid_ || !spec_.empty());
}