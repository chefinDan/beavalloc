#include "beavalloc.h"
#include "memorySegment.h"

uint8_t beavVerbose = 0;

void 
*beavalloc(size_t requestedMemorySize){
    size_t willAllocateSize;
    void *allocatedMemoryAddr;


    if(requestedMemorySize <= 0){
        beavVerbose ? fprintf(stderr, "Line %d: Requested memory size must be greater than zero\n", __LINE__): 0;
        return NULL;
    }

    beavVerbose ? fprintf(stderr, "Line %d: User requests %ld bytes of memory\n", __LINE__, requestedMemorySize): 0;
   
    if(requestedMemorySize < MEM_MIN){ willAllocateSize = MEM_MIN; }

    allocatedMemoryAddr = addToLinkedList(willAllocateSize, requestedMemorySize);
    // printLinkedList();

    return allocatedMemoryAddr; 
}

void beavalloc_set_verbose(uint8_t setting){
    beavVerbose = setting;
    setVerbose(setting);
}

