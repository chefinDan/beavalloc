#include "memorySegment.h"

// variables that represent the head and tail of the linked list
static struct MemorySegment *head = 0;
static struct MemorySegment *tail =0;


// variable to hold program break address when program is first run
static void *initialProgramBreak;
static void *maxProgramBreak;

// variable to determine verbosity of execution
static uint8_t _Verbose = 0;

// flag to indicate that the segment data is free and can be directly replaced
static uint8_t replaceSegment = 0; 

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
    { // check if the current memSeg is free and has data space for a new linkedList node and the user requested data
        if ((memSegPtr->segmentSize - memSegPtr->dataSize) >= (dataSize + MEM_SEG_INFO))
        {
            return memSegPtr; //return address of segment that has room for new data
        }

        // if (memSegPtr->isFree && memSegPtr->segmentSize >= dataSize){
        //     replaceSegment = 1;
        //     return memSegPtr; //return address of segment that has room for new data
        // }

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
    tail = newMemSeg;
    return newMemSeg + 1; // return address to beginning of data section of new memorysegment on heap
}

static struct MemorySegment 
*replaceMemorySegment(struct MemorySegment *freedMemorySegment, size_t dataSize){
    _Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n    | Replacing freed memory at %p.\n\n", __FILE__, __LINE__, __FUNCTION__, freedMemorySegment) : 0;
    freedMemorySegment->dataSize = dataSize;
    freedMemorySegment->isFree = 0;
    replaceSegment = 0;
    return freedMemorySegment +1;
}

static struct MemorySegment 
*splitMemorySegment(struct MemorySegment *memSegWithRoom, size_t segmentSize, size_t dataSize)
{
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
    newMemSeg->prev = tail;
    (newMemSeg->prev)->next = newMemSeg;
    tail = newMemSeg;

    return newMemSeg +1;
}

// segB is joined into segA. segA has it's segment size increased by segB's segment size. 
static struct MemorySegment *joinSegments(struct MemorySegment *segA, struct MemorySegment *segB){
    // point segA next to segB next 
    segA->next = segB->next;
    //
    if(segB->next)
        (segB->next)->prev = segA;

    // add segB segment size to segA's segment size
    segA->segmentSize = segA->segmentSize + segB->segmentSize;
    // segPtr = segA->prev;
    segB = NULL;
    // segA = NULL;

    return segA;
}


void setVerbose(uint8_t oneOrZero)
{
    _Verbose = oneOrZero;
}


// returns addr of data portion of memory block
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

    if(memSegWithRoom && replaceSegment){
        // An existing segment was freed and has room for the requested memory, 
        // replace the data and update the header data
        return replaceMemorySegment(memSegWithRoom, dataSize);
    }
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
    // size_t sum = sumAllSegments();
    // _Verbose ? fprintf(stderr, "--> %s:%d, in %s()\n    | Filling %ld bytes with null characters\n", __FILE__, __LINE__, __FUNCTION__, sum): 0;
    // memset(initialProgramBreak, '\0', sum);
    _Verbose ? fprintf(stderr, "    | Reseting heap back to initial program break: %p\n\n", initialProgramBreak): 0;
    brk(initialProgramBreak);
    head = NULL;
    tail = NULL;

}

void linkedListMarkFree(void *addr){
    struct MemorySegment *memSegPtr, *segmentToFree, *next, *prev;
    int freePrev, freeNext;

    segmentToFree = (struct MemorySegment*) (addr - sizeof(struct MemorySegment));

    _Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n   | Attempting to free memory addr: %p\n", __FILE__, __LINE__, __FUNCTION__, addr) : 0;
    _Verbose ? fprintf(stderr, "   | Header addr: %p\n", segmentToFree) : 0;

    for (memSegPtr = head; memSegPtr; memSegPtr = memSegPtr->next)
    {
        prev = NULL;
        next = NULL;
        freePrev = 0;
        freeNext = 0;

        // printf("=== %p\n", memSegPtr);
        if (memSegPtr == segmentToFree){
            _Verbose ? fprintf(stderr, "   | Marking addr %p as free\n", memSegPtr) : 0;
            memSegPtr->isFree = 1;
            
            if(memSegPtr->next)
                next = memSegPtr->next;
            if(memSegPtr->prev)
                prev = memSegPtr->prev;
            
            if(prev && prev->isFree)
                freePrev = 1;
            if(next && next->isFree)
                freeNext =1;

            if(freePrev && freeNext){
                _Verbose ? fprintf(stderr, "   | Joining 3 consecutive segments: %p, %p, %p \n\n", prev, memSegPtr, next) : 0;
                // free 3 segments, middle, prev and next
                joinSegments(prev, memSegPtr);
                joinSegments(prev, next);
            }
            else if (freePrev){
                _Verbose ? fprintf(stderr, "   | Joining 2 consecutive segments, prev and selected: %p, %p \n\n", prev, memSegPtr) : 0;
                // free 2 segments, middle and prev
                joinSegments(prev, memSegPtr);
            }
            else if(freeNext){
                _Verbose ? fprintf(stderr, "   | Joining 2 consecutive segments, selected and next: %p, %p \n\n", memSegPtr, next) : 0;
                // free 2 segments, middle and next 
                joinSegments(memSegPtr, next);
            }
        }
    }
}

void dumpLinkedList(uint leaks_only)
{
    struct MemorySegment *curr = NULL;
    uint i = 0;
    uint leak_count = 0;
    uint user_bytes = 0;
    uint capacity_bytes = 0;
    uint block_bytes = 0;
    uint used_blocks = 0;
    uint free_blocks = 0;


    /*** fix later ***/
    maxProgramBreak = NULL;
    /******************/

    if (leaks_only)
    {
        fprintf(stderr, "heap lost blocks\n");
    }
    else
    {
        fprintf(stderr, "heap map\n");
    }
    fprintf(stderr, "  %s\t%s\t%s\t%s\t%s"
                    "\t%s\t%s\t%s\t%s\t%s\t%s"
                    "\n",
            "blk no  ", "block add ", "next add  ", "prev add  ", "data add  "

            ,
            "blk off  ", "dat off  ", "capacity ", "size     ", "blk size ", "status   ");
    for (curr = head, i = 0; curr != NULL; curr = curr->next, i++)
    {
        if (leaks_only == 0 || (leaks_only == 1 && curr->isFree == 0))
        {
            fprintf(stderr, "  %u\t\t%9p\t%9p\t%9p\t%9p\t%u\t\t%u\t\t"
                            "%u\t\t%u\t\t%u\t\t%s\t%c\n",
                    i, curr, curr->next, curr->prev, curr +1, (unsigned)((void *)curr - initialProgramBreak), (unsigned)((void *)curr->dataSize - initialProgramBreak), (unsigned)curr->segmentSize, (unsigned)curr->dataSize, (unsigned)(curr->segmentSize + sizeof(struct MemorySegment)), curr->isFree ? "free  " : "in use", curr->isFree ? '*' : ' ');
            user_bytes += curr->dataSize;
            capacity_bytes += curr->segmentSize;
            block_bytes += curr->segmentSize + sizeof(struct MemorySegment);
            if (curr->isFree == 0 && leaks_only == 1)
            {
                leak_count++;
            }
            if (curr->isFree == 1)
            {
                free_blocks++;
            }
            else
            {
                used_blocks++;
            }
        }
    }
    if (leaks_only)
    {
        if (leak_count == 0)
        {
            fprintf(stderr, "  *** No leaks found!!! That does NOT mean no leaks are possible. ***\n");
        }
        else
        {
            fprintf(stderr, "  %s\t\t\t\t\t\t\t\t\t\t\t\t"
                            "%u\t\t%u\t\t%u\n",
                    "Total bytes lost", capacity_bytes, user_bytes, block_bytes);
        }
    }
    else
    {
        fprintf(stderr, "  %s\t\t\t\t\t\t\t\t\t\t\t\t"
                        "%u\t\t%u\t\t%u\n",
                "Total bytes used", capacity_bytes, user_bytes, block_bytes);
        fprintf(stderr, "  Used blocks: %u  Free blocks: %u  "
                        "Min heap: %p    Max heap: %p\n",
                used_blocks, free_blocks, initialProgramBreak, maxProgramBreak);
    }
}

void *linkedListResize(void *dataAddr, size_t size){
    struct MemorySegment *segmentToResizeHeader, *newSegmentData;
    segmentToResizeHeader = (struct MemorySegment *)(dataAddr - sizeof(struct MemorySegment));

    if(segmentToResizeHeader->segmentSize < size){
        _Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n   | Segment %p has isufficient space for resize. Will allocate elsewhere and move data\n", __FILE__, __LINE__, __FUNCTION__, segmentToResizeHeader) : 0;
        newSegmentData = addToLinkedList(size, size);
        memcpy(newSegmentData, segmentToResizeHeader +1, segmentToResizeHeader->segmentSize);
        linkedListMarkFree(segmentToResizeHeader +1);
        return newSegmentData - sizeof(struct MemorySegment);
    }
    else{
        _Verbose ? fprintf(stderr, "-> %s:%d, in %s()\n   | Segment %p has room for resize. Will increase data size\n", __FILE__, __LINE__, __FUNCTION__, segmentToResizeHeader) : 0;
        segmentToResizeHeader->dataSize = size;
        return dataAddr;
    }
    
}
