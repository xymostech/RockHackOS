#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "hexes.h"

uint8_t keyboard_get_last_key();
void _keyboard_int(int keycode);

#endif
