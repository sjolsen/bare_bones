#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "keybuffer.h"

typedef bool (*keyboard_consumer_t) (keybuffer* buffer);

void keyboard_initialize (keyboard_consumer_t consumer);
bool keyboard_consume (void);

#endif
