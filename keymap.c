#include "hhkb.h"
#include "config.h"
#include "action_macro.h"

// Keycodes definition
#define _______                 ( KC_TRNS )
#define KC_LBRACKET_JP          ( KC_RBRACKET )
#define KC_RBRACKET_JP          ( KC_BSLASH )
#define KC__DBLQUOTE_JP         ( KC_2 )
#define KC__SGLQUOTE_JP         ( KC_7 )
#define KC__LRBRACKET_JP        ( KC_8 )
#define KC__RRBRACKET_JP        ( KC_9 )

// Fn keys definition.
//    fn_actions[FN2IDX(...)]
#define FN_TURN_OFF_LYR_EDIT    ( KC_FN0 )
#define FN_LYR_EDIT_TAP_KEY     ( KC_FN1 )
#define FN_LYR_INP_TAP_KEY      ( KC_FN2 )
#define FN2IDX( x )             ( x - KC_FN0 )

// User macro identifer
//    M(...)
enum user_macro_id{
  UMI_NOTHING = 0,
  UMI_SANDS,
  UMI_SWITCH_EDIT_CRSR_OSKEY,
  UMI_SWITCH_INPUT_OSKEY,
  UMI_DELETE_FORWARD_WORD,
  UMI_DELETE_BACKWARD_WORD,
  UMI_SELECT_WORD,
  UMI_PAIRED_ROUND_BRANCKETS,   // type ()
  UMI_PAIRED_SQUARE_BRANCKETS,  // type []
  UMI_PAIRED_ANGLE_BRANKETS,    // type <>
  UMI_PAIRED_DOUBLE_QUOTATION,  // type ""
  UMI_PAIRED_SINGLE_QUOTATION,  // type ''
  UMI_TOGGLE_PLN_DVORAK,
  UMI_TOGGLE_MOD_ARROW,
  UMI_TOGGLE_MOD_SANDS,
};

// This indicates which part of 32bit layer state(0-7)
//    ACTION_LAYER_BIT_AND( ..., , )
enum part_of_layer{
  POL_PLAIN = 0,
  POL_MOD,
  POL_EDIT,
  POL_INPUT,
  POL_SETTING,
  //LYR_ = ...7,
};
#define PART_OF( part, lyr, ... )   lyr = 4 * part, ##__VA_ARGS__
#define LAYER_MASK_OF_EDIT          ( 0xFUL << (4 * POL_EDIT) )
#define LAYER_MASK_OF_INPUT         ( 0xFUL << (4 * POL_INPUT) )

// Keymap layer specifier
//    keymaps[...]
enum keymap_layer{
  PART_OF( POL_PLAIN    // Plain keys
    ,KL_PLN_QWERTY      //    : QWERTY
    ,KL_PLN_DVORAK      //    : DVORAK
  ),
  PART_OF( POL_MOD      // Modifier
    ,KL_MOD_FN          //    : Fn keys
    ,KL_MOD_RSIDE       //    : Right side modifier
    ,KL_MOD_SANDS       //    : Space and Shift
  ),
  PART_OF( POL_EDIT     // Editing
    ,KL_EDIT_CRSR       //    : Cursor
    ,KL_EDIT_SLCT       //    : Select
    ,KL_EDIT_SCRL       //    : Scroll
    ,KL_EDIT_MEDIA      //    : Media operation
  ),
  PART_OF( POL_INPUT    // Input
    ,KL_INPUT           //    : 
  ),
  PART_OF( POL_SETTING  // Setting switch
    ,KL_STNG            //    : 
  ),
};
static void toggle_default_layer( enum keymap_layer layer );

struct for_mods {
  uint8_t real, weak, macro;
};
static struct for_mods storeMods( void );
static void restoreMods( struct for_mods mods );
static uint8_t allMods( struct for_mods mods );
static void clearAllMods( void );

static const uint8_t SHIFT_MASK = MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT);

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
# define M_SECO   ( M(UMI_SWITCH_EDIT_CRSR_OSKEY) )
# define KC_SECO  ( KC_MHEN )
# define M_SINO   ( M(UMI_SWITCH_INPUT_OSKEY) )
# define KC_SINO  ( KC_HENK )
# define MO_MF    ( MO(KL_MOD_FN) )
  [KL_PLN_QWERTY] = KEYMAP_JP(
     KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,     KC_8,    KC_9,    KC_0, KC_MINS,  KC_EQL, KC_JYEN, KC_BSPC,
     KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,     KC_I,    KC_O,    KC_P, KC_LBRC, KC_RBRC,
    KC_LCTL,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,     KC_K,    KC_L, KC_SCLN, KC_QUOT, KC_BSLS,  KC_ENT,
    KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,  KC_COMM,  KC_DOT, KC_SLSH,   KC_RO,   KC_UP, KC_RSFT,
      MO_MF, KC_ZKHK, KC_LGUI, KC_LALT,  M_SECO,      KC_SPC     ,  M_SINO,  KC_KANA, KC_RALT,   MO_MF, KC_LEFT, KC_DOWN, KC_RGHT 
  ),
  [KL_PLN_DVORAK] = KEYMAP_JP(
    _______,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,     KC_8,    KC_9,    KC_0, KC_LBRC, KC_RBRC,  KC_GRV, _______,
    _______, KC_QUOT, KC_COMM,  KC_DOT,    KC_P,    KC_Y,    KC_F,    KC_G,     KC_C,    KC_R,    KC_L, KC_SLSH,  KC_EQL,
    _______,    KC_A,    KC_O,    KC_E,    KC_U,    KC_I,    KC_D,    KC_H,     KC_T,    KC_N,    KC_S, KC_MINS, _______, _______,
    _______, KC_SCLN,    KC_Q,    KC_J,    KC_K,    KC_X,    KC_B,    KC_M,     KC_W,    KC_V,    KC_Z, KC_JYEN, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______,  _______, _______, _______, _______, _______, _______ 
  ),
  [KL_MOD_FN] = KEYMAP_JP(
    KC_SLEP,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,  KC_INS,  KC_DEL,
    KC_CAPS, _______, _______, _______, _______, _______, _______, _______, KC_PSCR, KC_SLCK, KC_PAUS,   KC_UP, _______,
    _______, KC_VOLD, KC_VOLU, KC_MUTE, KC_EJCT, _______, KC_PAST, KC_PSLS, KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT, _______, KC_PENT,
    _______, _______, _______, _______, _______, _______, KC_PPLS, KC_PMNS,  KC_END, KC_PGDN, KC_DOWN, _______, KC_RSFT, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______,  KC_DEL, KC_RGUI, KC_RCTL 
  ),
  [KL_MOD_RSIDE] = KEYMAP_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_RSFT, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______,  KC_DEL, KC_RGUI, KC_RCTL 
  ),
# define M_SAS   ( M(UMI_SANDS) )
  [KL_MOD_SANDS] = KEYMAP_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,       M_SAS     , _______, _______, _______, _______, _______, _______, _______ 
  ),
# define KC_TOP   ( LCTL(KC_HOME) )   // move to top
# define KC_BTTM  ( LCTL(KC_END) )    // move to bottom
# define KC_MBW   ( LCTL(KC_LEFT) )   // move to backward-word
# define KC_MFW   ( LCTL(KC_RGHT) )   // move to forward-word
# define KC_UNDO  ( LCTL(KC_Z) )
# define KC_CUT   ( LCTL(KC_X) )
# define KC_COPY  ( LCTL(KC_C) )
# define KC_PST   ( LCTL(KC_V) )
# define KC_REDO  ( LCTL(KC_Y) )
# define M_DFW    ( M(UMI_DELETE_FORWARD_WORD) )
# define M_DBW    ( M(UMI_DELETE_BACKWARD_WORD) )
# define TG_EDSC  ( TG(KL_EDIT_SCRL) )
# define TG_EDME  ( TG(KL_EDIT_MEDIA) )
# define MO_EDSL  ( MO(KL_EDIT_SLCT) )   // NOTE: MO() can switch in the shortest time
# define OSL_STG  ( OSL(KL_STNG) )
# define OSM_CTL  ( OSM(MOD_LCTL) )
# define OSM_ALT  ( OSM(MOD_LALT) )
# define OSM_SFT  ( OSM(MOD_LSFT) )
# define OSM_GUI  ( OSM(MOD_LGUI) )
  [KL_EDIT_CRSR] = KEYMAP_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, TG_EDME,  KC_TOP, KC_BTTM, TG_EDSC, _______, KC_REDO, MO_EDSL, KC_HOME,  KC_END, _______, _______, _______,
    OSM_CTL, KC_PGUP,   KC_UP, KC_DOWN, KC_PGDN,  KC_DEL, KC_BSPC,  KC_MBW, KC_LEFT, KC_RGHT,  KC_MFW, _______, _______, _______,
    OSM_SFT, KC_UNDO,  KC_CUT, KC_COPY,  KC_PST, _______, _______,  KC_ENT,   M_DBW,   M_DFW, _______, _______, OSM_SFT, OSM_SFT,
    _______, _______, OSM_GUI, OSM_ALT, _______,     _______     , OSL_STG,  KC_APP, OSM_ALT, _______,   KC_NO, OSM_GUI, OSM_CTL 
  ),
# define S_TOP    ( S(LCTL(KC_HOME)) )
# define S_BTTM   ( S(LCTL(KC_END)) )
# define S_PGUP   ( S(KC_PGUP) )
# define S_UP     ( S(KC_UP) )
# define S_DOWN   ( S(KC_DOWN) )
# define S_PGDN   ( S(KC_PGDOWN) )
# define S_HOME   ( S(KC_HOME) )
# define S_END    ( S(KC_END) )
# define S_BKWD   ( S(LCTL(KC_LEFT)) )
# define S_LEFT   ( S(KC_LEFT) )
# define S_RGHT   ( S(KC_RIGHT) )
# define S_FDWD   ( S(LCTL(KC_RIGHT)) )
# define M_SW     ( M(UMI_SELECT_WORD) )
  [KL_EDIT_SLCT] = KEYMAP_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______,   S_TOP,  S_BTTM, _______, _______, _______,    M_SW,  S_HOME,   S_END, _______, _______, _______,
    _______,  S_PGUP,    S_UP,  S_DOWN,  S_PGDN, _______, _______,  S_BKWD,  S_LEFT,  S_RGHT,  S_FDWD, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______, _______, _______, _______ 
  ),
# define KC_PRVS  ( LALT(KC_LEFT) )   // previous location
# define KC_NEXT  ( LALT(KC_RIGHT) )  // next location
# define KC_SLU   ( LCTL(KC_UP) )     // scroll line up
# define KC_SLD   ( LCTL(KC_DOWN) )   // scroll line down
# define KC_SPU   ( LALT(KC_PGUP) )   // scroll page up
# define KC_SPD   ( LALT(KC_PGDOWN) ) // scroll page down
  [KL_EDIT_SCRL] = KEYMAP_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, KC_PRVS, KC_NEXT, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  KC_SPU,  KC_SLU,  KC_SLD,  KC_SPD, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______, _______, _______, _______ 
  ),
  [KL_EDIT_MEDIA] = KEYMAP_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, KC_MPRV, KC_MNXT, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, KC_MRWD, KC_MSTP, KC_MPLY, KC_MFFD, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, KC_VOLD, KC_VOLU, KC_MUTE, KC_EJCT, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______, _______, _______, _______ 
  ),
# define M_PRB    ( M(UMI_PAIRED_ROUND_BRANCKETS) )
# define M_PSB    ( M(UMI_PAIRED_SQUARE_BRANCKETS) )
# define M_PAB    ( M(UMI_PAIRED_ANGLE_BRANKETS) )
# define M_PDQ    ( M(UMI_PAIRED_DOUBLE_QUOTATION) )
# define M_PSQ    ( M(UMI_PAIRED_SINGLE_QUOTATION) )
  [KL_INPUT] = KEYMAP_JP(
    _______, _______,   M_PDQ, _______, _______, _______, _______,   M_PSQ,   M_PRB, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, KC_PSCR, KC_SLCK, KC_PAUS,  KC_INS,  M_PSB,
    _______,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______,   M_PAB, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, OSL_STG,     _______     , _______, _______, _______, _______, _______, _______, _______ 
  ),
# define M_TPD    ( M(UMI_TOGGLE_PLN_DVORAK) )
# define M_TMA    ( M(UMI_TOGGLE_MOD_ARROW) )
# define M_TMS    ( M(UMI_TOGGLE_MOD_SANDS) )
  [KL_STNG] = KEYMAP_JP(
    _______,   M_TPD, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   M_TMA, _______,
    _______, _______, _______, _______, _______,       M_TMS     , _______, _______, _______, _______,   M_TMA,   M_TMA,   M_TMA 
  ),
};

const uint16_t PROGMEM fn_actions[] = {
};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t macro_id, uint8_t opt) 
{
  switch (macro_id) {
    case UMI_DELETE_FORWARD_WORD: {
      return MACRODOWN( D(LCTL),
                          D(LSFT), T(RGHT),  U(LSFT),
                          T(X), 
                          U(LCTL), END );
    } break;

    case UMI_DELETE_BACKWARD_WORD: {
      return MACRODOWN( D(LCTL),
                          D(LSFT), T(LEFT),  U(LSFT),
                          T(X), 
                          U(LCTL), END );
    } break;

    case UMI_SANDS: {
      return MACRO_ONESHOT_MODIFIER( MOD_LSFT, KC_SPACE );
    } break;

    case UMI_SWITCH_EDIT_CRSR_OSKEY: {
      return MACRO_ONESHOT( ( layer_on(KL_EDIT_CRSR),         MACRO_NONE ),
                            ( layer_and(~LAYER_MASK_OF_EDIT), MACRO(TYPE(KC_SECO), END) ),
                            ( layer_and(~LAYER_MASK_OF_EDIT), MACRO_NONE ));
    } break;

    case UMI_SWITCH_INPUT_OSKEY: {
      return MACRO_ONESHOT( ( layer_on(KL_INPUT),              MACRO_NONE ),
                            ( layer_and(~LAYER_MASK_OF_INPUT), MACRO(TYPE(KC_SINO), END) ),
                            ( layer_and(~LAYER_MASK_OF_INPUT), MACRO_NONE ));
    } break;

    case UMI_SELECT_WORD: {
      return MACRODOWN( D(LCTL), 
                          T(RGHT), 
                          D(LSFT), T(LEFT), U(LSFT), 
                          U(LCTL), END );
    } break;

    case UMI_PAIRED_ROUND_BRANCKETS: {
      return MACRODOWN( D(LSFT), 
                          T(_LRBRACKET_JP), 
                          T(_RRBRACKET_JP), 
                          U(LSFT), 
                        T(LEFT), END );
    } break;

    case UMI_PAIRED_SQUARE_BRANCKETS: {
      static struct for_mods mods;
      if ( record->event.pressed ){
        mods = storeMods();
        clear_mods();
        clear_weak_mods();
        clear_macro_mods();
      } else {
        restoreMods( mods );
      }

      uint8_t isShifted = allMods( mods ) & SHIFT_MASK;
      return (isShifted)
                ? MACRODOWN(  D(LSHIFT),
                                T(LBRACKET_JP), 
                                T(RBRACKET_JP),
                                U(LSHIFT),
                              T(LEFT), END )
                : MACRODOWN(  T(LBRACKET_JP), 
                              T(RBRACKET_JP), 
                              T(LEFT), END );
    } break;

    case UMI_PAIRED_ANGLE_BRANKETS: {
      return MACRODOWN( D(LSFT), 
                          T(COMMA), 
                          T(DOT), 
                          U(LSFT), 
                        T(LEFT), END );
    } break;

    case UMI_PAIRED_DOUBLE_QUOTATION: {
      return MACRODOWN( D(LSFT), 
                          T(_DBLQUOTE_JP), 
                          T(_DBLQUOTE_JP), 
                          U(LSFT), 
                        T(LEFT), END );
    } break;

    case UMI_PAIRED_SINGLE_QUOTATION: {
      return MACRODOWN( D(LSFT), 
                          T(_SGLQUOTE_JP), 
                          T(_SGLQUOTE_JP), 
                          U(LSFT), 
                        T(LEFT), END );
    } break;

    case UMI_TOGGLE_PLN_DVORAK: {
      if ( !(record->event.pressed) ){
        toggle_default_layer( KL_PLN_DVORAK );
        //eeconfig_update_default_layer( default_layer_state );
      }
    } break;

    case UMI_TOGGLE_MOD_SANDS: {
      if ( !(record->event.pressed) ){
        toggle_default_layer( KL_MOD_SANDS );
        //eeconfig_update_default_layer( default_layer_state );
      }
    } break;

    case UMI_TOGGLE_MOD_ARROW: {
      if ( !(record->event.pressed) ){
        toggle_default_layer( KL_MOD_RSIDE );
        //eeconfig_update_default_layer( default_layer_state );
      }
    } break;
  }

  return MACRO_NONE;
}

static void 
toggle_default_layer(enum keymap_layer layer)
{
  default_layer_xor( 1UL << layer );
  layer_state = default_layer_state;
}

static struct for_mods 
storeMods( void )
{
  return (struct for_mods){ .real = get_mods(),
                            .weak = get_weak_mods(),
                            .macro = get_macro_mods() };
}

static void 
restoreMods( struct for_mods mods )
{
  set_mods( mods.real );
  set_weak_mods( mods.weak );
  set_macro_mods( mods.macro );
}

static uint8_t 
allMods( struct for_mods mods )
{
  return ( mods.real | mods.weak | mods.macro );
}

static void 
clearAllMods( void )
{
  clear_mods();
  clear_weak_mods();
  clear_macro_mods();
  clear_oneshot_mods();
}

