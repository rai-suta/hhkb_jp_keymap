#ifndef ACTION_MACRO_RAISUTA_H
#define ACTION_MACRO_RAISUTA_H

#include "quantum.h"
#include "tmk_core/common/action_macro.h"
  
#define MACRO_PRESS_TAP(record, press, tapped, release) \
  ( ((record)->event.pressed) \
      ? ( press ) \
      : ( (record)->tap.count > 0 && !(record)->tap.interrupted ) \
        ? ( tapped ) \
        : ( release ) )

#define MACRO_SEND_NONE \
  ( send_keyboard_report(), MACRO_NONE )

#define MACRO_HOLDMOD_TAPKC(record, mod_bits, kc) \
  MACRO_PRESS_TAP(  record, \
                    ( add_macro_mods(mod_bits), MACRO_SEND_NONE ), \
                    ( del_macro_mods(mod_bits), MACRO(TYPE(kc), END) ), \
                    ( del_macro_mods(mod_bits), MACRO_SEND_NONE ))

#endif /* ACTION_MACRO_RAISUTA_H */
