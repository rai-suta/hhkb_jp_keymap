#include "quantum.h"
#include "action_macro.h"
#include <stdarg.h>

LEADER_EXTERNS();
extern bool matrix_key_count(void);  /* matrix.c */
static void cancel_capsLock(void);
static void act_leaderKey(void);

#ifdef CONSOLE_ENABLE

# ifdef PRINT_MATRIX_ENABLE
    static void __print_matrix(void);
#   define print_matrix()     do { if (debug_enable) __print_matrix(); } while (0)
# else
#   define print_matrix()     do { } while(0)
# endif

  static void __print_layer_state(void);
# define print_layer_state()  do { if (debug_enable) __print_layer_state(); } while (0)

  static void __print_mods(void);
# define print_mods()         do { if (debug_enable) __print_mods(); } while (0)

#else   /* CONSOLE_ENABLE */

# define print_matrix()
# define print_layer_state()

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

  if ( keyboardLeds & (1<<USB_LED_CAPS_LOCK) ){
    cancel_capsLock();
  }

  LEADER_DICTIONARY() {
    act_leaderKey();
  }

  if (false) {
    print_matrix();
    print_layer_state();
    print_mods();
  }
}

enum next_key_down_up {
    NK_DOWN_UP,
    NK_DOWN,
    NK_UP // a bit of a hack, this works as long as NK_UP < KC_A
};

void send_keystrokes(uint8_t key, ...)
{
    va_list vl;
    va_start(vl, key);
    enum next_key_down_up nkdu = NK_DOWN_UP;
    while (key != KC_NO) {
        if (key < KC_A) {
            nkdu = key;
        } else {
            switch (nkdu) {
            case NK_DOWN_UP:
                register_code(key);
            case NK_UP:
                unregister_code(key);
                break;
            case NK_DOWN:
                register_code(key);
            }
            nkdu = NK_DOWN_UP;
        }
        key = va_arg(vl, int);
    }
    va_end(vl);
}

#ifdef JIS_KEYCODE
# define TAP_CAPSLOCK() send_keystrokes( NK_DOWN, KC_LSFT, KC_CAPSLOCK, NK_UP, KC_LSFT)
#else
# define TAP_CAPSLOCK() send_keystrokes( KC_CAPSLOCK )
#endif

// Turn off CapsLock when CANCEL_CAPSLOCK_KEY_POSITIONS are typed.
static void cancel_capsLock(void)
{
#ifdef CANCEL_CAPSLOCK_KEY_POSITIONS
  static keypos_t const keypos_toCancelCapslock[] = CANCEL_CAPSLOCK_KEY_POSITIONS;
  static size_t const ktcc_length = sizeof( keypos_toCancelCapslock ) / sizeof( keypos_t );

  if ( layer_state != default_layer_state ){
    return;
  }
  
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

#define SEQ_STRINGS( eval_kc, eval_str )  \
  eval_kc( G,  A, NO, NO, NO) eval_str("git add .") \
  eval_kc( G,  D, NO, NO, NO) eval_str("git diff") \
  eval_kc( G,  D,  S, NO, NO) eval_str("git diff --staged") \
  eval_kc( G,  L, NO, NO, NO) eval_str("git log") \
  eval_kc( G,  L,  O, NO, NO) eval_str("git log --online") \
  eval_kc( G,  F, NO, NO, NO) eval_str("git fetch") \
  eval_kc( G,  O, NO, NO, NO) eval_str("git checkout") \
  eval_kc( G,  P, NO, NO, NO) eval_str("git pull") \
  eval_kc( G,  S, NO, NO, NO) eval_str("git status") \
  eval_kc( G,  C,  A, NO, NO) eval_str("git commit --amend")
#define ITEM_KS( kc1, kc2, kc3, kc4, kc5 ) \
  const struct for_keySeq2String PROGMEM kc1##kc2##kc3##kc4##kc5 = { { KC_##kc1, KC_##kc2, KC_##kc3, KC_##kc4, KC_##kc5 }, 
#define ITEM_STR( str ) \
  str };
#define ITEM_SEQ_STRINGS( kc1, kc2, kc3, kc4, kc5 ) \
  &kc1##kc2##kc3##kc4##kc5, 
#define NOT_EVAL(...)

SEQ_STRINGS( ITEM_KS, ITEM_STR )
static const struct for_keySeq2String * const seq_strings[] = { SEQ_STRINGS( ITEM_SEQ_STRINGS, NOT_EVAL ) };
#define NUM_OF( x )   ( sizeof(x) / sizeof((x)[0]) )

static void act_leaderKey(void)
{
  leading = false;
  leader_end();

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
      send_string( it_str );
      return;
    }
  }
  
  SEQ_TWO_KEYS(KC_G, KC_C) {
      SEND_STRING("git commit -m ''");
      send_keystrokes(KC_LEFT, KC_NO);
  }
}

#ifdef CONSOLE_ENABLE

# ifdef PRINT_MATRIX_ENABLE
static void __print_matrix(void)
{
  // error: 'matrix_is_modified' is deprecated.
  if ( matrix_is_modified() ){
    matrix_print();
  }
}
# endif

static void __print_layer_state(void)
{
  static uint32_t last_state;

  if ( last_state != layer_state ){
    dprintf("default_layer_state\n"
            "    = %032lb\n", default_layer_state);
    dprintf("layer_state\n"
            "    = %032lb\n", layer_state);
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
            "    .real  = %08b\n"
            "    .weak  = %08b\n"
            "    .macro = %08b\n" 
            "    .oneshot = %08b\n", mods.real, mods.weak, mods.macro, mods.oneshot );
    last_mods = mods;
  }
}

#endif /* CONSOLE_ENABLE */
