#include "hexes.h"

void hexes_move_cursor(int row, int col) {
    uint16_t position = row * 80 + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, position & 0xFF);

    outb(0x3D4, 0x0E);
    outb(0x3D5, (position >> 8) & 0xFF);
}

void hexes_write_char(uint8_t character, int row, int col) {
    uint16_t position = row * 80 + col;

    *(char*)(0xb8000 + position * 2) = character;
}

void hexes_write_string(char *str, int row, int col) {
    while(*str) {
        hexes_write_char(*str, row, col);
        col++;
        str++;
    }
}
