#include "beavalloc.h"

struct memorySegmentData *head = 0;

void *beavalloc(size_t size){
    int requestedMemorySize;
    struct memorySegmentData *newMemSegData = 0;
    
    if(size < 0){
        fprintf(stderr, "size cannot be less than zero\n");
    }

    requestedMemorySize = (size <= 1024) ? 1024 : size;
    
    if(head == 0){
        // Start of linked list.
        // Increase heap to include requested memory and
        // memorySegment struct
        newMemSegData = sbrk(MEM_SEG_INFO + requestedMemorySize);
        newMemSegData->size = requestedMemorySize;
        newMemSegData->isFree = 0;
        newMemSegData->next = 0;
        newMemSegData->prev = 0;
    }

    head = newMemSegData;

    return newMemSegData + 1; //ptr arithmetic dictates that this will return 
}

