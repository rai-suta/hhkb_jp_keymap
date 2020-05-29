#ifndef CONFIG_RAISUTA_H
#define CONFIG_RAISUTA_H

#include "keyboards/hhkb/config.h"

// quantum function settings
#undef  TAPPING_TERM
#define TAPPING_TERM        ( 500 )
#define TAPPING_FORCE_HOLD
#define LEADER_TIMEOUT      ( 1000 )

// action.c settings
#define TAP_CODE_DELAY      20

// cancel_capsLock settings
#ifdef HHKB_JP
# define CANCEL_CAPSLOCK_KEY_POSITIONS    \
    {                                     \
      {  0x3,  0x0 },   /* Tab(K03) */    \
      {  0x4,  0x7 },   /* Space(K74) */  \
      {  0x6,  0xC },   /* Enter(KC6) */  \
    }
#else
# define CANCEL_CAPSLOCK_KEY_POSITIONS    \
    {                                     \
      {  0x2,  0x3 },   /* Tab(K32) */    \
      {  0x7,  0x3 },   /* Space(K37) */  \
      {  0x3,  0x5 },   /* Enter(K53) */  \
    }
#endif

#endif  /* CONFIG_RAISUTA_H */
