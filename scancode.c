#include "scancode.h"

enum {
	STATE_BASE,
	STATE_E0,
	STATE_E02A,
	STATE_E02AE0,
	STATE_E0B7,
	STATE_E0B7E0,
	STATE_E1,
	STATE_E11D,
	STATE_E11D45,
	STATE_E11D45E1,
	STATE_E11D45E19D,
};

#define INRANGE(x,a,b) ((a) <= (x) && (x) <= (b))
#define NONE (key_event) {TYPE_NONE, KEY_NONE}
#define PRESS(key) (key_event) {TYPE_PRESSED, key}
#define RELEASE(key) (key_event) {TYPE_RELEASED, key}

static inline
key_event decode_BASE (scancode_decoder_state (*state), uint8_t scancode)
{
	if (scancode == 0xE0) {
		(*state).state = STATE_E0;
		return NONE;
	}
	if (scancode == 0xE1) {
		(*state).state = STATE_E1;
		return NONE;
	}

	if (INRANGE (scancode, KEY_ESCAPE, KEY_KP_PERIOD) ||
	    INRANGE (scancode, KEY_F11,    KEY_F12)) {
		(*state).state = STATE_BASE;
		return PRESS (scancode);
	}
	if (INRANGE (scancode & ~0x80, KEY_ESCAPE, KEY_KP_PERIOD) ||
	    INRANGE (scancode & ~0x80, KEY_F11,    KEY_F12)) {
		(*state).state = STATE_BASE;
		return RELEASE (scancode & ~0x80);
	}

	(*state).state = STATE_BASE;
	return NONE;
}

static inline
key_event decode_E0 (scancode_decoder_state (*state), uint8_t scancode)
{
	if (scancode == 0x2A) {
		(*state).state = STATE_E02A;
		return NONE;
	}
	if (scancode == 0xB7) {
		(*state).state = STATE_E0B7;
		return NONE;
	}

	if (INRANGE (scancode, KEY_KP_ENTER   - 0xE000, KEY_RIGHT_CONTROL - 0xE000) ||
	    INRANGE (scancode, KEY_KP_SLASH   - 0xE000, KEY_KP_SLASH      - 0xE000) ||
	    INRANGE (scancode, KEY_RIGHT_ALT  - 0xE000, KEY_RIGHT_ALT     - 0xE000) ||
	    INRANGE (scancode, KEY_HOME       - 0xE000, KEY_PAGE_UP       - 0xE000) ||
	    INRANGE (scancode, KEY_LEFT       - 0xE000, KEY_LEFT          - 0xE000) ||
	    INRANGE (scancode, KEY_RIGHT      - 0xE000, KEY_RIGHT         - 0xE000) ||
	    INRANGE (scancode, KEY_END        - 0xE000, KEY_DELETE        - 0xE000) ||
	    INRANGE (scancode, KEY_LEFT_SUPER - 0xE000, KEY_MENU          - 0xE000)) {
		(*state).state = STATE_BASE;
		return PRESS (scancode);
	}
	if (INRANGE (scancode & ~0x80, KEY_KP_ENTER   - 0xE000, KEY_RIGHT_CONTROL - 0xE000) ||
	    INRANGE (scancode & ~0x80, KEY_KP_SLASH   - 0xE000, KEY_KP_SLASH      - 0xE000) ||
	    INRANGE (scancode & ~0x80, KEY_RIGHT_ALT  - 0xE000, KEY_RIGHT_ALT     - 0xE000) ||
	    INRANGE (scancode & ~0x80, KEY_HOME       - 0xE000, KEY_PAGE_UP       - 0xE000) ||
	    INRANGE (scancode & ~0x80, KEY_LEFT       - 0xE000, KEY_LEFT          - 0xE000) ||
	    INRANGE (scancode & ~0x80, KEY_RIGHT      - 0xE000, KEY_RIGHT         - 0xE000) ||
	    INRANGE (scancode & ~0x80, KEY_END        - 0xE000, KEY_DELETE        - 0xE000) ||
	    INRANGE (scancode & ~0x80, KEY_LEFT_SUPER - 0xE000, KEY_MENU          - 0xE000)) {
		(*state).state = STATE_BASE;
		return RELEASE (scancode);
	}

	(*state).state = STATE_BASE;
	return NONE;
}

#define DECLARE_INTERMEDIATE_DECODER(CUR,code) \
static inline \
key_event decode_##CUR (scancode_decoder_state (*state), uint8_t scancode) \
{ \
	if (scancode == 0x##code) { \
		(*state).state = STATE_##CUR##code; \
		return NONE; \
	} \
 \
	(*state).state = STATE_BASE; \
	return NONE; \
}

#define DECLARE_ACCEPTOR_DECODER(CUR,code,result) \
static inline \
key_event decode_##CUR (scancode_decoder_state (*state), uint8_t scancode) \
{ \
	if (scancode == 0x##code) { \
		(*state).state = STATE_BASE; \
		return result; \
	} \
 \
	(*state).state = STATE_BASE; \
	return NONE; \
}

DECLARE_INTERMEDIATE_DECODER (E02A, E0);
DECLARE_INTERMEDIATE_DECODER (E0B7, E0);
DECLARE_ACCEPTOR_DECODER (E02AE0, 37, PRESS (KEY_PRINT_SCREEN));
DECLARE_ACCEPTOR_DECODER (E0B7E0, AA, RELEASE (KEY_PRINT_SCREEN));

DECLARE_INTERMEDIATE_DECODER (E1, 1D);
DECLARE_INTERMEDIATE_DECODER (E11D, 45);
DECLARE_INTERMEDIATE_DECODER (E11D45, E1);
DECLARE_INTERMEDIATE_DECODER (E11D45E1, 9D);
DECLARE_ACCEPTOR_DECODER (E11D45E19D, C5, PRESS (KEY_PAUSE));


// Extern functions

scancode_decoder_state make_decoder_state (void)
{
	return (scancode_decoder_state) {STATE_BASE};
}

key_event scancode_decode (scancode_decoder_state (*state), uint8_t scancode)
{
	switch ((*state).state)
	{
	case STATE_BASE: return decode_BASE (state, scancode);
	case STATE_E0: return decode_E0 (state, scancode);
	case STATE_E02A: return decode_E02A (state, scancode);
	case STATE_E02AE0: return decode_E02AE0 (state, scancode);
	case STATE_E0B7: return decode_E0B7 (state, scancode);
	case STATE_E0B7E0: return decode_E0B7E0 (state, scancode);
	case STATE_E1: return decode_E1 (state, scancode);
	case STATE_E11D: return decode_E11D (state, scancode);
	case STATE_E11D45: return decode_E11D45 (state, scancode);
	case STATE_E11D45E1: return decode_E11D45E1 (state, scancode);
	case STATE_E11D45E19D: return decode_E11D45E19D (state, scancode);
	default: return NONE;
	}
}
