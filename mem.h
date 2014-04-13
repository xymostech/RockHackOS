#ifndef MEM_H
#define MEM_H

#include "types.h"

int str_equal(char *a, char *b);
void strcpy(char *to, char *from);
void memset(uint8_t *buffer, int size, uint8_t toset);
void memcpy(uint8_t *from, uint8_t *to, int bytes);

#endif
