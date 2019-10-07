#include "beavalloc.h"
#include "memorySegment.h"

struct MemorySegment *head = 0;

void 
*beavalloc(size_t requestedMemorySize){
    size_t willAllocateSize;
    struct MemorySegment *newMemSegData = 0;

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
    
    if(head == 0){
        // Start of linked list.
        // Increase heap to include requested memory and MemorySegmentData struct
        fprintf(stderr, "Line %d: First allocation, initializing head of memory segment linked list\n", __LINE__);
        newMemSegData = sbrk(MEM_SEG_INFO + willAllocateSize);
        fprintf(stderr, "Line %d: Heap increased by %ld + %ld\n", __LINE__, willAllocateSize, MEM_SEG_INFO);
        newMemSegData->size = willAllocateSize;
        newMemSegData->dataSize = requestedMemorySize;
        newMemSegData->isFree = 0;
        newMemSegData->next = 0;
        newMemSegData->prev = 0;
        head = newMemSegData;
    }
    else{
        // Add new node to linked list
        fprintf(stderr, "Line %d: Adding new node to memory segment linked list\n", __LINE__);
        newMemSegData = sbrk(MEM_SEG_INFO + willAllocateSize);
        fprintf(stderr, "Line %d: Heap increased by %ld + %ld\n", __LINE__, willAllocateSize, MEM_SEG_INFO);
        newMemSegData->size = willAllocateSize;
        newMemSegData->dataSize = requestedMemorySize;
        newMemSegData->isFree = 0;
        newMemSegData->next = 0;
        newMemSegData->prev = head;
        head->next = newMemSegData;
    }
    
    printLinkedList(head);
    return newMemSegData + 1; //ptr arithmetic dictates that this will return 
}

