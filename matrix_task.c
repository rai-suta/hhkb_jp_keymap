#include "quantum.h"
#include "action_macro.h"
#include "sendstring_jis.h"
#include <stdarg.h>

LEADER_EXTERNS();
extern bool matrix_key_count(void);  /* matrix.c */
static void cancel_capsLock(void);
static void act_leaderKey(void);

#ifdef CONSOLE_ENABLE

  static void __print_layer_state(void);
# define print_layer_state()  do { if (debug_enable) __print_layer_state(); } while (0)

  static void __print_mods(void);
# define print_mods()         do { if (debug_enable) __print_mods(); } while (0)

#else   /* CONSOLE_ENABLE */

# define print_layer_state()
# define print_mods()

#endif  /* CONSOLE_ENABLE */

// Runs just one time when the keyboard initializes.
// TODO: Call from "hhkb/matrix.c".
void matrix_init_user(void)
{
}

// Runs constantly in the background, in a loop.
void matrix_scan_user(void)
{
  uint8_t keyboardLeds = host_keyboard_leds();

  static uint8_t mem_keyboardLeds;
  if ( mem_keyboardLeds != keyboardLeds ){
    dprintf( "keyboardLeds = %08b\n", keyboardLeds );
    mem_keyboardLeds = keyboardLeds;
  }

  if ( keyboardLeds & (1u << USB_LED_CAPS_LOCK) ){
    cancel_capsLock();
  }

  static bool led_kana = false;
  if ( keyboardLeds & (1u << USB_LED_KANA) ){
    if ( !led_kana ) {
      dprintf( "USB_LED_KANA ON\n" );
    }
    led_kana = true;
  }
  else {
    led_kana = false;
  }

  // when timeout from typing of KC_LEAD
  LEADER_DICTIONARY() {
    leading = false;
    leader_end();
    act_leaderKey();
  }

  if (true) {
    print_layer_state();
    print_mods();
  }
}

#ifdef JIS_KEYCODE
# define TAP_CAPSLOCK() action_macro_play( MACRO( D(LSFT), T(CAPSLOCK), U(LSFT), END ))
#else
# define TAP_CAPSLOCK() action_macro_play( MACRO( T(CAPSLOCK), END ))
#endif

// Turn off CapsLock when CANCEL_CAPSLOCK_KEY_POSITIONS are typed.
static void cancel_capsLock(void)
{
#ifdef CANCEL_CAPSLOCK_KEY_POSITIONS
  static keypos_t const keypos_toCancelCapslock[] = CANCEL_CAPSLOCK_KEY_POSITIONS;
  static size_t const ktcc_length = sizeof( keypos_toCancelCapslock ) / sizeof( keypos_t );

  // if layer_state is switching then, process break.
  if ( layer_state > default_layer_state ){
    return;
  }
  
  // if shift-key being pressed then, process break.
  bool isShiftOn = get_mods() & (MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT));
  if ( isShiftOn ){
    return;
  }

  for ( int i = 0; i < ktcc_length; i++ ){
    keypos_t it_kp = keypos_toCancelCapslock[i];
    if ( matrix_is_on( it_kp.row, it_kp.col ) ){
      TAP_CAPSLOCK();
      dprintf("Cancel CapsLock\n");
      break;
    }
  }
#endif
}

struct for_keySeq2String {
  const uint16_t keyseq[5];
  const char string[];
};

#define KC___   KC_NO
#define SEQ_STR_EVAL( macro )  \
  macro( G,  A, __, __, __, "git add ."), \
  macro( G,  D, __, __, __, "git diff"), \
  macro( G,  D,  S, __, __, "git diff --staged"), \
  macro( G,  L, __, __, __, "git log"), \
  macro( G,  L,  O, __, __, "git log --online"), \
  macro( G,  F, __, __, __, "git fetch"), \
  macro( G,  O, __, __, __, "git checkout"), \
  macro( G,  P, __, __, __, "git pull"), \
  macro( G,  S, __, __, __, "git status"), \
  macro( G,  C,  A, __, __, "git commit --amend"), \
  macro( G,  C, __, __, __, "git commit -m ''")
#define SEQ_STR_ITEMS( k1, k2, k3, k4, k5, str ) \
  k1##k2##k3##k4##k5 PROGMEM = { { KC_##k1, KC_##k2, KC_##k3, KC_##k4, KC_##k5 }, str }
#define SEQ_STR_PITEMS( k1, k2, k3, k4, k5, ... ) \
  &k1##k2##k3##k4##k5

#define NUM_OF( x )   ( sizeof(x) / sizeof((x)[0]) )

static void act_leaderKey(void)
{
  static const struct for_keySeq2String 
    SEQ_STR_EVAL( SEQ_STR_ITEMS ),
    * const seq_strings[] = { SEQ_STR_EVAL( SEQ_STR_PITEMS ) };

  for ( int i = 0; i < NUM_OF(seq_strings); i++ ) {
    const uint16_t *it_ks = seq_strings[i]->keyseq;
    const char *it_str = seq_strings[i]->string;
    SEQ_FIVE_KEYS(
      pgm_read_word(it_ks),
      pgm_read_word(++it_ks),
      pgm_read_word(++it_ks),
      pgm_read_word(++it_ks),
      pgm_read_word(++it_ks)
    ){
      if ( seq_strings[i] == SEQ_STR_PITEMS(G, C, __ ,__ ,__) ) {
        send_string_P( it_str );
        action_macro_play( MACRO( T(LEFT), END ));
      } 
      else {
        send_string_P( it_str );
      }

      return;
    }
  }
  
  SEQ_TWO_KEYS(KC_G, KC_C) {
      SEND_STRING("git commit -m ''");
      action_macro_play( MACRO( T(LEFT), END ));
  }
}

#ifdef CONSOLE_ENABLE

static void __print_layer_state(void)
{
  static uint32_t last_state;

  if ( last_state != layer_state ){
    dprintf("default_layer_state\n"
            "  = %032lb\n"
            "layer_state\n"
            "  = %032lb\n"
            , default_layer_state, layer_state);
    last_state = layer_state;
  }
}

struct for_mods {
  uint8_t real, weak, macro, oneshot;
};
#define EQ_MODS( x, y )   ( x.real == y.real      \
                            && x.weak == y.weak   \
                            && x.macro == y.macro \
                            && x.oneshot == y.oneshot )

static void __print_mods(void)
{
  static struct for_mods last_mods;
  struct for_mods mods = { get_mods(), get_weak_mods(), get_macro_mods(), get_oneshot_mods() };

  if ( !EQ_MODS(last_mods, mods) ){
    dprintf("mods\n"
            "  .r. = %08b\n"
            "  .w. = %08b\n"
            "  .m. = %08b\n" 
            "  .o. = %08b\n"
            , mods.real, mods.weak, mods.macro, mods.oneshot );
    last_mods = mods;
  }
}

#endif /* CONSOLE_ENABLE */
