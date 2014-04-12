#ifndef PIC_H
#define PIC_H

#include "types.h"

void pic_remap(uint8_t offset1, uint8_t offset2);
void pic_mask(uint8_t mask1, uint8_t mask2);
void pic_enable();
void pic_disable();

#endif
