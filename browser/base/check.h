// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CHECK_H_
#define BASE_CHECK_H_

#include <assert.h>

#define CHECK(cond, ...) assert(cond)
#define DCHECK(condition) assert(condition)
#define DPCHECK(condition) assert(condition)

#endif  // BASE_CHECK_H_
