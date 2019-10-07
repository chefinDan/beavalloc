#include "beavalloc.h"
#include "memorySegment.h"

uint8_t beavVerbose = 0;

void 
*beavalloc(size_t requestedMemorySize){
    size_t willAllocateSize;
    void *allocatedMemoryAddr;


    if(requestedMemorySize < 0){
        beavVerbose ? fprintf(stderr, "-> %s:%d, in %s()\n    | Requested memory size must be greater than zero\n", __FILE__, __LINE__, __FUNCTION__) : 0;

        return NULL;
    }

    if(requestedMemorySize == 0){
        beavVerbose ? fprintf(stderr, "-> %s:%d, in %s()\n    | Requested memory size is 0 or NULL, returning NULL\n", __FILE__, __LINE__, __FUNCTION__) : 0;

        return NULL;
    }

    beavVerbose ? fprintf(stderr, "-> %s:%d, in %s()\n    | User requests %ld bytes of memory\n", __FILE__, __LINE__, __FUNCTION__, requestedMemorySize) : 0;

    willAllocateSize = (requestedMemorySize < MEM_MIN) ? MEM_MIN : requestedMemorySize;

    allocatedMemoryAddr = addToLinkedList(willAllocateSize, requestedMemorySize);
    printLinkedList();

    return allocatedMemoryAddr; 
}

void 
beavalloc_set_verbose(uint8_t setting){
    beavVerbose = setting;
    setVerbose(setting);
}

void 
beavalloc_reset(void){
    beavVerbose ? fprintf(stderr, "-> %s:%d, in %s()\n", __FILE__, __LINE__, __FUNCTION__): 0;
    linkedListReset();
}

