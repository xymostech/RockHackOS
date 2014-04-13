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

int curr_col = 0, curr_row = 0;

void hexes_setup_print() {
    hexes_move_cursor(0, 0);
    curr_col = 0;
    curr_row = 0;
}

void hexes_print_char(uint8_t character) {
    if (character == 0x0A) {
        curr_row++;
        curr_col = 0;
    } else {
        hexes_write_char(character, curr_row, curr_col);
        curr_col++;
        if (curr_col == 80) {
            curr_col = 0;
            curr_row++;
            if (curr_row == 25) {
                curr_row = 0;
            }
        }
    }
    hexes_move_cursor(curr_row, curr_col);
}

void hexes_print_backspace() {
    curr_col--;
    if (curr_col < 0) {
        curr_col = 79;
        curr_row--;
        if (curr_row < 0) {
            curr_row = 24;
        }
    }
    hexes_write_char(' ', curr_row, curr_col);
    hexes_move_cursor(curr_row, curr_col);
}
