#ifndef FLOPPY_H
#define FLOPPY_H

#include "pic.h"

void _irq6_handler();
void floppy_init();
void floppy_reset();
void floppy_seek();
void floppy_read(uint8_t *buffer);

#endif
