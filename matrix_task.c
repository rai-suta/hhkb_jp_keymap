#include "quantum.h"
#include "config.h"
#include "action_macro.h"
#include <stdarg.h>

LEADER_EXTERNS();   // Leader key process variable declaration

extern bool matrix_key_count(void);   /* matrix.c */
static void cancel_capsLock(void);
static void dprint_layer_state(void);
static void dprint_mods(void);

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

  // be triggered from KC_LEAD
  LEADER_DICTIONARY() {
    leader_end();
  }

  if ( debug_enable ) {
    dprint_layer_state();
    dprint_mods();
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

// Leader key settings
#define KC__  KC_NO
#define COMB_KC_STR( k1, k2, k3, k4, k5, str ) \
        { .keyseq = { KC_##k1, KC_##k2, KC_##k3, KC_##k4, KC_##k5 }, .string = str }
#define NUM_OF( x )   ( sizeof(x) / sizeof((x)[0]) )

// Begin call back from process_leader
void leader_start(void)
{
  dprintf( "leader_start\n" );
  layer_clear();
}

// End call back from process_leader
void leader_end(void)
{
  static struct {
    uint16_t keyseq[5];
    char string[];
  } const PROGMEM LEADER_KEY_SETTINGS[] = {
    COMB_KC_STR( G, A, _, _, _, "git add ."),
    COMB_KC_STR( G, D, _, _, _, "git diff"),
    COMB_KC_STR( G, D, S, _, _, "git diff --staged"),
    COMB_KC_STR( G, L, _, _, _, "git log"),
    COMB_KC_STR( G, L, O, _, _, "git log --online"),
    COMB_KC_STR( G, F, _, _, _, "git fetch"),
    COMB_KC_STR( G, O, _, _, _, "git checkout"),
    COMB_KC_STR( G, P, _, _, _, "git pull"),
    COMB_KC_STR( G, S, _, _, _, "git status"),
    COMB_KC_STR( G, C, A, _, _, "git commit --amend"),
  };
  const size_t NUM_OF_LEADER_KEY_SETTINGS = NUM_OF(LEADER_KEY_SETTINGS);
  const int RANDOM_WORD_LENGTH = 8u;

  dprintf( "leader_end\n" );
  leading = false;

  SEQ_TWO_KEYS(KC_G, KC_C) {
    // type "git commit -m ''"
    SEND_STRING( "git commit -m ''" );
    action_macro_play( MACRO( T(LEFT), END ));  // move cursor into single quotes
  }
  else SEQ_TWO_KEYS(KC_R, KC_W) {
    // type <random word>
    for ( int i = RANDOM_WORD_LENGTH; i > 0; i-- ){
      tap_random_base64();
    }
  }
  else for ( int i = 0; i < NUM_OF_LEADER_KEY_SETTINGS; i++ ) {
    const uint16_t *it_keyseq  = LEADER_KEY_SETTINGS[i].keyseq;
    const char     *it_string  = LEADER_KEY_SETTINGS[i].string;

    SEQ_FIVE_KEYS(
      pgm_read_word(it_keyseq+0),
      pgm_read_word(it_keyseq+1),
      pgm_read_word(it_keyseq+2),
      pgm_read_word(it_keyseq+3),
      pgm_read_word(it_keyseq+4)
    ){
      // send string
      send_string_P( it_string );
      break;
    }
  }
}

#ifdef CONSOLE_ENABLE

static void dprint_layer_state(void)
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

static void dprint_mods(void)
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
