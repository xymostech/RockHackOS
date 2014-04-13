#ifndef HEXES_H
#define HEXES_H

#include "types.h"
#include "io.h"

void hexes_move_cursor(int row, int col);
void hexes_write_char(uint8_t character, int row, int col);
void hexes_write_string(char *str, int row, int col);

void hexes_print_setup();
void hexes_print_string(char *str);

#endif
