#include "pic.h"

__attribute__((naked)) void _main() {
    pic_remap(0x20, 0x28);
    pic_mask(0xfd, 0xff);
    pic_enable();
    *(int*)(0xb8000) = 0x203a2028;
    while (1) {}
}
