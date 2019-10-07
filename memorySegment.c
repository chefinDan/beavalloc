#include "memorySegment.h"

// variable that represents the head of the linked list
static struct MemorySegment *head = 0;

static void *initialProgramBreak;

// variable to determine verbosity of execution
uint8_t Verbose = 0;

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

static struct MemorySegment *findAvailableMemory(size_t dataSize)
{
    struct MemorySegment *memSegPtr;

    Verbose ? fprintf(stderr, "--> %s:%d, in %s()\n    | Traversing memory segment linked list looking for available memory\n", __FILE__, __LINE__, __FUNCTION__) : 0;
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
    Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n   | Traversing memory segment linked list looking for last segment\n", __FILE__, __LINE__, __FUNCTION__) : 0;

    memSegPtr = head;
    do
    {
        if (memSegPtr->next == NULL)
        {
            return memSegPtr;
        }
    } while (memSegPtr);

    return NULL;
}


void setVerbose(uint8_t oneOrZero)
{
    Verbose = oneOrZero;
}


void *addToLinkedList(size_t segmentSize, size_t dataSize){
    struct MemorySegment *newMemSeg, *memSegWithRoom = 0;
    if (head == 0)
    {
        // Start of linked list.
        // Increase heap to include requested memory and MemorySegmentData struct
        Verbose ? fprintf(stderr, "--> %s:%d, in %s()\n    | First allocation, initializing head of memory segment linked list\n", __FILE__, __LINE__, __FUNCTION__) : 0;
        initialProgramBreak = sbrk(ZERO);
        Verbose ? fprintf(stderr, "    | Saving initial program break: %p\n", initialProgramBreak) : 0;

        newMemSeg = (struct MemorySegment*) sbrk(MEM_SEG_INFO + segmentSize);
        Verbose ? fprintf(stderr, "    | Heap increased by %ld + %ld bytes\n\n", segmentSize, MEM_SEG_INFO) : 0;
        newMemSeg->segmentSize = segmentSize;
        newMemSeg->dataSize = dataSize;
        newMemSeg->isFree = 0;
        newMemSeg->next = 0;
        newMemSeg->prev = 0;
        head = newMemSeg;
        return newMemSeg + 1; // return address to beginning of data section of new memorysegment on heap
    }
    
    // else head points to a memorySegment Node
    // first look for a node with available memory
    memSegWithRoom = findAvailableMemory(dataSize);

    if(memSegWithRoom){
        // Found memory segment with room for new data
        Verbose ? fprintf(stderr, "Line %d: Adding new node to memory segment linked list on existing heap\n", __LINE__): 0;
        newMemSeg = (memSegWithRoom + 2); // point newMemSegData to end of previous data section

        // newMemSeg's segment is the size of the xisting segment minus the existing segment data size
        newMemSeg->segmentSize = memSegWithRoom->segmentSize - memSegWithRoom->dataSize;

        // reduce the size of the existing segment size to be the size of the data stored there,
        // Essentially making the segment "Full", no more room.
        memSegWithRoom->segmentSize = memSegWithRoom->dataSize;

        newMemSeg->dataSize = dataSize;
        newMemSeg->isFree = 0;
        newMemSeg->prev = memSegWithRoom;
        newMemSeg->next = memSegWithRoom->next;
        memSegWithRoom->next = newMemSeg;
    }
    else
    {
        // no room found in existing segments 
        Verbose ? fprintf(stderr, "Line %d: No space found in exsiting heap, adding new node to memory segment linked list\n", __LINE__): 0;
        newMemSeg = sbrk(MEM_SEG_INFO + segmentSize);
        Verbose ? fprintf(stderr, "Line %d: Heap increased by %ld + %ld bytes\n", __LINE__, segmentSize, MEM_SEG_INFO): 0;
        newMemSeg->segmentSize = segmentSize;
        newMemSeg->dataSize = dataSize;
        newMemSeg->isFree = 0;
        newMemSeg->next = NULL;
        newMemSeg->prev = findLastSegment();
        (newMemSeg->prev)->next = newMemSeg;
    }

    return newMemSeg + 1;
}


void printLinkedList(void)
{
    struct MemorySegment *memSegPtr;
    size_t i = 0;

    Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n    | Printing memory segment linked list...\n\n", __FILE__, __LINE__, __FUNCTION__) : 0;
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
    Verbose ? fprintf(stderr, "--> %s:%d, in %s()\n    | Filling %ld bytes with null characters\n", __FILE__, __LINE__, __FUNCTION__, sum): 0;
    memset(initialProgramBreak, '\0', sum);
    Verbose ? fprintf(stderr, "    | Reseting heap back to initial program break: %p\n\n", initialProgramBreak): 0;
    brk(initialProgramBreak);
    head = NULL;

}