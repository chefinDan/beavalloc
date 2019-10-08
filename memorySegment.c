#include "memorySegment.h"

// variable that represents the head of the linked list
static struct MemorySegment *head = 0;

static void *initialProgramBreak;

// variable to determine verbosity of execution
static uint8_t _Verbose = 0;

static size_t sumAllSegments(void){
    struct MemorySegment *memSegPtr;
    size_t sum = 0;

    memSegPtr = head;
    do
    { 
        sum += memSegPtr->segmentSize;
        memSegPtr = memSegPtr->next;
    } while (memSegPtr);

    return sum;
}

static struct MemorySegment *findFirstAvailableMemory(size_t dataSize)
{
    struct MemorySegment *memSegPtr;

    _Verbose ? fprintf(stderr, "--> %s:%d, in %s()\n    | Traversing memory segment linked list looking for available memory\n", __FILE__, __LINE__, __FUNCTION__) : 0;
    memSegPtr = head;
    do
    { // check if the current memSeg has data space for a new linkedList node and the user requested data
        if ((memSegPtr->segmentSize - memSegPtr->dataSize) > (dataSize + MEM_SEG_INFO))
        {
            return memSegPtr; //return address of segment that has room for new data
        }

        memSegPtr = memSegPtr->next;
    } while (memSegPtr);

    return NULL;
}

static struct MemorySegment *findLastSegment(void)
{
    struct MemorySegment *memSegPtr;
    _Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n   | Traversing memory segment linked list looking for last segment\n", __FILE__, __LINE__, __FUNCTION__) : 0;

    for(memSegPtr=head; memSegPtr->next; memSegPtr=memSegPtr->next){}
    return memSegPtr;
}

static struct MemorySegment *createLinkedListHead(size_t segmentSize, size_t dataSize){
    struct MemorySegment *newMemSeg;

    // Start of linked list.
    // Increase heap to include requested memory and MemorySegmentData struct
    _Verbose ? fprintf(stderr, "--> %s:%d, in %s()\n    | First allocation, initializing head of memory segment linked list\n", __FILE__, __LINE__, __FUNCTION__) : 0;
    initialProgramBreak = sbrk(ZERO);
    _Verbose ? fprintf(stderr, "    | Saving initial program break: %p\n", initialProgramBreak) : 0;

    newMemSeg = (struct MemorySegment *)sbrk(MEM_SEG_INFO + segmentSize);
    _Verbose ? fprintf(stderr, "    | Heap increased by %ld + %ld bytes\n\n", segmentSize, MEM_SEG_INFO) : 0;
    newMemSeg->segmentSize = segmentSize;
    newMemSeg->dataSize = dataSize;
    newMemSeg->isFree = 0;
    newMemSeg->next = 0;
    newMemSeg->prev = 0;
    head = newMemSeg;
    return newMemSeg + 1; // return address to beginning of data section of new memorysegment on heap
}

static struct MemorySegment *splitMemorySegment(struct MemorySegment *memSegWithRoom, size_t segmentSize, size_t dataSize){
    struct MemorySegment *newMemSeg = 0;

    // Found memory segment with room for new data
    _Verbose ? fprintf(stderr, "Line %d: Adding new node to memory segment linked list on existing heap\n", __LINE__) : 0;
    newMemSeg = (memSegWithRoom + 2); // point newMemSegData to end of the data section in memory segment with room

    // newMemSeg's segment is the size of the xisting segment minus the existing segment data size
    newMemSeg->segmentSize = memSegWithRoom->segmentSize - memSegWithRoom->dataSize;

    // reduce the size of the existing segment size to be the size of the data stored there,
    // Essentially making the segment "Full", ie no more room.
    memSegWithRoom->segmentSize = memSegWithRoom->dataSize;

    newMemSeg->dataSize = dataSize;
    newMemSeg->isFree = 0;
    newMemSeg->prev = memSegWithRoom;
    newMemSeg->next = memSegWithRoom->next;
    memSegWithRoom->next = newMemSeg;

    return newMemSeg + 1;
}

static struct MemorySegment *addNewSegment(size_t segmentSize, size_t dataSize){
    // no room found in existing segments
    struct MemorySegment *newMemSeg;
    _Verbose ? fprintf(stderr, "Line %d: No space found in exsiting heap, adding new node to memory segment linked list\n", __LINE__) : 0;
    newMemSeg = (struct MemorySegment *)sbrk(MEM_SEG_INFO + segmentSize);
    _Verbose ? fprintf(stderr, "Line %d: Heap increased by %ld + %ld bytes\n", __LINE__, segmentSize, MEM_SEG_INFO) : 0;
    newMemSeg->segmentSize = segmentSize;
    newMemSeg->dataSize = dataSize;
    newMemSeg->isFree = 0;
    newMemSeg->next = NULL;
    newMemSeg->prev = findLastSegment();
    (newMemSeg->prev)->next = newMemSeg;

    return newMemSeg +1;
}

static struct MemorySegment *joinSegments(struct MemorySegment *segA, struct MemorySegment *segB){
    struct MemorySegment *segPtr;
    // point (segment after segB_->prev to segA->prev
    (segB->next)->prev = segA->prev;
    // point segA next to segB next 
    (segA->prev)->next = segB->next;

    // add segB segment size to segA's segment size
    (segA->prev)->segmentSize = (segA->prev)->segmentSize + segA->segmentSize + segB->segmentSize;
    segPtr = segA->prev;
    segB = NULL;
    segA = NULL;

    return segPtr;
}


void setVerbose(uint8_t oneOrZero)
{
    _Verbose = oneOrZero;
}


void *addToLinkedList(size_t segmentSize, size_t dataSize){
    struct MemorySegment *memSegWithRoom = 0;
    if (head == 0)
    {
        // head is uninitialized, create the first node in linked list
        // and return the address of the data segment
        return createLinkedListHead(segmentSize, dataSize);
    }
    
    // head points to a memorySegment Node
    // first look for a node with available memory
    memSegWithRoom = findFirstAvailableMemory(dataSize);

    if(memSegWithRoom){
        // An existing segment has room for the requested memory, split the segment and return the addr.
        return splitMemorySegment(memSegWithRoom, segmentSize, dataSize);
    }
        // no room found in existing segments, add a new node to end of linked list
        return addNewSegment(segmentSize, dataSize);
}


void printLinkedList(void)
{
    struct MemorySegment *memSegPtr;
    size_t i = 0;

    _Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n    | Printing memory segment linked list...\n\n", __FILE__, __LINE__, __FUNCTION__) : 0;
    memSegPtr = head;
    do
    {
        fprintf(stderr, " %ld. Segment Addr: %p\n     Segment Size: %ld\n     Data Size: %ld\n     isFree: %ld\n     Next: %p\n     Prev: %p\n", i++, memSegPtr, memSegPtr->segmentSize, memSegPtr->dataSize, memSegPtr->isFree, memSegPtr->next, memSegPtr->prev);
        memSegPtr = memSegPtr->next;
    } while (memSegPtr);
    fprintf(stderr, "\n");
}

void linkedListReset(void){
    size_t sum = sumAllSegments();
    _Verbose ? fprintf(stderr, "--> %s:%d, in %s()\n    | Filling %ld bytes with null characters\n", __FILE__, __LINE__, __FUNCTION__, sum): 0;
    memset(initialProgramBreak, '\0', sum);
    _Verbose ? fprintf(stderr, "    | Reseting heap back to initial program break: %p\n\n", initialProgramBreak): 0;
    brk(initialProgramBreak);
    head = NULL;

}

void linkedListMarkFree(void *addr){
    struct MemorySegment *memSegPtr, *segmentToFree;

    segmentToFree = (struct MemorySegment*) (addr - sizeof(struct MemorySegment));

    _Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n   | Attempting to free memory addr: %p\n", __FILE__, __LINE__, __FUNCTION__, addr) : 0;
    _Verbose ? fprintf(stderr, "   | Header addr: %p\n", segmentToFree) : 0;

    for (memSegPtr = head; memSegPtr->next; memSegPtr = memSegPtr->next)
    {
        if (memSegPtr == segmentToFree){
            _Verbose ? fprintf(stderr, "   | Marking addr %p as free\n", memSegPtr) : 0;
            memSegPtr->isFree = 1;
            
            if( (memSegPtr->prev)->isFree == 1 && (memSegPtr->next)->isFree ){
                _Verbose ? fprintf(stderr, "   | Joining 3 consecutive segments: %p, %p, %p \n\n", memSegPtr->prev, memSegPtr, memSegPtr->next) : 0;
                // free 3 segments, middle, prev and next
                joinSegments(memSegPtr->prev, memSegPtr);
                joinSegments(memSegPtr->prev, memSegPtr->next);
            }
            else if ((memSegPtr->prev)->isFree ){
                _Verbose ? fprintf(stderr, "   | Joining 2 consecutive segments: %p, %p \n\n", memSegPtr->prev, memSegPtr) : 0;
                // free 2 segments, middle and prev
                joinSegments(memSegPtr->prev, memSegPtr);
            }
            else if((memSegPtr->next)->isFree){
                _Verbose ? fprintf(stderr, "   | Joining 2 consecutive segments: %p, %p \n\n", memSegPtr, memSegPtr->next) : 0;
                // free 2 segments, middle and next 
                joinSegments(memSegPtr, memSegPtr->next);
            }
        }
    }
}