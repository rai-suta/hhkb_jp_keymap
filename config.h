#ifndef CONFIG_RAISUTA_H
#define CONFIG_RAISUTA_H

#include "keyboards/hhkb/config.h"

#undef  TAPPING_TERM
#define TAPPING_TERM        ( 200 )
#define ONESHOTKEY_TIMEOUT  ( 500 )

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