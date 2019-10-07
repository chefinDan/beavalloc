#include "memorySegment.h"

struct MemorySegment *head = 0;

void *addToLinkedList(size_t segmentSize, size_t dataSize){
    struct MemorySegment *newMemSeg, *memSegWithRoom = 0;
    if (head == 0)
    {
        // Start of linked list.
        // Increase heap to include requested memory and MemorySegmentData struct
        fprintf(stderr, "Line %d: First allocation, initializing head of memory segment linked list\n", __LINE__);
        newMemSeg = sbrk(MEM_SEG_INFO + segmentSize);
        fprintf(stderr, "Line %d: Heap increased by %ld + %ld\n", __LINE__, segmentSize, MEM_SEG_INFO);
        newMemSeg->segmentSize = segmentSize;
        newMemSeg->dataSize = dataSize;
        newMemSeg->isFree = 0;
        newMemSeg->next = 0;
        newMemSeg->prev = 0;
        head = newMemSeg;
    }
    else // head points to a memorySegment Node
    {
        // first look for enough space in an existing memory segment
        memSegWithRoom = findAvailableMemory(dataSize);
        if(memSegWithRoom){
            // Space was found in existing memory segment

            fprintf(stderr, "Line %d: Adding new node to memory segment linked list\n", __LINE__);
            newMemSeg = (memSegWithRoom + 2); // point newMemSegData to end of data section
            
            // newMemSeg's segment is the size of the xisting segment minus the existing segment data size
            newMemSeg->segmentSize = memSegWithRoom->segmentSize - memSegWithRoom->dataSize;
            
            // reduce the size of the existing segment size to be the size of the data stored there,
            // Essentially making the segment "Full", no more room. 
            memSegWithRoom->segmentSize = memSegWithRoom->dataSize;

            newMemSeg->dataSize = dataSize;
            newMemSeg->isFree = 0;
            newMemSeg->next = memSegWithRoom->next;
            memSegWithRoom->next = newMemSeg;
            newMemSeg->prev = memSegWithRoom;
        }
        else{
            fprintf(stderr, "Line %d: Adding new node to memory segment linked list\n", __LINE__);
            newMemSeg = sbrk(MEM_SEG_INFO + segmentSize);
            fprintf(stderr, "Line %d: Heap increased by %ld + %ld\n", __LINE__, segmentSize, MEM_SEG_INFO);
            newMemSeg->segmentSize = segmentSize;
            newMemSeg->dataSize = dataSize;
            newMemSeg->isFree = 0;
            newMemSeg->next = NULL;
            newMemSeg->prev = findLastSegment();
            head->next = newMemSeg;
        }
    }

    return newMemSeg + 1;
}

struct MemorySegment *findAvailableMemory(size_t dataSize){
    struct MemorySegment *memSegPtr;
    size_t i = 0;

    fprintf(stderr, "*** Traversing memory segment linked list looking for available memory *** \n");
    memSegPtr = head;
    do
    {   // check if the current memSeg has data space for a new linkedList node and the user requested data
        if( (memSegPtr->size - memSegPtr->dataSize) > (dataSize + MEM_SEG_INFO) ) {
            return  memSegPtr; //return address of segment that has room for new data
        }
        
        memSegPtr = memSegPtr->next;
    } while (memSegPtr);

    return NULL;
}


struct MemorySegment *findLastSegment(void)
{
    fprintf(stderr, "*** Traversing memory segment linked list looking for last segment *** \n");
    memSegPtr = head;
    do
    {
        if(memSegPtr->next == NULL){ return memSegPtr; }
    } while (memSegPtr);

    return NULL;
}


void printLinkedList(void)
{
    struct MemorySegment *memSegPtr;
    size_t i = 0;

    fprintf(stderr, "*** Traversing memory segment linked list *** \n");
    memSegPtr = head;
    do
    {
        fprintf(stderr, "%ld. Segment Addr: %p\n    Segment Size: %ld\n    Data Size: %ld\n    isFree: %ld\n    Next: %p\n    Prev: %p\n", i++, memSegPtr, memSegPtr->size, memSegPtr->dataSize, memSegPtr->isFree, memSegPtr->next, memSegPtr->prev);
        memSegPtr = memSegPtr->next;
    } while (memSegPtr);
}