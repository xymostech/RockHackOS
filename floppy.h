#ifndef FLOPPY_H
#define FLOPPY_H

#include "pic.h"
#include "dma.h"
#include "hexes.h"

void _irq6_handler();
void floppy_init();
void floppy_reset();
void floppy_read(uint8_t *buffer, uint8_t head, uint8_t cylinder, uint8_t sector);
void floppy_write(uint8_t *buffer, uint8_t head, uint8_t cylinder, uint8_t sector);

#endif
