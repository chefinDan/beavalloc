#ifndef __MEMORYSEGMENT_H
#define __MEMORYSEGMENT_H

#include <stdio.h>

#define ZERO 0x0
#define MEM_MIN 1024

struct MemorySegment
{
    size_t size;
    size_t dataSize;
    size_t isFree;
    struct MemorySegment *next;
    struct MemorySegment *prev;
};
#define MEM_SEG_INFO sizeof(struct MemorySegment)

void printLinkedList(struct MemorySegment *memSegPtr);

#endif //__MEMORYSEGMENT_H