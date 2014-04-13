#ifndef DMA_H
#define DMA_H

#include "io.h"

void dma_setup_floppy();
void dma_prepare_floppy_write();
void dma_prepare_floppy_read();

#endif
