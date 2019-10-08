/**********************************************************************
 *  Data Format
 * ______________________________ ___
 * | - - - - - - - - - - - - -  |  |
 * | - - - Free Space - - - - - |  |
 * |_-_-_-_-_-_-_-_-_-_-_-_-_-_ |  |
 * |                            |  | Segment Size
 * |            Data            |  |
 * |                            |  |
 * |____________________________| _|_
 * |        Segment Data        | 
 * |____________________________|
 *  
 * *******************************************************************/

#ifndef __MEMORYSEGMENT_H
#define __MEMORYSEGMENT_H

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#define ZERO 0x0
#define MEM_MIN 1024

#define MEM_SEG_INFO sizeof(struct MemorySegment)
struct MemorySegment
{
    size_t segmentSize;
    size_t dataSize;
    size_t isFree;
    struct MemorySegment *next;
    struct MemorySegment *prev;
};

// "Public" functions
void *addToLinkedList(size_t segmentSize, size_t dataSize);
void printLinkedList(void);
void setVerbose(uint8_t oneOrZero);
void linkedListReset(void);
void linkedListMarkFree(void*);


#endif //__MEMORYSEGMENT_H