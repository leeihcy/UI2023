// Copyright 2011 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_EXPORT_H_
#define MOJO_EXPORT_H_

// Defines MOJO_EXPORT so that functionality implemented by the net module can
// be exported to consumers, and MOJO_EXPORT_PRIVATE that allows unit tests to
// access features not intended to be used directly by real consumers.

#if 1 //defined(COMPONENT_BUILD)
#if defined(OS_WIN)

#if defined(MOJO_IMPLEMENTATION)
#define MOJO_EXPORT __declspec(dllexport)
#define MOJO_EXPORT_PRIVATE __declspec(dllexport)
#else
#define MOJO_EXPORT __declspec(dllimport)
#define MOJO_EXPORT_PRIVATE __declspec(dllimport)
#endif  // defined(MOJO_IMPLEMENTATION)

#else  // defined(OS_WIN)
#if defined(MOJO_IMPLEMENTATION)
#define MOJO_EXPORT __attribute__((visibility("default")))
#define MOJO_EXPORT_PRIVATE __attribute__((visibility("default")))
#else
#define MOJO_EXPORT
#define MOJO_EXPORT_PRIVATE
#endif
#endif

#else  /// defined(COMPONENT_BUILD)
#define MOJO_EXPORT
#define MOJO_EXPORT_PRIVATE
#endif

#endif  // MOJO_EXPORT_H_
