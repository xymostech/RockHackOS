#include "mem.h"

int str_equal(char *a, char *b) {
    while(*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

void strcpy(char *to, char *from) {
    while (*from) {
        *to = *from;
        to++;
        from++;
    }
}

void memset(uint8_t *buffer, int size, uint8_t toset) {
    int i;
    for (i = 0; i < size; i++) {
        buffer[i] = toset;
    }
}

void memcpy(uint8_t *from, uint8_t *to, int bytes) {
    int i;
    for (i = 0; i < bytes; i++) {
        to[i] = from[i];
    }
}
