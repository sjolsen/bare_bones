#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "cbuffer.h"

typedef bool (*keyboard_consumer_t) (cbuffer* buffer);

void keyboard_initialize (keyboard_consumer_t consumer);
bool keyboard_consume (void);

#endif
