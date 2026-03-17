// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef URL_URL_UTIL_H_
#define URL_URL_UTIL_H_

#include <string>
#include "url/url_canon.h"
#include "url/url_constants.h"
#include "url/third_party/mozilla/url_parse.h"

namespace url {

  // Returns true if the given scheme identified by |scheme| within |spec| is in
// the list of known standard-format schemes (see AddStandardScheme).
// TODO(crbug.com/351564777): Delete this after //third_party/openscreen
// transition is complete.
bool IsStandard(const char* spec, const Component& scheme);
bool IsStandard(std::optional<std::string_view> scheme);
bool IsStandard(std::optional<std::u16string_view> scheme);

bool IsStandardScheme(std::string_view scheme);


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
