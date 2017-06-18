#include "quantum.h"
#include "action_macro.h"

struct OneShotKeysStatus oskStatus;

extern bool matrix_key_count(void);  /* matrix.c */
static void cancel_capsLock(void);
static void process_oneshotKey(void);

#ifdef CONSOLE_ENABLE

# ifdef PRINT_MATRIX_ENABLE
    static void __print_matrix(void);
#   define print_matrix()     do { if (debug_enable) __print_matrix(); } while (0)
# else
#   define print_matrix()     do { } while(0)
# endif

  static void __print_layer_state(void);
# define print_layer_state()  do { if (debug_enable) __print_layer_state(); } while (0)

  static void __print_oneShotKeyState(void);
# define print_oneShotKeyState() do { if (debug_enable) __print_oneShotKeyState(); } while (0)

  static void __print_mods(void);
# define print_mods()         do { if (debug_enable) __print_mods(); } while (0)

#else   /* CONSOLE_ENABLE */

# define print_matrix()
# define print_layer_state()
# define print_oneShotKeyState()

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

  if ( oskStatus.keyState != OSK_NONE ){
    process_oneshotKey();
  }

  print_matrix();
  print_layer_state();
  print_oneShotKeyState();
  print_mods();
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

// Process OneShotKey status.
static void process_oneshotKey(void)
{
  static uint16_t lastTime;
  static uint8_t lastKeyCount;

  switch (oskStatus.keyState){
    case OSK_DOWN: {
      lastTime = timer_read();
      lastKeyCount = matrix_key_count();
      oskStatus.keyState = OSK_PRESS;
    } break;

    case OSK_PRESS: {
#     if defined(ONESHOTKEY_TIMEOUT)
      if ( timer_elapsed(lastTime) > ONESHOTKEY_TIMEOUT ){
        oskStatus.keyState = OSK_MAINTAIN;
        dprintf( "OneShotKey time-out\n" );
      }
#     endif /* ONESHOTKEY_TIMEOUT */

    case OSK_MAINTAIN: ;
      uint8_t keyCount = matrix_key_count();
      if ( keyCount > lastKeyCount ){
        oskStatus.keyState = OSK_NONE;
      } else {
        lastKeyCount = keyCount;
      }
    } break;

    default:{
      oskStatus.keyState = OSK_NONE;
    } break;
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

static void __print_oneShotKeyState(void)
{
  static enum osk_state last_state;

  if (last_state != oskStatus.keyState){
    dprintf("oskStatus\n"
            "  .keyState = %d\n"
            "  .lastMacroId = %d\n", 
              oskStatus.keyState, oskStatus.lastMacroId );
    last_state = oskStatus.keyState;
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
