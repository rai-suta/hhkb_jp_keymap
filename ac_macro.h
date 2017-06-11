#ifndef AC_MACRO_H
#define AC_MACRO_H

#include "quantum.h"

enum osk_state {
  OSK_NONE = 0,
  OSK_DOWN,
  OSK_PRESS,
  OSK_MAINTAIN,
};

extern struct OneShotKeysStatus {
  enum osk_state keyState;
  uint8_t lastMacroId;
} oskStatus;

// This macro assigns three types of function key for down, tap, release.
// The 'down' and 'tap' work when key input as single.
// The 'down' and 'release' work when key input as combination.
#define MACRO_ONESHOT(down, tap, release)           \
  ({                                                \
    const macro_t* ret;                             \
    if ( record->event.pressed ){                   \
      ret = down;                                   \
      oskStatus = (struct OneShotKeysStatus){       \
                    .keyState = OSK_DOWN,           \
                    .lastMacroId = macro_id };      \
    }                                               \
    else if ( oskStatus.lastMacroId == macro_id ){  \
      ret = (oskStatus.keyState == OSK_PRESS)       \
              ? tap                                 \
              : release;                            \
      oskStatus.keyState = OSK_NONE;                \
    }                                               \
    else {                                          \
      ret = release;                                \
    }                                               \
    ret;                                            \
  })

#define MACRO_ONESHOT_MODIFIER(mods, kc)                          \
  MACRO_ONESHOT(  ( add_macro_mods(mods), MACRO_NONE ),           \
                  ( del_macro_mods(mods), MACRO(TYPE(kc), END) ), \
                  ( del_macro_mods(mods), MACRO_NONE ) )

#endif /* AC_MACRO_H */
