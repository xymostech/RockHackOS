#include "pic.h"
#include "floppy.h"
#include "dma.h"
#include "hexes.h"
#include "fs.h"
#include "shell.h"

void print_number(uint16_t x, int row, int col) {
    char buf[5];
    char ch;
    int c;

    for (c = 3; c >= 0; c--) {
        ch = x & 0xf;
        if (ch < 10) {
            ch = ch + '0';
        } else {
            ch = ch + 'A' - 10;
        }
        buf[c] = ch;
        x >>= 4;
    }

    buf[4] = 0;
    hexes_write_string(buf, row, col);
}

void clear_screen() {
    int i, j;
    for (i = 0; i < 25; i++) {
        for (j = 0; j < 80; j++) {
            hexes_write_char(' ', i, j);
        }
    }
}

__attribute__((naked)) void _main() {
    clear_screen();

    pic_remap(0x20, 0x28);
    pic_mask(0xbd, 0xff);
    pic_enable();

    floppy_init();

    uint8_t buffer[512];
    floppy_read(buffer, 0, 0, 1);

    clear_screen();
    hexes_setup_print();

    shell_run();
}
