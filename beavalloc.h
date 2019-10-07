#ifndef __BEAVALLOC_H
#define __BEAVALLOC_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ZERO 0x0


struct memorySegmentData
{
    size_t size;
    int isFree;
    struct memorySegmentData *next;
    struct memorySegmentData *prev;
};
#define MEM_SEG_INFO sizeof(struct memorySegmentData)


// function declarations
void *beavalloc(size_t memSize);

#endif //__BEAVALLOC_H