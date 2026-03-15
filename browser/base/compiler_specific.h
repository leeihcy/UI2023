// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_COMPILER_SPECIFIC_H_
#define BASE_COMPILER_SPECIFIC_H_

#define UNSAFE_BUFFER_USAGE

#if __has_cpp_attribute(msvc::no_unique_address)
#define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
#define NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
#define NO_UNIQUE_ADDRESS
#endif

#if __has_cpp_attribute(clang::lifetimebound)
#define LIFETIME_BOUND [[clang::lifetimebound]]
#else
#define LIFETIME_BOUND
#endif

#if __has_cpp_attribute(gsl::Owner)
#define GSL_OWNER [[gsl::Owner]]
#else
#define GSL_OWNER
#endif

#if __has_cpp_attribute(clang::reinitializes)
#define REINITIALIZES_AFTER_MOVE [[clang::reinitializes]]
#else
#define REINITIALIZES_AFTER_MOVE
#endif

#if __has_cpp_attribute(gsl::Pointer)
#define GSL_POINTER [[gsl::Pointer]]
#else
#define GSL_POINTER
#endif

#if __has_cpp_attribute(clang::trivial_abi)
#define TRIVIAL_ABI [[clang::trivial_abi]]
#else
#define TRIVIAL_ABI
#endif

#if defined(NDEBUG)
#if __has_cpp_attribute(clang::always_inline)
#define ALWAYS_INLINE [[clang::always_inline]] inline
#elif __has_cpp_attribute(gnu::always_inline)
#define ALWAYS_INLINE [[gnu::always_inline]] inline
#elif defined(COMPILER_MSVC)
#define ALWAYS_INLINE __forceinline
#endif
#endif
#if !defined(ALWAYS_INLINE)
#define ALWAYS_INLINE inline
#endif


#if defined(__clang_analyzer__)
inline constexpr bool AnalyzerNoReturn()
#if HAS_ATTRIBUTE(analyzer_noreturn)
    __attribute__((analyzer_noreturn))
#endif
{
  return false;
}
#define ANALYZER_SKIP_THIS_PATH() static_cast<void>(::AnalyzerNoReturn())
#else
// The above definition would be safe even outside the analyzer, but defining
// the macro away entirely avoids the need for the optimizer to eliminate it.
#define ANALYZER_SKIP_THIS_PATH()
#endif

#endif  // BASE_COMPILER_SPECIFIC_H_
