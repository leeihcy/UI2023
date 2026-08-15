#ifndef MOJO_PUBLIC_C_SYSTEM_TRAP_H_
#define MOJO_PUBLIC_C_SYSTEM_TRAP_H_

#include <stdint.h>
#include "mojo/public/c/system/types.h"

// Flags passed to trap event handlers within |MojoTrapEvent|.
typedef uint32_t MojoTrapEventFlags;

#ifdef __cplusplus
inline constexpr MojoTrapEventFlags MOJO_TRAP_EVENT_FLAG_NONE = 0;
inline constexpr MojoTrapEventFlags MOJO_TRAP_EVENT_FLAG_WITHIN_API_CALL = 1
                                                                           << 0;
#else
#define MOJO_TRAP_EVENT_FLAG_NONE ((MojoTrapEventFlags)0)
#define MOJO_TRAP_EVENT_FLAG_WITHIN_API_CALL ((MojoTrapEventFlags)1 << 0)
#endif


struct MojoTrapEvent {
  uint32_t struct_size;
  MojoTrapEventFlags flags;
  // MOJO_POINTER_FIELD(uintptr_t, trigger_context);
  uintptr_t* trigger_context;
  MojoResult result;
  struct MojoHandleSignalsState signals_state;
};

typedef uint32_t MojoTriggerCondition;

#ifdef __cplusplus
inline constexpr MojoTriggerCondition
    MOJO_TRIGGER_CONDITION_SIGNALS_UNSATISFIED = 0;
inline constexpr MojoTriggerCondition MOJO_TRIGGER_CONDITION_SIGNALS_SATISFIED =
    1;
#else
#define MOJO_TRIGGER_CONDITION_SIGNALS_UNSATISFIED ((MojoTriggerCondition)0)
#define MOJO_TRIGGER_CONDITION_SIGNALS_SATISFIED ((MojoTriggerCondition)1)
#endif


// Flags passed to |MojoCreateTrap()| via |MojoCreateTrapOptions|.
typedef uint32_t MojoCreateTrapFlags;

#ifdef __cplusplus
inline constexpr MojoCreateTrapFlags MOJO_CREATE_TRAP_FLAG_NONE = 0;
#else
#define MOJO_CREATE_TRAP_FLAG_NONE ((MojoCreateTrapFlags)0)
#endif

// Options passed to |MojoCreateTrap()|.
struct MojoCreateTrapOptions {
  // The size of this structure, used for versioning.
  uint32_t struct_size;

  // Flags. Currently unused.
  MojoCreateTrapFlags flags;
};

typedef uint32_t MojoAddTriggerFlags;

#ifdef __cplusplus
inline constexpr MojoAddTriggerFlags MOJO_ADD_TRIGGER_FLAG_NONE = 0;
#else
#define MOJO_ADD_TRIGGER_FLAG_NONE ((MojoAddTriggerFlags)0)
#endif

struct MojoAddTriggerOptions {
  uint32_t struct_size;
  MojoAddTriggerFlags flags;
};

typedef uint32_t MojoRemoveTriggerFlags;

#ifdef __cplusplus
inline constexpr MojoRemoveTriggerFlags MOJO_REMOVE_TRIGGER_FLAG_NONE = 0;
#else
#define MOJO_REMOVE_TRIGGER_FLAG_NONE ((MojoRemoveTriggerFlags)0)
#endif

struct MojoRemoveTriggerOptions {
  uint32_t struct_size;
  MojoRemoveTriggerFlags flags;
};


typedef uint32_t MojoArmTrapFlags;

#ifdef __cplusplus
inline constexpr MojoArmTrapFlags MOJO_ARM_TRAP_FLAG_NONE = 0;
#else
#define MOJO_ARM_TRAP_FLAG_NONE ((MojoArmTrapFlags)0)
#endif

struct MojoArmTrapOptions {
  uint32_t struct_size;
  MojoArmTrapFlags flags;
};


#ifdef __cplusplus
extern "C" {
#endif

// A user-provided callback to handle trap events. Passed to |MojoCreateTrap()|.
typedef void (*MojoTrapEventHandler)(const struct MojoTrapEvent* event);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // MOJO_PUBLIC_C_SYSTEM_TRAP_H_