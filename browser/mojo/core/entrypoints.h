// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_CORE_ENTRYPOINTS_H_
#define MOJO_CORE_ENTRYPOINTS_H_


namespace mojo {
namespace core {

void InitializeCore();
void ShutDownCore();
// const MojoSystemThunks2& GetSystemThunks();

}  // namespace core
}  // namespace mojo

#endif  // MOJO_CORE_ENTRYPOINTS_H_
