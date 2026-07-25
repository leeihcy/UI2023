#ifndef MOJO_PUBLIC_C_SYSTEM_TYPES_H_
#define MOJO_PUBLIC_C_SYSTEM_TYPES_H_

#include <stdint.h>

// |MojoHandle|: Handles to Mojo objects.
//   |MOJO_HANDLE_INVALID| - A value that is never a valid handle.

typedef uintptr_t MojoHandle;

#ifdef __cplusplus
inline constexpr MojoHandle MOJO_HANDLE_INVALID = 0;
#else
#define MOJO_HANDLE_INVALID ((MojoHandle)0)
#endif



typedef uint32_t MojoResult;

inline constexpr MojoResult MOJO_RESULT_OK = 0;
inline constexpr MojoResult MOJO_RESULT_CANCELLED = 1;
inline constexpr MojoResult MOJO_RESULT_UNKNOWN = 2;
inline constexpr MojoResult MOJO_RESULT_INVALID_ARGUMENT = 3;
inline constexpr MojoResult MOJO_RESULT_DEADLINE_EXCEEDED = 4;
inline constexpr MojoResult MOJO_RESULT_NOT_FOUND = 5;
inline constexpr MojoResult MOJO_RESULT_ALREADY_EXISTS = 6;
inline constexpr MojoResult MOJO_RESULT_PERMISSION_DENIED = 7;
inline constexpr MojoResult MOJO_RESULT_RESOURCE_EXHAUSTED = 8;
inline constexpr MojoResult MOJO_RESULT_FAILED_PRECONDITION = 9;
inline constexpr MojoResult MOJO_RESULT_ABORTED = 10;
inline constexpr MojoResult MOJO_RESULT_OUT_OF_RANGE = 11;
inline constexpr MojoResult MOJO_RESULT_UNIMPLEMENTED = 12;
inline constexpr MojoResult MOJO_RESULT_INTERNAL = 13;
inline constexpr MojoResult MOJO_RESULT_UNAVAILABLE = 14;
inline constexpr MojoResult MOJO_RESULT_DATA_LOSS = 15;
inline constexpr MojoResult MOJO_RESULT_BUSY = 16;
inline constexpr MojoResult MOJO_RESULT_SHOULD_WAIT = 17;

#endif