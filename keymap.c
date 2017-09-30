#include "hhkb.h"
#include "config.h"
#include "action_macro.h"

#ifdef JIS_KEYCODE
  // JIS keyboard layout
# define KC_LBRACKET_KL         ( KC_RBRACKET )
# define KC_RBRACKET_KL         ( KC_BSLASH )
# define KC__LR_BRACKET_KL      ( KC_8 )
# define KC__RR_BRACKET_KL      ( KC_9 )
# define KC__LA_BRANKETS_KL     ( KC_COMMA )
# define KC__RA_BRANKETS_KL     ( KC_DOT )
# define KC__SGLQUOTE_KL        ( KC_7 )
# define KC__DBLQUOTE_KL        ( KC_2 )
#else
  // ISO keyboard layout
# define KC_LBRACKET_KL         ( KC_LBRACKET )
# define KC_RBRACKET_KL         ( KC_RBRACKET )
# define KC__LR_BRACKET_KL      ( KC_9 )
# define KC__RR_BRACKET_KL      ( KC_0 )
# define KC__LA_BRANKETS_KL     ( KC_COMMA )
# define KC__RA_BRANKETS_KL     ( KC_DOT )
# define KC_SGLQUOTE_KL         ( KC_QUOT )
#endif

// Fn keys definition.
//    fn_actions[FN2IDX(...)]
#define FN2IDX( x )             ( x - KC_FN0 )

// User macro identifer
//    M(...)
enum user_macro{
  UM_NOTHING = 0,
  UM_SANDS,
  UM_SWITCH_EDIT_LAYER,
  UM_SWITCH_INPUT_LAYER,
  UM_TURN_EDIT_LAYER,
  UM_TURN_INPUT_LAYER,
  UM_DELETE_FORWARD_WORD,
  UM_DELETE_BACKWARD_WORD,
  UM_SELECT_WORD,
  UM_PAIRED_BRANKETS,
  UM_TOGGLE_PLN_DVORAK,
  UM_TOGGLE_MOD_ARROW,
  UM_TOGGLE_MOD_SANDS,
  UM_DISPLAY_SETTINGS,
  UM_1_B,
  UM_2_C,
  UM_3_D,
  UM_4_E,
  UM_5_F,
  UM_6_,
  UM_7_,
  UM_8_,
  UM_9_,
  UM_0_A,
};
#define UM( id )  ( M(UM_##id) )
static void action_displaySettings( void );

#define LAYER_NAMES_EVAL( macro ) \
  macro(PLN_QWERTY), \
  macro(PLN_DVORAK), \
  macro(MOD_FN),     \
  macro(MOD_RSIDE),  \
  macro(MOD_SANDS),  \
  macro(EDIT_CRSR),  \
  macro(EDIT_SLCT),  \
  macro(EDIT_SCRL),  \
  macro(EDIT_MEDIA), \
  macro(INPUT),      \
  macro(STNG)

// keymap layer specifier
//    keymaps[...]
#define KL_( name )     KL_##name
enum keymap_layer{
  LAYER_NAMES_EVAL( KL_ ),
  KL_NUM
};
#define LAYER_MASK_OF_EDIT          ( 1UL << KL_(EDIT_CRSR)   \
                                      | 1UL << KL_(EDIT_SLCT) \
                                      | 1UL << KL_(EDIT_SCRL) \
                                      | 1UL << KL_(EDIT_MEDIA) )
#define LAYER_MASK_OF_INPUT         ( 1UL << KL_(INPUT) )
#define LAYER_MASK_OF_STNG          ( 1UL << KL_(STNG) )
static void toggle_default_layer( enum keymap_layer layer );

// keymap layer name strings
#define STR_( name )      STR_##name
#define STR_DEFINE( name )  STR_(name)[] PROGMEM = #name
#define LAYER_NAMES_LUT_ITEM( name )  [KL_(name)] = STR_(name)
static const char 
  LAYER_NAMES_EVAL( STR_DEFINE ),
  * const layer_names_lut[] = { LAYER_NAMES_EVAL( LAYER_NAMES_LUT_ITEM ) };

struct for_mods {
  uint8_t real, weak, macro, oneshot;
};
static struct for_mods storeMods( void );
static void restoreMods( struct for_mods mods );
static uint8_t anyMods( struct for_mods mods );
static void clearAllMods( void );

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
# define M_MHEN   ( MACROTAP(UM_SWITCH_EDIT_LAYER) )
# define M_HENK   ( MACROTAP(UM_SWITCH_INPUT_LAYER) )
# define KC_SECO  ( KC_MHEN )
# define KC_SINO  ( KC_HENK )
# define MO_MF    ( MO(KL_MOD_FN) )
  [KL_(PLN_QWERTY)] = KEYMAP_JP(
     KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,     KC_8,    KC_9,    KC_0, KC_MINS,  KC_EQL, KC_JYEN, KC_BSPC,
     KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,     KC_I,    KC_O,    KC_P, KC_LBRC, KC_RBRC,
    KC_LCTL,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,     KC_K,    KC_L, KC_SCLN, KC_QUOT, KC_BSLS,  KC_ENT,
    KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,  KC_COMM,  KC_DOT, KC_SLSH,   KC_RO,   KC_UP, KC_RSFT,
      MO_MF, KC_ZKHK, KC_LGUI, KC_LALT,  M_MHEN,      KC_SPC     ,  M_HENK,  KC_KANA, KC_RALT,   MO_MF, KC_LEFT, KC_DOWN, KC_RGHT 
  ),
  [KL_(PLN_DVORAK)] = KEYMAP_JP(
    _______,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,     KC_8,    KC_9,    KC_0, KC_LBRC, KC_RBRC,  KC_GRV, _______,
    _______, KC_QUOT, KC_COMM,  KC_DOT,    KC_P,    KC_Y,    KC_F,    KC_G,     KC_C,    KC_R,    KC_L, KC_SLSH,  KC_EQL,
    _______,    KC_A,    KC_O,    KC_E,    KC_U,    KC_I,    KC_D,    KC_H,     KC_T,    KC_N,    KC_S, KC_MINS, _______, _______,
    _______, KC_SCLN,    KC_Q,    KC_J,    KC_K,    KC_X,    KC_B,    KC_M,     KC_W,    KC_V,    KC_Z, KC_JYEN, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______,  _______, _______, _______, _______, _______, _______ 
  ),
  [KL_(MOD_FN)] = KEYMAP_JP(
    KC_SLEP,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,  KC_INS,  KC_DEL,
    KC_CAPS, _______, _______, _______, _______, _______, _______, _______, KC_PSCR, KC_SLCK, KC_PAUS,   KC_UP, _______,
    _______, KC_VOLD, KC_VOLU, KC_MUTE, KC_EJCT, _______, KC_PAST, KC_PSLS, KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT, _______, KC_PENT,
    _______, _______, _______, _______, _______, _______, KC_PPLS, KC_PMNS,  KC_END, KC_PGDN, KC_DOWN, _______, KC_RSFT, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______,  KC_APP, KC_RGUI, KC_RCTL 
  ),
  [KL_(MOD_RSIDE)] = KEYMAP_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_RSFT, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______,  KC_APP, KC_RGUI, KC_RCTL 
  ),
//# define M_SAS   ( MACROTAP(UM_SANDS) )
# define M_SAS    ( MT(MOD_LSFT, KC_SPACE) )
  [KL_(MOD_SANDS)] = KEYMAP_JP(
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
# define M_DFW    ( M(UM_DELETE_FORWARD_WORD) )
# define M_DBW    ( M(UM_DELETE_BACKWARD_WORD) )
# define M_TEL    ( M(UM_TURN_EDIT_LAYER) )
# define MO_EDSL  ( MO(KL_(EDIT_SLCT)) )   // NOTE: MO() can switch in the shortest time
# define MO_STNG  ( MO(KL_(STNG)) )
# define OSM_CTL  ( OSM(MOD_LCTL) )
# define OSM_ALT  ( OSM(MOD_LALT) )
# define OSM_SFT  ( OSM(MOD_LSFT) )
# define OSM_GUI  ( OSM(MOD_LGUI) )
  [KL_(EDIT_CRSR)] = KEYMAP_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  KC_ESC,  KC_TOP, KC_BTTM,   M_TEL, _______, KC_REDO, MO_EDSL, KC_HOME,  KC_END, _______, _______, _______,
    OSM_CTL, KC_PGUP,   KC_UP, KC_DOWN, KC_PGDN,  KC_DEL, KC_BSPC,  KC_MBW, KC_LEFT, KC_RGHT,  KC_MFW, _______, _______, _______,
    OSM_SFT, KC_UNDO,  KC_CUT, KC_COPY,  KC_PST, _______, _______,  KC_ENT,   M_DBW,   M_DFW, _______, _______, OSM_SFT, OSM_SFT,
    _______, _______, OSM_GUI, OSM_ALT, _______,     _______     , MO_STNG,  KC_APP, OSM_ALT, _______,   KC_NO, OSM_GUI, OSM_CTL 
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
# define M_SW     ( M(UM_SELECT_WORD) )
  [KL_(EDIT_SLCT)] = KEYMAP_JP(
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
  [KL_(EDIT_SCRL)] = KEYMAP_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, KC_PRVS, KC_NEXT, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  KC_SPU,  KC_SLU,  KC_SLD,  KC_SPD, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______, _______, _______, _______ 
  ),
  [KL_(EDIT_MEDIA)] = KEYMAP_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, KC_MPRV, KC_MNXT, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, KC_MRWD, KC_MSTP, KC_MPLY, KC_MFFD, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, KC_VOLD, KC_VOLU, KC_MUTE, KC_EJCT, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______, _______, _______, _______ 
  ),
# define M_PB     ( M(UM_PAIRED_BRANKETS) )
  [KL_(INPUT)] = KEYMAP_JP(
    _______, _______,    M_PB, _______, _______, _______, _______,    M_PB,    M_PB,    M_PB, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, KC_LEAD, KC_PSCR, KC_SLCK, KC_PAUS,    M_PB,    M_PB,
    _______, UM(1_B), UM(2_C), UM(3_D), UM(4_E), UM(5_F),  UM(6_),  UM(7_),  UM(8_),  UM(9_), UM(0_A),    M_PB, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______,    M_PB, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, MO_STNG,     _______     , _______, _______, _______, _______, _______, _______, _______ 
  ),
# define M_TPD    ( M(UM_TOGGLE_PLN_DVORAK) )
# define M_TMA    ( M(UM_TOGGLE_MOD_ARROW) )
# define M_TMS    ( M(UM_TOGGLE_MOD_SANDS) )
# define M_DSST   ( M(UM_DISPLAY_SETTINGS) )
  [KL_(STNG)] = KEYMAP_JP(
    XXXXXXX,   M_TPD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX,  M_DSST, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   M_TMA, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MO_STNG,       M_TMS     , MO_STNG, XXXXXXX, XXXXXXX, XXXXXXX,   M_TMA,   M_TMA,   M_TMA 
  ),
};

const uint16_t PROGMEM fn_actions[] = {
};

struct for_agm_status {
    keyrecord_t *record;
    uint8_t macro_id;
    uint8_t opt;
    struct for_mods mods;
};
typedef const struct for_agm_status AGM_STATUS_T;
static const uint8_t SHIFT_MASK = MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT);
#define IF_PRESSED  if ( record->event.pressed )

static const macro_t* getMacro_pairedBrankets( AGM_STATUS_T* const status );

const macro_t*
action_get_macro(keyrecord_t *record, uint8_t macro_id, uint8_t opt) 
{
  dprintf( "====action_get_macro====\n"
           "record.\n"
           "  e.p. = %u\n"
           "  t.c. = %u\n"
           "  t.i. = %u\n"
           "macro_id = %u\n"
           "opt = %u\n"
           , record->event.pressed
           , record->tap.count
           , record->tap.interrupted
           , macro_id
           , opt );

  AGM_STATUS_T agm_status = {
    .record = record,
    .macro_id = macro_id,
    .opt = opt,
    .mods = storeMods(),
  };

  switch (macro_id) {
    case UM_DELETE_FORWARD_WORD: {
      return MACRODOWN( D(LCTL),
                          D(LSFT), T(RGHT),  U(LSFT),
                          T(X), 
                          U(LCTL), END );
    } break;

    case UM_DELETE_BACKWARD_WORD: {
      return MACRODOWN( D(LCTL),
                          D(LSFT), T(LEFT),  U(LSFT),
                          T(X), 
                          U(LCTL), END );
    } break;

    case UM_SANDS: {
      return MACRO_HOLDMOD_TAPKC( record, MOD_BIT(KC_LSHIFT), KC_SPACE );
    } break;

    case UM_SWITCH_EDIT_LAYER: {
      return MACRO_TAP_HOLD( record,
                /*press*/     ( layer_on(KL_(EDIT_CRSR)),       MACRO_NONE ),
                /*release*/   ( clearAllMods(), 
                                layer_and(~LAYER_MASK_OF_EDIT), MACRO_SEND_NONE ),
                /*tap*/       ( clearAllMods(), 
                                layer_and(~LAYER_MASK_OF_EDIT), MACRO(TYPE(KC_SECO), END) ));
    } break;

    case UM_SWITCH_INPUT_LAYER: {
      return MACRO_TAP_HOLD( record,
                /*press*/     ( layer_on(KL_(INPUT)),            MACRO_NONE ),
                /*release*/   ( clearAllMods(), 
                                layer_and(~LAYER_MASK_OF_INPUT), MACRO_SEND_NONE ),
                /*tap*/       ( clearAllMods(), 
                                layer_and(~LAYER_MASK_OF_INPUT), MACRO(TYPE(KC_SECO), END) ));
    } break;

    case UM_TURN_EDIT_LAYER: IF_PRESSED {
      uint32_t state = layer_state & LAYER_MASK_OF_EDIT;
      enum keymap_layer kl;
      for ( kl = KL_NUM-1; kl > 0; kl-- ){
        if ( state & (1U<<kl) ){ break; }
      }
      layer_and( (~LAYER_MASK_OF_EDIT) | (1U<<KL_(EDIT_CRSR)) );
      switch (kl){
        case KL_(EDIT_CRSR):  layer_on( KL_(EDIT_SCRL) );   break;
        case KL_(EDIT_SLCT):  layer_on( KL_(EDIT_SCRL) );   break;
        case KL_(EDIT_SCRL):  layer_on( KL_(EDIT_MEDIA) );  break;
        case KL_(EDIT_MEDIA): break;  /* layer on KL_(EDIT_SCRL) */
        default:  break;
      }
    } break;

    case UM_TURN_INPUT_LAYER: {} break;

    case UM_SELECT_WORD: {
      return MACRODOWN( D(LCTL), 
                          T(RGHT), 
                          D(LSFT), T(LEFT), U(LSFT), 
                          U(LCTL), END );
    } break;

    case UM_PAIRED_BRANKETS: {
      return getMacro_pairedBrankets( &agm_status );
    } break;

    case UM_TOGGLE_PLN_DVORAK: IF_PRESSED {
      toggle_default_layer( KL_(PLN_DVORAK) );
      //eeconfig_update_default_layer( default_layer_state );
    } break;

    case UM_TOGGLE_MOD_SANDS: IF_PRESSED {
      toggle_default_layer( KL_(MOD_SANDS) );
      //eeconfig_update_default_layer( default_layer_state );
    } break;

    case UM_TOGGLE_MOD_ARROW: IF_PRESSED {
      toggle_default_layer( KL_(MOD_RSIDE) );
      //eeconfig_update_default_layer( default_layer_state );
    } break;
    
    case UM_DISPLAY_SETTINGS: IF_PRESSED {
      action_displaySettings();
    } break;

    case UM_1_B ... UM_0_A: IF_PRESSED {
      uint8_t kc;
      uint8_t isShifted = anyMods( storeMods() ) & SHIFT_MASK;
      if ( isShifted ){
        switch (macro_id){
          case UM_1_B: { kc = KC_B; } break;
          case UM_2_C: { kc = KC_C; } break;
          case UM_3_D: { kc = KC_D; } break;
          case UM_4_E: { kc = KC_E; } break;
          case UM_5_F: { kc = KC_F; } break;
          case UM_0_A: { kc = KC_A; } break;
          default:     { kc = KC_NO; } break;
        }
      }
      else {
        kc = KC_1 + (macro_id - UM_1_B);
      }
      register_code( kc );
      unregister_code( kc );
    } break;
  }

  return MACRO_NONE;
}

static const macro_t* 
getMacro_pairedBrankets( AGM_STATUS_T* const status )
{
  keyrecord_t * const record = status->record;
  const keypos_t key = status->record->event.key;
  const bool pressed = status->record->event.pressed;

  // Get keycode at default-layer.
  uint32_t layer_state_mem = layer_state;
  layer_state = 0;
  uint16_t keycode = keymap_key_to_keycode( layer_switch_get_layer(key), key );
  layer_state = layer_state_mem;

  // Evacuation and restoration mods.
  static struct for_mods mods;
  if ( pressed ){
    mods = status->mods;
    clearAllMods();
  } 
  else {
    restoreMods( mods );
  }
  send_keyboard_report();

  // Select macro sequence.
  bool isShifted = anyMods( status->mods ) & SHIFT_MASK;
  switch ( keycode ){
#   ifdef KC_LBRACKET_KL
    case KC_LBRACKET_KL: {
      if ( isShifted ){
        return MACRODOWN( D(LSFT),
                            T(LBRACKET_KL),
                            T(RBRACKET_KL),
                            U(LSFT),
                          T(LEFT), END );
      }
      else {
        return MACRODOWN( T(LBRACKET_KL),
                          T(RBRACKET_KL),
                          T(LEFT), END );
      }
    } break;
#   endif

#   ifdef KC__LR_BRACKET_KL
    case KC__LR_BRACKET_KL: {
      return MACRODOWN( D(LSFT),
                          T(_LR_BRACKET_KL),
                          T(_RR_BRACKET_KL),
                          U(LSFT),
                        T(LEFT), END );
    } break;
#   endif

#   ifdef KC__LA_BRANKETS_KL
    case KC__LA_BRANKETS_KL: {
      return MACRODOWN( D(LSFT),
                          T(_LA_BRANKETS_KL),
                          T(_RA_BRANKETS_KL),
                          U(LSFT),
                        T(LEFT), END );
    } break;
#   endif

#   ifdef KC_SGLQUOTE_KL
    case KC_SGLQUOTE_KL: {
      if ( isShifted ){
        return MACRODOWN( D(LSFT),
                            T(SGLQUOTE_KL),
                            T(SGLQUOTE_KL),
                            U(LSFT),
                          T(LEFT), END );
      }
      else {
        return MACRODOWN( T(SGLQUOTE_KL),
                          T(SGLQUOTE_KL),
                          T(LEFT), END );
      }
    } break;
#   endif

#   ifdef KC__SGLQUOTE_KL
    case KC__SGLQUOTE_KL: {
      return MACRODOWN( D(LSFT),
                          T(_SGLQUOTE_KL),
                          T(_SGLQUOTE_KL),
                          U(LSFT),
                        T(LEFT), END );
    } break;
#   endif

#   ifdef KC__DBLQUOTE_KL
    case KC__DBLQUOTE_KL: {
      return MACRODOWN( D(LSFT),
                          T(_DBLQUOTE_KL),
                          T(_DBLQUOTE_KL),
                          U(LSFT),
                        T(LEFT), END );
    } break;
#   endif

    default: {
      if ( pressed ){
        register_code( keycode );
      }
      else {
        unregister_code( keycode );
      }
    } break;
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
    send_string_P( layer_names_lut[0] );
    SEND_STRING( "\n" );
  }
  else {
    for ( int i = 0; i < KL_NUM; i++ ){
      if ( state & (1U<<i) ){
        SEND_STRING( "  " );
        send_string_P( layer_names_lut[i] );
        SEND_STRING( " = active\n" );
      }
    }
  }
}

static void 
toggle_default_layer(enum keymap_layer layer)
{
  default_layer_xor( 1UL << layer );
}

static struct for_mods 
storeMods( void )
{
  return (struct for_mods){ .real = get_mods(),
                            .weak = get_weak_mods(),
                            .macro = get_macro_mods(),
                            .oneshot = get_oneshot_mods() };
}

static void 
restoreMods( struct for_mods mods )
{
  set_mods( mods.real );
  set_weak_mods( mods.weak );
  set_macro_mods( mods.macro );
  set_oneshot_mods( mods.oneshot );
}

static uint8_t 
anyMods( struct for_mods mods )
{
  return (  mods.real 
            | mods.weak 
            | mods.macro 
            | mods.oneshot );
}

static void 
clearAllMods( void )
{
  clear_mods();
  clear_weak_mods();
  clear_macro_mods();
  clear_oneshot_mods();
}
