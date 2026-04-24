#ifndef TYPES_CORE_H
#define TYPES_CORE_H
#include <stdint.h>

typedef struct SelectionBuffer{
    uint32_t* bits; // Array of bit-chunks
    int width;
    int height;
} SelectionBuffer;
#endif
