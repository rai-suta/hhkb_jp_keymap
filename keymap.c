#include "hhkb.h"
#include "config.h"
#include "action_macro.h"
#include "sendstring_jis.h"

// Key position per keyboard layout
#ifdef JIS_KEYCODE
  // JIS keyboard layout
# define KP_L_BRACKET_S         ( KC_RBRACKET ) // [ ({)
# define KP_R_BRACKET_S         ( KC_BSLASH )   // ] (})
# define KP__L_RBRACKET         ( KC_8 )        // unshifted (
# define KP__R_RBRACKET         ( KC_9 )        // unshifted )
# define KP__L_ABRANKETS        ( KC_COMMA )    // unshifted <
# define KP__R_ABRANKETS        ( KC_DOT )      // unshifted >
# define KP__SGLQUOTE           ( KC_7 )        // unshifted '
# define KP__DBLQUOTE           ( KC_2 )        // unshifted "
#endif
#ifdef ISO_KEYCODE
  // ISO keyboard layout
# define KP_L_BRACKET_S         ( KC_LBRACKET ) // [ ({)
# define KP_R_BRACKET_S         ( KC_RBRACKET ) // ] (})
# define KP__L_RBRACKET         ( KC_9 )        // unshifted (
# define KP__R_RBRACKET         ( KC_0 )        // unshifted )
# define KP__L_ABRANKETS        ( KC_COMMA )    // unshifted <
# define KP__R_ABRANKETS        ( KC_DOT )      // unshifted >
# define KP_SGLQUOTE_S          ( KC_QUOT )     // ' (")
#endif

// Fn keys definition.
//    fn_actions[FN2IDX(...)]
#define FN2IDX( x )             ( x - KC_FN0 )

// User macro identifer
//    M(...)
enum user_macro{
  UM_NOTHING = 0,
  // UM_SWITCH_* is that goto layer when pressed, return layer when release
  UM_SWITCH_EDIT_LAYER_WITH_KC,
  UM_SWITCH_INPUT_LAYER_WITH_KC,
  UM_SWITCH_STNG_LAYER,
  // UM_TOGGLE_* is that toggle default_layer
  UM_TOGGLE_PLN_DVORAK,
  UM_TOGGLE_MOD_ARROW,
  UM_TOGGLE_MOD_SANDS,
  UM_INPUT_PAIRED_BRANKETS,
  UM_CLEAR_DEFAULT_LAYER,
  UM_DISPLAY_SETTINGS,
  UM_TAP_RANDOM_BASE64,
};
#define UM( id )  ( M(UM_##id) )
#define KC_SECO  ( KC_MHEN )    // input key when UM_SWITCH_EDIT_LAYER_WITH_KC is tapped
#define KC_SINO  ( KC_HENK )    // input key when UM_SWITCH_INPUT_LAYER_WITH_KC is tapped
static void action_displaySettings( void );

enum custom_keycodes {
  CUSTOM_KEYCODE = SAFE_RANGE,
  UNDO,
  CUT,
  COPY,
  PASTE,
  REDO,
  DELETE_FORWARD_WORD,
  DELETE_BACKWARD_WORD,
  SWITCH_EDIT_LAYER,
};

enum tap_dance_code {
  TC_SELECT,
};

// custom keycode
#define DEL_FW    DELETE_FORWARD_WORD
#define DEL_BW    DELETE_BACKWARD_WORD
#define SW_EDIT   SWITCH_EDIT_LAYER

// tap dance
#ifdef TAP_DANCE_ENABLE
  #define TD_SLCT   TD(TC_SELECT)
  #define TAP_DANCE_MOD_SFT   MOD_BIT(KC_RSFT)
#else
  #define TD_SLCT   XXXXXXX
  #define TAP_DANCE_MOD_SFT   0
#endif

// modified keycode
#define C_HOME    C(KC_HOME)    // move to top
#define C_END     C(KC_END)     // move to bottom
#define C_LEFT    C(KC_LEFT)    // move to backward-word
#define C_RGHT    C(KC_RGHT)    // move to forward-word
#define A_LEFT    A(KC_LEFT)    // previous location
#define A_RIGHT   A(KC_RIGHT)   // next location
#define SCRL_UP   C(KC_UP)      // scroll line up
#define SCRL_DN   C(KC_DOWN)    // scroll line down
#define SCRL_PU   A(KC_PGUP)    // scroll page up
#define SCRL_PD   A(KC_PGDOWN)  // scroll page down

// modifier keycode
#define OSM_CTL  OSM(MOD_LCTL)
#define OSM_ALT  OSM(MOD_LALT)
#define OSM_SFT  OSM(MOD_LSFT)
#define OSM_GUI  OSM(MOD_LGUI)

// switching and toggling layers
#define MO_LOWER  MO(KL_EDIT_CRSR)
#define MO_RAISE  MO(KL_INPUT)

// user macro
#define M_TEL    ( M(UM_TURN_EDIT_LAYER) )
#define M_SSL    ( M(UM_SWITCH_STNG_LAYER) )

// keymap layer names
#define LAYER_NAMES_EVAL( func ) \
  func(QWERTY),     \
  func(DVORAK),     \
  func(FN_KEYS),    \
  func(MOD_RSIDE),  \
  func(MOD_SANDS),  \
  func(EDIT_CRSR),  \
  func(EDIT_SCRL),  \
  func(EDIT_MEDIA), \
  func(INPUT),      \
  func(STNG)

// keymaps subscript
//    keymaps[...]
#define KL_( name )     KL_##name
enum keymap_layer{
  LAYER_NAMES_EVAL( KL_ ),
  KL_NUM
};
#define LAYER_MASK_OF_EDIT          ( 1UL << KL_(EDIT_CRSR)   \
                                      | 1UL << KL_(EDIT_SCRL) \
                                      | 1UL << KL_(EDIT_MEDIA) )
#define LAYER_MASK_OF_INPUT         ( 1UL << KL_(INPUT) )
#define LAYER_MASK_OF_STNG          ( 1UL << KL_(STNG) )
static void toggle_default_layer( enum keymap_layer layer );
static const char* layerNameStr_P( enum keymap_layer layer );

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

# define MO_FN    ( MO(KL_(FN_KEYS)) )
# define M_MHEN   ( MACROTAP(UM_SWITCH_EDIT_LAYER_WITH_KC) )  // switch layer when pressed, send KC_SECO when tapped
# define M_HENK   ( MACROTAP(UM_SWITCH_INPUT_LAYER_WITH_KC) ) // switch layer when pressed, send KC_SINO when tapped
  [KL_(QWERTY)] = LAYOUT_JP(
     KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,     KC_8,    KC_9,    KC_0, KC_MINS,  KC_EQL, KC_JYEN, KC_BSPC,
     KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,     KC_I,    KC_O,    KC_P, KC_LBRC, KC_RBRC,
    KC_LCTL,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,     KC_K,    KC_L, KC_SCLN, KC_QUOT, KC_BSLS,  KC_ENT,
    KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,  KC_COMM,  KC_DOT, KC_SLSH,   KC_RO,   KC_UP, KC_RSFT,
      MO_FN, KC_ZKHK, KC_LGUI, KC_LALT,  M_MHEN,      KC_SPC     ,  M_HENK,  KC_KANA, KC_RALT,   MO_FN, KC_LEFT, KC_DOWN, KC_RGHT
  ),

  [KL_(DVORAK)] = LAYOUT_JP(
    _______,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,     KC_8,    KC_9,    KC_0, KC_LBRC, KC_RBRC,  KC_GRV, _______,
    _______, KC_QUOT, KC_COMM,  KC_DOT,    KC_P,    KC_Y,    KC_F,    KC_G,     KC_C,    KC_R,    KC_L, KC_SLSH,  KC_EQL,
    _______,    KC_A,    KC_O,    KC_E,    KC_U,    KC_I,    KC_D,    KC_H,     KC_T,    KC_N,    KC_S, KC_MINS, _______, _______,
    _______, KC_SCLN,    KC_Q,    KC_J,    KC_K,    KC_X,    KC_B,    KC_M,     KC_W,    KC_V,    KC_Z, KC_JYEN, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______,  _______, _______, _______, _______, _______, _______
  ),

  [KL_(FN_KEYS)] = LAYOUT_JP(
    KC_SLEP,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,  KC_INS,  KC_DEL,
    KC_CAPS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_PSCR, KC_SLCK, KC_PAUS,   KC_UP, XXXXXXX,
    _______, KC_VOLD, KC_VOLU, KC_MUTE, KC_EJCT, XXXXXXX, KC_PAST, KC_PSLS, KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT, XXXXXXX, KC_PENT,
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_PPLS, KC_PMNS,  KC_END, KC_PGDN, KC_DOWN, XXXXXXX, KC_RSFT, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______,  KC_APP, KC_RGUI, KC_RCTL
  ),

  [KL_(MOD_RSIDE)] = LAYOUT_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_RSFT, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______,  KC_APP, KC_RGUI, KC_RCTL
  ),

# define M_SAS    ( MT(MOD_LSFT, KC_SPACE) )
  [KL_(MOD_SANDS)] = LAYOUT_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,       M_SAS     , _______, _______, _______, _______, _______, _______, _______
  ),

  [KL_(EDIT_CRSR)] = LAYOUT_JP(
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
    _______,  KC_ESC,  C_HOME,   C_END, SW_EDIT, XXXXXXX,    REDO, TD_SLCT, KC_HOME,  KC_END, XXXXXXX, XXXXXXX, XXXXXXX,
    OSM_CTL, KC_PGUP,   KC_UP, KC_DOWN, KC_PGDN,  KC_DEL, KC_BSPC,  C_LEFT, KC_LEFT, KC_RGHT,  C_RGHT, XXXXXXX, XXXXXXX, _______,
    OSM_SFT,    UNDO,     CUT,    COPY,   PASTE, XXXXXXX, XXXXXXX,  KC_ENT,  DEL_BW,  DEL_FW, XXXXXXX, XXXXXXX, OSM_SFT, OSM_SFT,
    _______, _______, OSM_GUI, OSM_ALT, _______,     _______     , _______, _______, OSM_ALT, _______, _______, OSM_GUI, OSM_CTL
  ),

  [KL_(EDIT_SCRL)] = LAYOUT_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______,  A_LEFT, A_RIGHT, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, SCRL_PU, SCRL_UP, SCRL_DN, SCRL_PD, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______, _______, _______, _______
  ),

  [KL_(EDIT_MEDIA)] = LAYOUT_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, KC_MPRV, KC_MNXT, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, KC_MRWD, KC_MSTP, KC_MPLY, KC_MFFD, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, KC_VOLD, KC_VOLU, KC_MUTE, KC_EJCT, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______, _______, _______, _______
  ),

# define M_PB     ( M(UM_INPUT_PAIRED_BRANKETS) )
# define M_RAND   ( M(UM_TAP_RANDOM_BASE64) )
# define KC_NMLC  ( KC_NUMLOCK )
  [KL_(INPUT)] = LAYOUT_JP(
    _______, XXXXXXX,    M_PB, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    M_PB,    M_PB,    M_PB, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
    _______, XXXXXXX, XXXXXXX, KC_LEAD,  M_RAND, XXXXXXX, XXXXXXX, KC_NMLC, KC_PSCR, KC_SLCK, KC_PAUS,    M_PB,    M_PB,
    _______,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, _______,
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    M_PB, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______, _______, _______, _______
  ),

# define M_CDL    ( M(UM_CLEAR_DEFAULT_LAYER) )
# define M_TPD    ( M(UM_TOGGLE_PLN_DVORAK) )
# define M_TMA    ( M(UM_TOGGLE_MOD_ARROW) )
# define M_TMS    ( M(UM_TOGGLE_MOD_SANDS) )
# define M_DSST   ( M(UM_DISPLAY_SETTINGS) )
  [KL_(STNG)] = LAYOUT_JP(
    XXXXXXX,   M_TPD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   M_CDL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX,  M_DSST, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   M_TMA, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   M_SSL,       M_TMS     ,   M_SSL, XXXXXXX, XXXXXXX, XXXXXXX,   M_TMA,   M_TMA,   M_TMA
  ),

};

uint32_t layer_state_set_user(uint32_t state)
{
  del_mods(TAP_DANCE_MOD_SFT);
  state = update_tri_layer_state(state, KL_(EDIT_CRSR), KL_(INPUT), KL_(STNG));
  return state;
}

const uint16_t PROGMEM fn_actions[] = {
};

// Modifier utility
typedef uint8_t mod_bits_t;
struct for_mods {
  mod_bits_t real, weak, macro, oneshot;
};
static const mod_bits_t SHIFT_MOD_BITS = MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT);
static struct for_mods get_stMods( void );
static mod_bits_t get_orMods( void );
static void set_stMods( struct for_mods mods );
static void clear_allMods( void );

// subroutine of action_get_macro
static const macro_t* getMacro_pairedBrankets( keyrecord_t *record, uint16_t keycode, bool isShifted );
static uint16_t getKeycode_fromDefaultLayer( keyrecord_t *record );

const macro_t*
action_get_macro(keyrecord_t *record, uint8_t macro_id, uint8_t opt)
{
  const bool event_pressed = record->event.pressed;
  const bool isShifted = !!( get_orMods() & SHIFT_MOD_BITS );

  dprintf( "\n==== action_get_macro ====\n"
           "record   = { %u, %u, %u }\n"
           "macro_id = %u\n"
           "opt      = %u\n"
           , record->event.pressed
           , record->tap.count
           , record->tap.interrupted  // set from ACTION_MACRO_TAP, MACROTAP
           , macro_id
           , opt );

  switch (macro_id) {
    case UM_SWITCH_EDIT_LAYER_WITH_KC: {
      return MACRO_TAP_HOLD( record,
                /*press*/     ( layer_on(KL_(EDIT_CRSR)),       MACRO_NONE ),
                /*release*/   ( layer_and(~LAYER_MASK_OF_EDIT), MACRO_SEND_NONE ),
                /*tap*/       ( layer_and(~LAYER_MASK_OF_EDIT), MACRO(TYPE(KC_SECO), END) ));
    } break;

    case UM_SWITCH_INPUT_LAYER_WITH_KC: {
      return MACRO_TAP_HOLD( record,
                /*press*/     ( layer_on(KL_(INPUT)),            MACRO_NONE ),
                /*release*/   ( layer_and(~LAYER_MASK_OF_INPUT), MACRO_SEND_NONE ),
                /*tap*/       ( layer_and(~LAYER_MASK_OF_INPUT), MACRO(TYPE(KC_SINO), END) ));
    } break;

    case UM_SWITCH_STNG_LAYER: {
      clear_allMods();
      if ( layer_state & (1UL << KL_(STNG)) ){
        layer_off( KL_(STNG) );
      }
      else {
        layer_move( KL_(STNG) );
      }
    } break;

    case UM_INPUT_PAIRED_BRANKETS: {
      // evacuate or restoration mods
      static struct for_mods evacuatedMods;
      if ( event_pressed ) {
        evacuatedMods = get_stMods();
        clear_allMods();
      }
      else {
        set_stMods( evacuatedMods );
      }
      send_keyboard_report();

      uint16_t keycode = getKeycode_fromDefaultLayer( record );
      return getMacro_pairedBrankets( record, keycode, isShifted );
    } break;

    case UM_CLEAR_DEFAULT_LAYER: if ( event_pressed ) {
      default_layer_set( 0 );
      eeconfig_update_default_layer( default_layer_state );
    } break;

    case UM_TOGGLE_PLN_DVORAK: if ( event_pressed ) {
      toggle_default_layer( KL_(DVORAK) );
      eeconfig_update_default_layer( default_layer_state );
    } break;

    case UM_TOGGLE_MOD_SANDS: if ( event_pressed ) {
      toggle_default_layer( KL_(MOD_SANDS) );
      eeconfig_update_default_layer( default_layer_state );
    } break;

    case UM_TOGGLE_MOD_ARROW: if ( event_pressed ) {
      toggle_default_layer( KL_(MOD_RSIDE) );
      eeconfig_update_default_layer( default_layer_state );
    } break;

    case UM_DISPLAY_SETTINGS: if ( event_pressed ) {
      action_displaySettings();
    } break;

    case UM_TAP_RANDOM_BASE64: if ( event_pressed ){
      tap_random_base64();
    } break;
  }

  dprintf( "return MACRO_NONE\n" );
  return MACRO_NONE;
}

#define PROCESS_OVERRIDE_BEHAVIOR   false
#define PROCESS_USUAL_BEHAVIOR      true
#define IF_PRESSED                  if (record->event.pressed)
static void process_undo(void);
static void process_cut(void);
static void process_copy(void);
static void process_paste(void);
static void process_redo(void);
static void process_delete_forward_word(void);
static void process_delete_backward_word(void);
static void process_SWITCH_EDIT_LAYER(void);
static void store_mods(void);
static void restore_mods(void);

bool
process_record_user(uint16_t keycode, keyrecord_t *record)
{
  switch (keycode) {
    case UNDO: IF_PRESSED {
      process_undo();
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case CUT: IF_PRESSED {
      process_cut();
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case COPY: IF_PRESSED {
      process_copy();
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case PASTE: IF_PRESSED {
      process_paste();
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case REDO: IF_PRESSED {
      process_redo();
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case DELETE_FORWARD_WORD: IF_PRESSED {
      process_delete_forward_word();
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case DELETE_BACKWARD_WORD: IF_PRESSED {
      process_delete_backward_word();
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case SWITCH_EDIT_LAYER: IF_PRESSED {
      process_SWITCH_EDIT_LAYER();
    } break;
  }

  return PROCESS_USUAL_BEHAVIOR;
}

// rules.mk: TAP_DANCE_ENABLE = yes
#ifdef TAP_DANCE_ENABLE
void tap_dance_select(qk_tap_dance_state_t *state, void *user_data);
qk_tap_dance_action_t tap_dance_actions[] =  {
  [TC_SELECT] = ACTION_TAP_DANCE_FN(tap_dance_select),
};

void tap_dance_select(qk_tap_dance_state_t *state, void *user_data)
{
  if (state->count == 1) {
    // switch shift mod
    if (get_mods() & TAP_DANCE_MOD_SFT) {
      del_mods(TAP_DANCE_MOD_SFT);
    }
    else {
      add_mods(TAP_DANCE_MOD_SFT);
    }
  }
  else if (state->count == 2) {
    // select word
    register_code(KC_LCTL);
    tap_code(KC_RIGHT);
    register_code(KC_LSFT);
    tap_code(KC_LEFT);
    unregister_code(KC_LSFT);
    unregister_code(KC_LCTL);
  }
  else {
    // ignore
  }
}
#endif // TAP_DANCE_ENABLE

static void
process_undo(void)
{
  store_mods();

  register_code(KC_LCTL);
  tap_code(KC_Z);
  unregister_code(KC_LCTL);

  restore_mods();
}

static void
process_cut(void)
{
  store_mods();

  register_code(KC_LCTL);
  tap_code(KC_X);
  unregister_code(KC_LCTL);

  restore_mods();
}

static void
process_copy(void)
{
  store_mods();

  register_code(KC_LCTL);
  tap_code(KC_C);
  unregister_code(KC_LCTL);

  restore_mods();
}

static void
process_paste(void)
{
  store_mods();

  register_code(KC_LCTL);
  tap_code(KC_V);
  unregister_code(KC_LCTL);

  restore_mods();
}

static void
process_redo(void)
{
  store_mods();

  register_code(KC_LCTL);
  tap_code(KC_Y);
  unregister_code(KC_LCTL);

  restore_mods();
}

static void
process_delete_forward_word(void)
{
  store_mods();

  register_code(KC_LCTL);
  register_code(KC_LSFT);
  tap_code(KC_RGHT);
  unregister_code(KC_LSFT);
  unregister_code(KC_LCTL);
  tap_code(KC_DEL);

  restore_mods();
}

static void
process_delete_backward_word(void)
{
  store_mods();

  register_code(KC_LCTL);
  register_code(KC_LSFT);
  tap_code(KC_LEFT);
  unregister_code(KC_LSFT);
  unregister_code(KC_LCTL);
  tap_code(KC_DEL);

  restore_mods();
}

static void
process_SWITCH_EDIT_LAYER(void)
{
  enum keymap_layer top_layer = biton32(layer_state);
  layer_move( KL_(EDIT_CRSR) );
  switch (top_layer){
    case KL_EDIT_CRSR:  layer_on(KL_EDIT_SCRL);   break;
    case KL_EDIT_SCRL:  layer_on(KL_EDIT_MEDIA);  break;
    case KL_EDIT_MEDIA: layer_on(KL_EDIT_SCRL);   break;
    default: break;
  }
}

static uint8_t stored_mods;
static void store_mods(void)
{
  stored_mods = get_mods();
  clear_mods();
}

static void restore_mods(void)
{
  set_mods(stored_mods);
}

static uint16_t
getKeycode_fromDefaultLayer( keyrecord_t *record )
{
  uint32_t layer_state_mem = layer_state;
  layer_state = 0;
  uint8_t layer = layer_switch_get_layer( record->event.key );
  uint16_t keycode = keymap_key_to_keycode( layer, record->event.key );
  layer_state = layer_state_mem;

  return keycode;
}

// Estimate brackets type frome current event.key
static const macro_t*
getMacro_pairedBrankets( keyrecord_t *record, uint16_t keycode, bool isShifted )
{
  // Select macro sequence.
  switch ( keycode ){
#   ifdef KP_L_BRACKET_S
    case KP_L_BRACKET_S: {
      if ( isShifted ){
        return MACRODOWN( D(LSFT),
                            TYPE(KP_L_BRACKET_S),
                            TYPE(KP_R_BRACKET_S),
                            U(LSFT),
                          T(LEFT), END );
      }
      else {
        return MACRODOWN( TYPE(KP_L_BRACKET_S),
                          TYPE(KP_R_BRACKET_S),
                          T(LEFT), END );
      }
    } break;
#   endif

#   ifdef KP__L_RBRACKET
    case KP__L_RBRACKET: {
      return MACRODOWN( D(LSFT),
                          TYPE(KP__L_RBRACKET),
                          TYPE(KP__R_RBRACKET),
                          U(LSFT),
                        T(LEFT), END );
    } break;
#   endif

#   ifdef KP__L_ABRANKETS
    case KP__L_ABRANKETS: {
      return MACRODOWN( D(LSFT),
                          TYPE(KP__L_ABRANKETS),
                          TYPE(KP__R_ABRANKETS),
                          U(LSFT),
                        T(LEFT), END );
    } break;
#   endif

#   ifdef KP__SGLQUOTE
    case KP__SGLQUOTE: {
      return MACRODOWN( D(LSFT),
                          TYPE(KP__SGLQUOTE),
                          TYPE(KP__SGLQUOTE),
                          U(LSFT),
                        T(LEFT), END );
    } break;
#   endif

#   ifdef KP_SGLQUOTE_S
    case KP_SGLQUOTE_S: {
      if ( isShifted ){
        return MACRODOWN( D(LSFT),
                            TYPE(KP_SGLQUOTE_S),
                            TYPE(KP_SGLQUOTE_S),
                            U(LSFT),
                          T(LEFT), END );
      }
      else {
        return MACRODOWN( TYPE(KP_SGLQUOTE_S),
                          TYPE(KP_SGLQUOTE_S),
                          T(LEFT), END );
      }

    } break;
#   endif

#   ifdef KP__DBLQUOTE
    case KP__DBLQUOTE: {
      return MACRODOWN( D(LSFT),
                          TYPE(KP__DBLQUOTE),
                          TYPE(KP__DBLQUOTE),
                          U(LSFT),
                        T(LEFT), END );
    } break;
#   endif
  }

  return MACRO_NONE;
}

static void
action_displaySettings( void )
{
  // display default_layer_state
  uint32_t state = default_layer_state;
  SEND_STRING( "default_layer_state:\n" );
  if ( state == 0 ){
    SEND_STRING( "  fall back to " );
    send_string_P( layerNameStr_P(0) );
    SEND_STRING( "\n" );
  }
  else {
    for ( int i = 0; i < KL_NUM; i++ ){
      if ( state & (1U<<i) ){
        SEND_STRING( "  +" );
        send_string_P( layerNameStr_P(i) );
        SEND_STRING( "\n" );
      }
    }
  }
}

static void
toggle_default_layer(enum keymap_layer layer)
{
  default_layer_xor( 1UL << layer );
}

static const char*
layerNameStr_P( enum keymap_layer layer )
{
# define DEFINE_STR_ITEM( name )  STR_##name[] PROGMEM = #name
# define SET_LAYER_NAMES_LUT_ITEM( name )  [KL_(name)] = STR_##name
  static const char
    LAYER_NAMES_EVAL( DEFINE_STR_ITEM ),
    * const layer_names_lut[KL_NUM] = { LAYER_NAMES_EVAL( SET_LAYER_NAMES_LUT_ITEM ) };

  return (layer_names_lut[layer]);
}

static struct for_mods
get_stMods( void )
{
  return (struct for_mods){ .real = get_mods(),
                            .weak = get_weak_mods(),
                            .macro = get_macro_mods(),
                            .oneshot = get_oneshot_mods() };
}

static mod_bits_t
get_orMods( void )
{
  struct for_mods mods = get_stMods();
  return (  mods.real
            | mods.weak
            | mods.macro
            | mods.oneshot );
}

static void
set_stMods( struct for_mods mods )
{
  set_mods( mods.real );
  set_weak_mods( mods.weak );
  set_macro_mods( mods.macro );
  set_oneshot_mods( mods.oneshot );
}

static void
clear_allMods( void )
{
  clear_mods();
  clear_weak_mods();
  clear_macro_mods();
  clear_oneshot_mods();
}
