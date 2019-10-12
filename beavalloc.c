#include <sys/types.h>
#include "beavalloc.h"
#include "memorySegment.h"

static uint8_t Verbose = 0;

void *beavalloc(size_t requestedMemorySize){
    size_t willAllocateSize;
    void *allocatedMemoryAddr;

    if(requestedMemorySize < 0){
        Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n    | Requested memory size must be greater than zero\n", __FILE__, __LINE__, __FUNCTION__) : 0;
        return NULL;
    }

    if(requestedMemorySize == 0){
        Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n    | Requested memory size is 0 or NULL, returning NULL\n", __FILE__, __LINE__, __FUNCTION__) : 0;
        return NULL;
    }

    Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n    | User requests %ld bytes of memory\n", __FILE__, __LINE__, __FUNCTION__, requestedMemorySize) : 0;

    willAllocateSize = (requestedMemorySize < MEM_MIN) ? MEM_MIN : requestedMemorySize;

    allocatedMemoryAddr = addToLinkedList(willAllocateSize, requestedMemorySize);
    printLinkedList();

    return allocatedMemoryAddr; 
}

void beavfree(void *ptr)
{
    struct MemorySegment *segmentToFree;

    if (!ptr)
        return;

    segmentToFree = (struct MemorySegment *)ptr;

    linkedListMarkFree(segmentToFree);
    printLinkedList();
}

void beavalloc_reset(void)
{
    Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n", __FILE__, __LINE__, __FUNCTION__) : 0;
    linkedListReset();
}

void 
beavalloc_set_verbose(uint8_t setting){
    Verbose = setting;
    setVerbose(setting);
}

void
*beavcalloc(size_t nmemb, size_t size){
    void *allocatedMemoryAddr;
    size_t totalMemoryRequested;

    if(nmemb == 0 || size == 0){
        return NULL;
    }

    totalMemoryRequested = nmemb * size;

    allocatedMemoryAddr = beavalloc(nmemb*size);
    memset(allocatedMemoryAddr, 0, totalMemoryRequested);

    return allocatedMemoryAddr;

}

void 
*beavrealloc(void *ptr, size_t size){
    size_t willAllocateSize = 0;
    void * dataSegPtr;
    // size must be an integer
    if(size < 0)
        return NULL;
    
    Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n    | User requests %ld bytes be reallocated\n", __FILE__, __LINE__, __FUNCTION__, size) : 0;

    // if size is 0 and ptr is an addr, equivalent to free(ptr)
    if(size == 0 && ptr){
        linkedListMarkFree(ptr);
    }

    // If requested size is less than MEM_MIN/2, then allocate MEM_MIN, otherwise
    // allocate size * 2
    // giving the user at least double what they asked for  
    willAllocateSize  = ((size / 2) < (MEM_MIN / 2)) ? MEM_MIN : size * 2;
    Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n    | Will allocate %ld bytes\n", __FILE__, __LINE__, __FUNCTION__, willAllocateSize) : 0;
    
    // ptr is null, then function behaves like malloc()
    if(!ptr){
        dataSegPtr = beavalloc(willAllocateSize);
    }
    else{
        // resize the linkedList node pointed to by ptr
        dataSegPtr = linkedListResize(ptr, willAllocateSize);
    }

    printLinkedList();
    return dataSegPtr; 
}



void beavalloc_dump(uint leaks_only)
{
    dumpLinkedList(leaks_only);
}
