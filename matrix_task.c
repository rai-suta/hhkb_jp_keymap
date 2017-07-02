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

#define TAP_ONCE(code)  do {  register_code(code);  \
                              unregister_code(code); } while(0)

#ifdef JIS_KEYBOARD
# define TAP_CAPSLOCK() do {  register_code(KC_LSFT); \
                              TAP_ONCE(KC_CAPSLOCK);  \
                              unregister_code(KC_LSFT); } while(0)
#else
# define TAP_CAPSLOCK() do {  TAP_ONCE(KC_CAPSLOCK); } while(0)
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

const uint16_t PROGMEM kcs[] = {KC_G, KC_A, 0};
static const struct {
  const uint16_t *keycodes;
  const char strings[];
} PROGMEM seq_strings[] = {
  //{ (const uint16_t[]){KC_G, KC_A, 0}, "git add ." },
  { kcs, "git add ." },
  { (const uint16_t[]){KC_G, KC_D, 0}, "git diff" },
  { (const uint16_t[]){KC_G, KC_D, KC_S, 0}, "git diff --staged" },
  { (const uint16_t[]){KC_G, KC_L, 0}, "git log" },
  { (const uint16_t[]){KC_G, KC_L, KC_O, 0}, "git log --oneline" },
  { (const uint16_t[]){KC_G, KC_F, 0}, "git fetch" },
  { (const uint16_t[]){KC_G, KC_O, 0}, "git checkout" },
  { (const uint16_t[]){KC_G, KC_P, 0}, "git pull" },
  { (const uint16_t[]){KC_G, KC_S, 0}, "git status" },
  { (const uint16_t[]){KC_G, KC_C, KC_A, 0}, "git commit --amend" },
};
static const int seq_strings_num = (sizeof seq_strings)/(sizeof seq_strings[0]);

static void act_leaderKey(void)
{
  dprintf("==== act_leaderKey ====\n");
  leading = false;
  leader_end();

  for ( int i = 0; i < seq_strings_num; i++ ) {
    const uint16_t *it_kc = seq_strings[i].keycodes;
    bool is_eq;
    int ls_cnt = 0;
    do {
      const uint16_t kc = pgm_read_word(it_kc);
      //is_eq = ( leader_sequence[ls_cnt] == *it_kc );
      is_eq = ( leader_sequence[ls_cnt] == kc );
      dprintf( "  is_eq = %u\n"
               "  leader_seq[%d] = %u\n"
               "  it_kc = %u\n"
               "  kc = %u\n"
                  , is_eq
                  , ls_cnt, leader_sequence[ls_cnt]
                  , (unsigned int)it_kc
                  , (unsigned int)kc );
      if ( kc == 0 ){ break; }
      ls_cnt++, it_kc++;
    } while ( /*is_eq &&*/ ls_cnt<5 );
    if ( !is_eq ){
      dprintf( "  continue\n" );
      continue;
    }
    else {
      dprintf( "  send_string\n" );
      send_string( seq_strings[i].strings );
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
