#include "hhkb.h"
#include "config.h"
#include "action_macro.h"
#ifdef JIS_KEYCODE
 #include "sendstring_jis.h"
#endif

enum custom_keycodes {
  CUSTOM_KEYCODE = SAFE_RANGE,
  FN_RSFT,
  FN_APP,
  FN_RGUI,
  FN_RCTL,
  UNDO,
  CUT,
  COPY,
  PASTE,
  REDO,
  DELETE_FORWARD_WORD,
  DELETE_BACKWARD_WORD,
  TOGGLE_US_LIKE,
  TOGGLE_CAPS_LOCK,
  TOGGLE_EDIT_LAYER,
  TOGGLE_INPUT_LAYER,
  SWITCH_EDIT_LAYER,
  TOGGLE_MOD_RSIDE,
  TOGGLE_MOD_SANDS,
  INPUT_PAIRED_BRANKETS,
  CLEAR_DEFAULT_LAYER,
  DISPLAY_SETTINGS,
  TAP_RANDOM_BASE64,
};

enum tap_dance_code {
  TC_SELECT,
};

// custom keycode
#define DEL_FW    DELETE_FORWARD_WORD
#define DEL_BW    DELETE_BACKWARD_WORD
#define TO_USLK   TOGGLE_US_LIKE
#define TO_CPSLK  TOGGLE_CAPS_LOCK
#define TO_EDIT   TOGGLE_EDIT_LAYER
#define TO_INPT   TOGGLE_INPUT_LAYER
#define SW_EDIT   SWITCH_EDIT_LAYER
#define TO_ARRO   TOGGLE_MOD_RSIDE
#define TO_SANDS  TOGGLE_MOD_SANDS
#define INP_BRKS  INPUT_PAIRED_BRANKETS
#define CLR_DLYR  CLEAR_DEFAULT_LAYER
#define DSP_STT   DISPLAY_SETTINGS
#define TAP_RND   TAP_RANDOM_BASE64

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
#define SANDS     MT(MOD_LSFT, KC_SPACE)

// modifier keycode
#define OSM_CTL  OSM(MOD_LCTL)
#define OSM_ALT  OSM(MOD_LALT)
#define OSM_SFT  OSM(MOD_LSFT)
#define OSM_GUI  OSM(MOD_LGUI)

// switching and toggling layers
#define MO_FN     MO(KL_(FN_KEYS))
#define MO_LOWER  MO(KL_EDIT)
#define MO_RAISE  MO(KL_INPUT)

// keymap layer names
#define LAYER_NAMES_EVAL( func ) \
  /* default_layer_state */ \
  func(QWERTY),     \
  func(US_LIKE),    \
  func(CAPS_LOCK),  \
  func(MOD_RSIDE),  \
  func(MOD_SANDS),  \
  /* layer_state */ \
  func(FN_KEYS),    \
  func(EDIT),       \
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
#define EDIT_LAYER_MASK         ( 1UL << KL_(EDIT)      \
                                | 1UL << KL_(EDIT_SCRL) \
                                | 1UL << KL_(EDIT_MEDIA) )
#define INPUT_LAYER_MASK        ( 1UL << KL_(INPUT) )
static void toggle_default_layer( enum keymap_layer layer );
static const char* layerNameStr_P( enum keymap_layer layer );

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [KL_(QWERTY)] = LAYOUT_JP(
     KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,     KC_8,    KC_9,    KC_0, KC_MINS,  KC_EQL, KC_JYEN, KC_BSPC,
     KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,     KC_I,    KC_O,    KC_P, KC_LBRC, KC_RBRC,
    KC_LCTL,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,     KC_K,    KC_L, KC_SCLN, KC_QUOT, KC_BSLS,  KC_ENT,
    KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,  KC_COMM,  KC_DOT, KC_SLSH,   KC_RO,   KC_UP, KC_RSFT,
      MO_FN, KC_ZKHK, KC_LGUI, KC_LALT, TO_EDIT,      KC_SPC     , TO_INPT,  KC_RALT, KC_KANA,   MO_FN, KC_LEFT, KC_DOWN, KC_RGHT
  ),

  [KL_(US_LIKE)] = LAYOUT_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_BSLS, KC_BSPC,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  KC_GRV, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_BSLS, _______, _______,
    _______, _______, KC_LGUI, KC_LALT, _______,     _______     , _______, KC_RALT, KC_RGUI, _______, _______, _______, _______
  ),

  [KL_(CAPS_LOCK)] = LAYOUT_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    KC_CAPS, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    KC_LCTL,   MO_FN, _______, _______, _______,     _______     , _______, _______, _______, _______, _______, _______, _______
  ),

  [KL_(MOD_RSIDE)] = LAYOUT_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_RSFT, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______,  KC_APP, KC_RGUI, KC_RCTL
  ),

  [KL_(MOD_SANDS)] = LAYOUT_JP(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,       SANDS     , _______, _______, _______, _______, _______, _______, _______
  ),

  [KL_(FN_KEYS)] = LAYOUT_JP(
    KC_SLEP,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,  KC_INS,  KC_DEL,
    KC_CAPS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_PSCR, KC_SLCK, KC_PAUS,   KC_UP, XXXXXXX,
    _______, KC_VOLD, KC_VOLU, KC_MUTE, KC_EJCT, XXXXXXX, KC_PAST, KC_PSLS, KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT, XXXXXXX, KC_PENT,
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_PPLS, KC_PMNS,  KC_END, KC_PGDN, KC_DOWN, XXXXXXX, FN_RSFT, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______,  FN_APP, FN_RGUI, FN_RCTL
  ),

  [KL_(EDIT)] = LAYOUT_JP(
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

  [KL_(INPUT)] = LAYOUT_JP(
    _______, XXXXXXX,INP_BRKS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,INP_BRKS,INP_BRKS,INP_BRKS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
    _______, XXXXXXX, XXXXXXX, KC_LEAD, TAP_RND, XXXXXXX, XXXXXXX, KC_NLCK, KC_PSCR, KC_SLCK, KC_PAUS,INP_BRKS,INP_BRKS,
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,INP_BRKS,INP_BRKS, _______,
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,INP_BRKS, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______,
    _______, _______, _______, _______, _______,     _______     , _______, _______, _______, _______, _______, _______, _______
  ),

  [KL_(STNG)] = LAYOUT_JP(
     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,CLR_DLYR, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, TO_USLK, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    TO_CPSLK, XXXXXXX, XXXXXXX, DSP_STT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, TO_ARRO, XXXXXXX,
     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,     TO_SANDS    , _______, XXXXXXX, XXXXXXX, XXXXXXX, TO_ARRO, TO_ARRO, TO_ARRO
  ),

};

uint32_t layer_state_set_user(uint32_t state)
{
  del_mods(TAP_DANCE_MOD_SFT);
  state = update_tri_layer_state(state, KL_(EDIT), KL_(INPUT), KL_(STNG));
  return state;
}

// Modifier utility
typedef uint8_t mod_bits_t;
static const mod_bits_t SHIFT_MOD_BITS = MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT);

// subroutine of action_get_macro
static uint16_t getKeycode_fromDefaultLayer( keyrecord_t *record );

#define PROCESS_OVERRIDE_BEHAVIOR   false
#define PROCESS_USUAL_BEHAVIOR      true
#define IF_PRESSED                  if (record->event.pressed)
#define default_layer_state_cmp(layer)  layer_state_cmp(default_layer_state, (layer))
#define register_code_wait(kc)      register_code(kc), wait_ms(TAP_CODE_DELAY)
#define unregister_code_wait(kc)    unregister_code(kc), wait_ms(TAP_CODE_DELAY)
static void cancel_capsLock(uint16_t keycode);
static void process_undo(void);
static void process_cut(void);
static void process_copy(void);
static void process_paste(void);
static void process_redo(void);
static void process_delete_forward_word(void);
static void process_delete_backward_word(void);
static void process_TOGGLE_EDIT_LAYER(bool is_pressed, bool is_tapped);
static void process_TOGGLE_INPUT_LAYER(bool is_pressed, bool is_tapped);
static void process_SWITCH_EDIT_LAYER(void);
static void process_INPUT_PAIRED_BRANKETS(keyrecord_t *record);
static void action_displaySettings( void );
static void store_mods(void);
static void restore_mods(void);

bool
process_record_user(uint16_t keycode, keyrecord_t *record)
{
  static uint16_t mem_keycode;
  uint16_t prev_keycode = mem_keycode;
  bool is_tapped = ((!record->event.pressed) && (keycode == prev_keycode));
  mem_keycode = keycode;

  cancel_capsLock(keycode);

  switch (keycode) {
    case FN_RSFT: {
      uint16_t kc = default_layer_state_cmp(KL_(MOD_RSIDE)) ? KC_UP : KC_RSFT;
      IF_PRESSED
        register_code(kc);
      else
        unregister_code(kc);
    } return PROCESS_OVERRIDE_BEHAVIOR;

    case FN_APP: {
      uint16_t kc = default_layer_state_cmp(KL_(MOD_RSIDE)) ? KC_LEFT : KC_APP;
      IF_PRESSED
        register_code(kc);
      else
        unregister_code(kc);
    } return PROCESS_OVERRIDE_BEHAVIOR;

    case FN_RGUI: {
      uint16_t kc = default_layer_state_cmp(KL_(MOD_RSIDE)) ? KC_DOWN : KC_RGUI;
      IF_PRESSED
        register_code(kc);
      else
        unregister_code(kc);
    } return PROCESS_OVERRIDE_BEHAVIOR;

    case FN_RCTL: {
      uint16_t kc = default_layer_state_cmp(KL_(MOD_RSIDE)) ? KC_RGHT : KC_RCTL;
      IF_PRESSED
        register_code(kc);
      else
        unregister_code(kc);
    } return PROCESS_OVERRIDE_BEHAVIOR;

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

    case TOGGLE_US_LIKE: IF_PRESSED {
      toggle_default_layer( KL_(US_LIKE) );
      eeconfig_update_default_layer( default_layer_state );
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case TOGGLE_CAPS_LOCK: IF_PRESSED {
      toggle_default_layer( KL_(CAPS_LOCK) );
      eeconfig_update_default_layer( default_layer_state );
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case TOGGLE_EDIT_LAYER: {
      process_TOGGLE_EDIT_LAYER(record->event.pressed, is_tapped);
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case TOGGLE_INPUT_LAYER: {
      process_TOGGLE_INPUT_LAYER(record->event.pressed, is_tapped);
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case SWITCH_EDIT_LAYER: IF_PRESSED {
      process_SWITCH_EDIT_LAYER();
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case TOGGLE_MOD_RSIDE: IF_PRESSED {
      toggle_default_layer( KL_(MOD_RSIDE) );
      eeconfig_update_default_layer( default_layer_state );
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case TOGGLE_MOD_SANDS: IF_PRESSED {
      toggle_default_layer( KL_(MOD_SANDS) );
      eeconfig_update_default_layer( default_layer_state );
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case INPUT_PAIRED_BRANKETS: IF_PRESSED {
      process_INPUT_PAIRED_BRANKETS( record );
      return PROCESS_OVERRIDE_BEHAVIOR;
    } break;

    case CLEAR_DEFAULT_LAYER: IF_PRESSED {
      default_layer_set(0);
      eeconfig_update_default_layer( default_layer_state );
    } break;

    case DISPLAY_SETTINGS: IF_PRESSED {
      action_displaySettings();
    } break;

    case TAP_RANDOM_BASE64: IF_PRESSED {
      tap_random_base64();
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
    register_code_wait(KC_LCTL);
    tap_code(KC_RIGHT);
    register_code_wait(KC_LSFT);
    tap_code(KC_LEFT);
    unregister_code_wait(KC_LSFT);
    unregister_code_wait(KC_LCTL);
  }
  else if (state->count == 3) {
    // select line
    tap_code(KC_END);
    register_code_wait(KC_LSFT);
    tap_code(KC_HOME);
    unregister_code_wait(KC_LSFT);
  }
  else {
    // ignore
  }
}
#endif // TAP_DANCE_ENABLE

// Turn off CapsLock when it be typed space character.
static void
cancel_capsLock(uint16_t keycode)
{
  if (!IS_HOST_LED_ON(USB_LED_CAPS_LOCK)){
    return;
  }

  // if layer_state is switching then, process break.
  if ( layer_state > default_layer_state ){
    return;
  }

  // if shift-key being pressed then, process break.
  bool isShiftOn = get_mods() & (MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT));
  if ( isShiftOn ){
    return;
  }

  switch(keycode){
    case KC_ENTER:
    case KC_TAB:
    case KC_SPACE:
    case SANDS:
      dprintf("Cancel CapsLock\n");
#ifdef JIS_KEYCODE
      register_mods(MOD_MASK_SHIFT);
      tap_code(KC_CAPS);
      unregister_mods(MOD_MASK_SHIFT);
#else
      tap_code(KC_CAPS);
#endif
      break;
  }
}

static void
process_undo(void)
{
  store_mods();

  register_code_wait(KC_LCTL);
  tap_code(KC_Z);
  unregister_code_wait(KC_LCTL);

  restore_mods();
}

static void
process_cut(void)
{
  store_mods();

  register_code_wait(KC_LCTL);
  tap_code(KC_X);
  unregister_code_wait(KC_LCTL);

  restore_mods();
}

static void
process_copy(void)
{
  store_mods();

  register_code_wait(KC_LCTL);
  tap_code(KC_C);
  unregister_code_wait(KC_LCTL);

  restore_mods();
}

static void
process_paste(void)
{
  store_mods();

  register_code_wait(KC_LCTL);
  tap_code(KC_V);
  unregister_code_wait(KC_LCTL);

  restore_mods();
}

static void
process_redo(void)
{
  store_mods();

  register_code_wait(KC_LCTL);
  tap_code(KC_Y);
  unregister_code_wait(KC_LCTL);

  restore_mods();
}

static void
process_delete_forward_word(void)
{
  store_mods();

  register_code_wait(KC_LCTL);
  register_code_wait(KC_LSFT);
  tap_code(KC_RGHT);
  unregister_code_wait(KC_LSFT);
  unregister_code_wait(KC_LCTL);
  tap_code(KC_DEL);

  restore_mods();
}

static void
process_delete_backward_word(void)
{
  store_mods();

  register_code_wait(KC_LCTL);
  register_code_wait(KC_LSFT);
  tap_code(KC_LEFT);
  unregister_code_wait(KC_LSFT);
  unregister_code_wait(KC_LCTL);
  tap_code(KC_DEL);

  restore_mods();
}

static void
process_TOGGLE_EDIT_LAYER(bool is_pressed, bool is_tapped)
{
  if (is_pressed) {
    layer_on(KL_EDIT);
  }
  else {
    layer_and(~EDIT_LAYER_MASK);
    if (is_tapped) {
      tap_code(KC_MHEN);
    }
  }
}

static void
process_TOGGLE_INPUT_LAYER(bool is_pressed, bool is_tapped)
{
  if (is_pressed) {
    layer_on(KL_INPUT);
  }
  else {
    layer_and(~INPUT_LAYER_MASK);
    if (is_tapped) {
      tap_code(KC_HENK);
    }
  }
}

static void
process_SWITCH_EDIT_LAYER(void)
{
  enum keymap_layer top_layer = biton32(layer_state);
  layer_move( KL_(EDIT) );
  switch (top_layer){
    case KL_EDIT:       layer_on(KL_EDIT_SCRL);   break;
    case KL_EDIT_SCRL:  layer_on(KL_EDIT_MEDIA);  break;
    case KL_EDIT_MEDIA: layer_on(KL_EDIT_SCRL);   break;
    default: break;
  }
}

static void
process_INPUT_PAIRED_BRANKETS(keyrecord_t *record)
{
  uint16_t keycode = getKeycode_fromDefaultLayer( record );
  bool shiftmod = !!( get_mods() & SHIFT_MOD_BITS );
  bool isolayout = layer_state_cmp(default_layer_state, KL_(US_LIKE));

  if (isolayout) {
    switch (keycode) {
      case KC_9: {
        register_code_wait(KC_LSFT);
        tap_code(KC_9);
        tap_code(KC_0);
        unregister_code_wait(KC_LSFT);
        tap_code(KC_LEFT);
      } break;

      case KC_LBRACKET: {
        if (shiftmod) {
          register_code_wait(KC_LSFT);
          tap_code(KC_LBRACKET);
          tap_code(KC_RBRACKET);
          unregister_code_wait(KC_LSFT);
          tap_code(KC_LEFT);
        }
        else {
          tap_code(KC_LBRACKET);
          tap_code(KC_RBRACKET);
          tap_code(KC_LEFT);
        }
      } break;

      case KC_COMMA: {
        register_code_wait(KC_LSFT);
        tap_code(KC_COMMA);
        tap_code(KC_DOT);
        unregister_code_wait(KC_LSFT);
        tap_code(KC_LEFT);
      } break;

      case KC_QUOT: {
        if (shiftmod) {
          register_code_wait(KC_LSFT);
          tap_code(KC_QUOT);
          tap_code(KC_QUOT);
          unregister_code_wait(KC_LSFT);
          tap_code(KC_LEFT);
        }
        else {
          tap_code(KC_QUOT);
          tap_code(KC_QUOT);
          tap_code(KC_LEFT);
        }
      } break;

      case KC_GRV: {
        tap_code(KC_GRV);
        tap_code(KC_GRV);
        tap_code(KC_LEFT);
      } break;
    }
  }
  else {
    switch (keycode) {
      case KC_8: {
        register_code_wait(KC_LSFT);
        tap_code(KC_8);
        tap_code(KC_9);
        unregister_code_wait(KC_LSFT);
        tap_code(KC_LEFT);
      } break;

      case KC_RBRACKET: {
        if (shiftmod) {
          register_code_wait(KC_LSFT);
          tap_code(KC_RBRACKET);
          tap_code(KC_BSLS);
          unregister_code_wait(KC_LSFT);
          tap_code(KC_LEFT);
        }
        else {
          tap_code(KC_RBRACKET);
          tap_code(KC_BSLS);
          tap_code(KC_LEFT);
        }
      } break;

      case KC_COMMA: {
        register_code_wait(KC_LSFT);
        tap_code(KC_COMMA);
        tap_code(KC_DOT);
        unregister_code_wait(KC_LSFT);
        tap_code(KC_LEFT);
      } break;

      case KC_7: {
        register_code_wait(KC_LSFT);
        tap_code(KC_7);
        tap_code(KC_7);
        unregister_code_wait(KC_LSFT);
        tap_code(KC_LEFT);
      } break;

      case KC_2: {
        register_code_wait(KC_LSFT);
        tap_code(KC_2);
        tap_code(KC_2);
        unregister_code_wait(KC_LSFT);
        tap_code(KC_LEFT);
      } break;

      case KC_LBRC: {
        register_code_wait(KC_LSFT);
        tap_code(KC_LBRC);
        tap_code(KC_LBRC);
        unregister_code_wait(KC_LSFT);
        tap_code(KC_LEFT);
      } break;
    }
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

  // Led status
  SEND_STRING("Led status:\n");
  if (IS_HOST_LED_ON(USB_LED_NUM_LOCK)) {
    SEND_STRING("  NUM_LOCK\n");
  }
  if (IS_HOST_LED_ON(USB_LED_CAPS_LOCK)) {
    SEND_STRING("  CAPS_LOCK\n");
  }
  if (IS_HOST_LED_ON(USB_LED_SCROLL_LOCK)) {
    SEND_STRING("  SCROLL_LOCK\n");
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
