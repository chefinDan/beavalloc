#include "memorySegment.h"

void printLinkedList(struct MemorySegment *headPtr)
{
    struct MemorySegment *memSegPtr;
    size_t i = 0;

    fprintf(stderr, "*** Traversing memory segment linked list *** \n");
    memSegPtr = headPtr;
    do
    {
        fprintf(stderr, "%ld. Segment Addr: %p\n    Segment Size: %ld\n    Data Size: %ld\n    isFree: %ld\n    Next: %p\n    Prev: %p\n", i++, memSegPtr, memSegPtr->size, memSegPtr->dataSize, memSegPtr->isFree, memSegPtr->next, memSegPtr->prev);
        memSegPtr = memSegPtr->next;
    } while (memSegPtr);
}