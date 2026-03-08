// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_NOTREACHED_H_
#define BASE_NOTREACHED_H_

#include <assert.h>

namespace logging {

#define NOTREACHED() abort()
#define NOTIMPLEMENTED() assert(false && "NotImplemented");

}
#endif  // BASE_NOTREACHED_H_
