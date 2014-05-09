#include "pic.h"
#include "floppy.h"
#include "dma.h"
#include "hexes.h"
#include "fs.h"
#include "shell.h"

__attribute__((naked)) void _main() {
    pic_remap(0x20, 0x28);
    pic_mask(0xbd, 0xff);
    pic_enable();

    floppy_init();

    volatile uint8_t buffer[512];
    buffer[511] = 0x1;

    hexes_clear_screen();
    hexes_setup_print();

    shell_run();
}
