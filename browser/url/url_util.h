// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef URL_URL_UTIL_H_
#define URL_URL_UTIL_H_

#include <string>
#include "url/third_party/mozilla/url_parse.h"

namespace url {

bool Canonicalize(std::string_view spec,
                  bool trim_path_end,
                  CharsetConverter* charset_converter,
                  CanonOutput* output,
                  Parsed* output_parsed);

bool Canonicalize(std::u16string_view spec,
                  bool trim_path_end,
                  CharsetConverter* charset_converter,
                  CanonOutput* output,
                  Parsed* output_parsed);
}

#endif  // URL_URL_UTIL_H_
