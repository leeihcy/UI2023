// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_COMPLETION_ONCE_CALLBACK_H_
#define NET_BASE_COMPLETION_ONCE_CALLBACK_H_

#include <stdint.h>
#include <functional>

namespace net {

using CompletionOnceCallback = std::function<void(int)>;
using Int64CompletionOnceCallback = std::function<void(int64_t)>;

}  // namespace net

#endif  // NET_BASE_COMPLETION_ONCE_CALLBACK_H_