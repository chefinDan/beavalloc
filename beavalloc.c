#include "beavalloc.h"
#include "memorySegment.h"


void 
*beavalloc(size_t requestedMemorySize){
    size_t willAllocateSize;
    void *allocatedMemoryAddr;


    if(requestedMemorySize <= 0){
        fprintf(stderr, "Line %d: Requested memory size must be greater than zero\n", __LINE__);
    }

    if(requestedMemorySize <= MEM_MIN){
        willAllocateSize = MEM_MIN;
        fprintf(stderr, "Line %d: User requests %ld bytes of memory, will allocate required minimum of %d bytes\n", __LINE__, requestedMemorySize, MEM_MIN);
    }
    else{
        willAllocateSize = requestedMemorySize;
        fprintf(stderr, "Line %d: User requests %ld bytes of memory, will attempt to allocate %ld bytes\n", __LINE__, requestedMemorySize, willAllocateSize);
    }
    
    allocatedMemoryAddr = addToLinkedList(willAllocateSize, requestedMemorySize);
    printLinkedList();

    return allocatedMemoryAddr; 
}

