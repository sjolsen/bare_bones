#ifndef SCANCODE_H
#define SCANCODE_H

#include "stdint.h"

typedef enum {
	KEY_NONE = 0,

	KEY_ESCAPE = 0x01,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_Q,
	KEY_W,
	KEY_E,
	KEY_R,
	KEY_T,
	KEY_Y,
	KEY_U,
	KEY_I,
	KEY_O,
	KEY_P,
	KEY_LEFT_BRACKET,
	KEY_RIGHT_BRACKET,
	KEY_ENTER,
	KEY_LEFT_CONTROL,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_SEMICOLON,
	KEY_QUOTE,
	KEY_BACKTICK,
	KEY_LEFT_SHIFT,
	KEY_BACKSLASH,
	KEY_Z,
	KEY_X,
	KEY_C,
	KEY_V,
	KEY_B,
	KEY_N,
	KEY_M,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_RIGHT_SHIFT,
	KEY_KP_ASTERISK,
	KEY_LEFT_ALT,
	KEY_SPACE,
	KEY_CAPS_LOCK,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_NUM_LOCK,
	KEY_SCROLL_LOCK,
	KEY_KP_7,
	KEY_KP_8,
	KEY_KP_9,
	KEY_KP_MINUS,
	KEY_KP_4,
	KEY_KP_5,
	KEY_KP_6,
	KEY_KP_PLUS,
	KEY_KP_1,
	KEY_KP_2,
	KEY_KP_3,
	KEY_KP_0,
	KEY_KP_PERIOD,

	KEY_F11 = 0x57,
	KEY_F12,

	KEY_KP_ENTER = 0xE01C,
	KEY_RIGHT_CONTROL,

	KEY_KP_SLASH = 0xE035,

	KEY_RIGHT_ALT = 0xE038,

	KEY_HOME = 0xE047,
	KEY_UP,
	KEY_PAGE_UP,

	KEY_LEFT = 0xE04B,

	KEY_RIGHT = 0xE04D,

	KEY_END = 0xE04F,
	KEY_DOWN,
	KEY_PAGE_DOWN,
	KEY_INSERT,
	KEY_DELETE,

	KEY_LEFT_SUPER = 0xE05B,
	KEY_RIGHT_SUPER,
	KEY_MENU,

	KEY_PRINT_SCREEN,
	KEY_PAUSE,
} physical_key;

typedef enum {
	TYPE_NONE,
	TYPE_PRESSED,
	TYPE_RELEASED
} event_type;

typedef struct {
	event_type type;
	physical_key key;
} key_event;

typedef struct {
	uint8_t state;
} scancode_decoder_state;

scancode_decoder_state make_decoder_state (void);

key_event scancode_decode (scancode_decoder_state (*state), uint8_t scancode);

#endif
