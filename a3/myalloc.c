/*-----------------------------------------------------------------------------
 * myalloc.c -- memory allocator for assignment 3
 *
 * 1 Apr 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "myalloc.h"


/***DEFINES*******************************************************************/
#define ERROR              -1
#define SUCCESS             0
#define FAILURE             1
//#define DEBUG     //For debugging 

#define HEADER_SIZE  sizeof(int)

/***GLOBAL VARIABLES**********************************************************/
struct memHead{
    int* curr;
    struct memHead* next;
};

struct Myalloc {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    struct memHead* freeList;
    struct memHead* allocList;
};

struct Myalloc myalloc;


/***FUNCTION DECLARATIONS*****************************************************/
//See myalloc.h

void getAllocatedStats(struct memHead* allocList, int* allocSize, int* allocChunks);

void getFreeMemStats(struct memHead* freeList, int* freeSize, int* freeChunks, int* smallestChunk, int* largestChunk); 

void setNewMemHead(struct memHead* newNode, int size, struct memHead** oldHead);

void alterMemHead(struct memHead** listHead, struct memHead* toAlter, int size);

void removeMemHead(struct memHead** listHead, struct memHead* toRemove);

void sortList(struct memHead** head);


/***FUNCTION DEFINITIONS******************************************************/

/*******************************************************************
** initialize_allocator -- initializes the memory allocator
**
** @param[in]  _size            the total size of the program expected
** @param[in]  _aalgorithm      the algorithm to be used for the allocation
**
********************************************************************/
void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) 
    {
    assert(_size > HEADER_SIZE);     //Will this pass the test cases
    myalloc.aalgorithm = _aalgorithm;
    myalloc.size = _size;
    myalloc.allocList = NULL;

    //Allocate memory and set it to empty 
    if(-1 == (myalloc.memory = malloc((size_t)myalloc.size)))
        {
        printf("Initialization memory allocation failed.\n");
        exit(ERROR);
        }
    
    memset(myalloc.memory, 0, _size);


    //Set the first header of the memory
    int freeSpace = _size - HEADER_SIZE;
    memcpy(myalloc.memory, &freeSpace, HEADER_SIZE);

    //Create freeList header in memory block
    struct memHead* first = malloc(sizeof(struct memHead));
    first->curr = (int*) myalloc.memory;        
    first->next = NULL;

    myalloc.freeList = &first;
    }

/*******************************************************************
** destroy_allocator -- frees all allocated memory
**
** @param[in]  none           
**
********************************************************************/
void destroy_allocator() 
    {
    free(myalloc.memory);

    //No other operations are needed. 
    //All memory is contained in myalloc.memory
    }


/*******************************************************************
** allocate -- allocated a specific size of memory 
**
** @param[in]  _size        size of memory to be allocated           
**
********************************************************************/
void* allocate(int _size) 
    {
    void* ptr = NULL;

    switch(myalloc.aalgorithm)
        {
        case FIRST_FIT:
            {
            struct memHead* availableMem = myalloc.freeList;

            if(availableMem != NULL)
            	{
                do
                    {
                    if(_size <= availableMem->size)
                        {
                        ptr = availableMem + HEADER_SIZE;

                        //Alter or remove the memHead from the freeList
                        if(_size == availableMem->size)
                        	removeMemHead(&myalloc.freeList, availableMem);
                        else
                        	alterMemHead(&myalloc.freeList, availableMem, _size);

                        //Set the header values and update the head of our alloc list
                        setNewMemHead(availableMem, _size, &myalloc.allocList);
                        break;
                        }
                    else
                        {
                        availableMem = availableMem->next;
                        }
                    } while(availableMem->next != NULL);
            	}

            break;
            }
        case BEST_FIT:
            break;

        case WORST_FIT:
            break;
        }

    return ptr;
    }


/*******************************************************************
** deallocate -- deallocated memory pointed to by _ptr 
**
** @param[in]  _ptr     pointer to memory to be freed          
**
********************************************************************/
void deallocate(void* _ptr) 
    {
    assert(_ptr != NULL);

    //Get reference to header
    struct memHead* oldHead = ((struct memHead*)_ptr) - HEADER_SIZE;

    //Remove the memHead from the allocList
    removeMemHead(&myalloc.allocList, oldHead);

    //Clear allocated memory
    memset(_ptr, 0, oldHead->size);

    //Set the header values and update the head of our free list
    setNewMemHead(oldHead, oldHead->size, &myalloc.freeList);
    }


/*******************************************************************
** compact_allocation -- groups allocated memory blocks to compact memory block
**
** @param[in]   _before     array of pointers to start of memory blocks 
** @param[out]  _after      array of pointers to start of memory 
**                          blocks after compaction
**
********************************************************************/
int compact_allocation(void** _before, void** _after) 
    {
    int freeMemSize = 0;
    void* nextFreeMem = NULL;           //This points to the header location

    //Sort the free and alloc header lists
    sortList(&myalloc.allocList);
    sortList(&myalloc.freeList);

    int ptrNum = 0;
    struct memHead* allocHeader = myalloc.allocList;
    struct memHead* freeHeader = myalloc.freeList;
    struct memHead* prevAllocHeader = NULL;

    while(allocHeader != NULL)
        {
        if(freeHeader == NULL || allocHeader < freeHeader)
            {
            //Set the location of the memory before compaction
            _before[ptrNum] = allocHeader + HEADER_SIZE;
       
            if(nextFreeMem == NULL)
                {
                //We are not moving the memory, set the after pointer
                _after[ptrNum] = allocHeader + HEADER_SIZE;
                }
            else 
                {
                //Copy the data to the new location 
                memmove(nextFreeMem, allocHeader, allocHeader->size + HEADER_SIZE);

                //Set the after pointer for the user
                _after[ptrNum] = nextFreeMem + HEADER_SIZE;

                //Update the allocList
                if(prevAllocHeader != NULL)
                    prevAllocHeader->next = nextFreeMem;
                else 
                    myalloc.allocList = nextFreeMem;

                //Set the location of the nextFreeMem and the headers size
                nextFreeMem += allocHeader->size + HEADER_SIZE;
                }
            
            //Update the current alloc header
            prevAllocHeader = allocHeader;
            allocHeader = allocHeader->next;
            ptrNum++;
            }
        else if(allocHeader >= freeHeader)
            {
            //Increase the free memory size
            freeMemSize += ((struct memHead*)nextFreeMem)->size + HEADER_SIZE;
            
            //Move our free header to next open space
            freeHeader = freeHeader->next;
            }
        else 
            {
            //Something is wrong 
            assert(false);
            }
        }

    //Condense remaining free memeory and set the free memory head
    myalloc.freeList = nextFreeMem;
    if(nextFreeMem != NULL)
        {
        //Set the free memory header
        struct memHead freeHeader = {.size = freeMemSize - HEADER_SIZE, .next = NULL};

        memcpy(myalloc.freeList, &freeHeader, HEADER_SIZE);
        memset(myalloc.freeList + HEADER_SIZE, 0, freeMemSize - HEADER_SIZE);
        }
    

    return ptrNum;
    } 


/*******************************************************************
** available_memory -- reports the availible memory 
**
** @param[in]  none 
**
********************************************************************/
int available_memory() 
    {    
    int available_memory_size = 0;

    //Calculate available memory size
    struct memHead* freeHead = myalloc.freeList;

    if(freeHead != NULL)
        {
        do
            {
            available_memory_size += freeHead->size;    //Increase free memory amount 
            freeHead = freeHead->next;                  //Move to next block of free memory
            } 
            while(freeHead->next != NULL);
        }
    
    return available_memory_size;
    }


/*******************************************************************
** print_statistics -- reports the statistics of the memory allocator 
**
** @param[in]  none 
**
********************************************************************/
void print_statistics() 
    {
    int allocated_size = 0;
    int allocated_chunks = 0;
    int free_size = 0;
    int free_chunks = 0;
    int smallest_free_chunk_size = 0;
    int largest_free_chunk_size = 0;

    // Calculate the statistics
    printf("test\n");
    getAllocatedStats(myalloc.allocList, &allocated_size, &allocated_chunks);
printf("test2\n");
    getFreeMemStats(myalloc.allocList, &free_size, &free_chunks, &smallest_free_chunk_size, &largest_free_chunk_size);

printf("test3\n");
    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
    }


/*******************************************************************
** getAllocatedStats -- determines the allocated size and number of chucks
**
** @param[in]  allocList        Linked list of allocated blocks 
** @param[out]  allocSize       size of allocated memory
** @param[out]  allocChunks     number of allocated chunks 
**
********************************************************************/
void getAllocatedStats(struct memHead* allocList, int* allocSize, int* allocChunks) 
    {
    struct memHead* currHead = allocList;
    int aSize = 0;
    int aChunks = 0;

    printf("start of currentHead: %p\n", currHead);
    while(currHead != NULL)
        {
        aSize += currHead->size;
        aChunks++;

        printf("currentHead: %p\n", currHead);

        if(aChunks > 50)
            break;

        currHead = currHead->next;
        }

    //Return values
    *allocSize = aSize;
    *allocChunks = aChunks;
    }


/*******************************************************************
** getAllocatedStats -- determines the allocated size and number of chucks
**
** @param[in]  freeList         Linked list of free blocks 
** @param[out]  freeSize        size of free memory
** @param[out]  freeChunks      number of free chunks 
** @param[out]  smallestChunk   smallest free chunk
** @param[out]  largestChunk    largest free chunk 
**
********************************************************************/
void getFreeMemStats(struct memHead* freeList, int* freeSize, int* freeChunks, int* smallestChunk, int* largestChunk) 
    {
    struct memHead* currHead = freeList;
    int fSize = 0;
    int fChunks = 0;
    int sChunk = freeList == NULL ? 0 : freeList->size;
    int lChunk = 0;

    while(currHead != NULL)
        {
        //Increment the total size and number of chunks
        fSize += currHead->size;
        fChunks++;

        //Determine if chunk is smallest
        if(sChunk > currHead->size)
            sChunk = currHead->size;

        //Determine if chunk is largest
        if(lChunk > currHead->size)
            lChunk = currHead->size;

        //Move to next element
        currHead = currHead->next;
        }

    //Return values
    *freeSize = fSize;
    *freeChunks = fChunks;
    *smallestChunk = sChunk;
    *largestChunk = lChunk;
    }


/*******************************************************************
** setNewMemHead -- will set a new header of the specified list 
**
** @param[in]  newNode          memHead node to be set as the new head 
** @param[in]  newNodeSize      the size of the memHead to be set 
** @param[in]  oldHead          The old head to be switched with newNode
**
********************************************************************/
void setNewMemHead(struct memHead* newNode, int size, struct memHead** oldHead) 
    {
    newNode->size = size;
    newNode->next = *oldHead;
    *oldHead = newNode;
    }


/*******************************************************************
** setNewMemHead -- will set a new header of the specified list 
**
** @param[in]  listHead     head of the list to remove element from
** @param[in]  toRemove     node of list to be removed 
**
********************************************************************/
void removeMemHead(struct memHead** listHead, struct memHead* toRemove) 
    {
    //Start at head of list
    struct memHead* currHead = *listHead;

    //Make sure the head is not what we are removing
    if(*listHead != toRemove)
        {
        while(currHead->next != toRemove)
            {
            currHead = currHead->next;
            }

        //Change the next pointer to skip over deallocated block
        currHead->next = toRemove->next;
        }
    else 
        {
        //Change the head of the list
        *listHead = toRemove->next;
        }
    }


/*******************************************************************
** alterMemHead -- Moves the available memory header to a new location
**
** @param[in]  listHead     head of the list to alter an element from
** @param[in]  toAlter      node of list to be altered
** @param[in]  size         size of memory taken away from the free block
**
********************************************************************/
void alterMemHead(struct memHead** listHead, struct memHead* toAlter, int size)
    {
    //Start at head of list
    struct memHead* currHead = *listHead;

    //Make sure the head is not what we are altering
    if(*listHead != toAlter)
        {
    	//Find toAlter node in the list
        while(currHead->next != toAlter)
            {
            currHead = currHead->next;
            }

        //Change the next pointer to point to the block following this
        memcpy(toAlter, toAlter + size + HEADER_SIZE, HEADER_SIZE);

        //Keep linked list linked
        currHead->next = toAlter + size;

        //Fix size change when allocating this block
        (toAlter+size)->size = toAlter->size - size - HEADER_SIZE;
        }
    else
        {
        //Change the head of the list
        *listHead = toAlter + size + HEADER_SIZE;
        (*listHead)->size = toAlter->size - size - HEADER_SIZE;
        }
    }

/*******************************************************************
** compact_allocation -- groups allocated memory blocks to compact memory block
**
** @param[in]   listHead       head of linked list to be sorted
**
********************************************************************/
void sortList( struct memHead **listHead )
    {
    struct memHead* oldListHead = *listHead;
    struct memHead* tempHead = NULL;
    struct memHead* tempTail = NULL;
    void* traverser;
    void* traverser_prev;
    void* leastAddr = oldListHead;
    void* leastAddr_prev = NULL;

    //While our oldList still points to memHeads
    while(oldListHead != NULL)
        {
        traverser = oldListHead;

        //Find the smallest address 
        while(traverser != NULL)
            {
            if(traverser <= leastAddr)
                {
                leastAddr_prev = traverser_prev;
                leastAddr = traverser;              //Set the smallest address
                }

            traverser_prev = traverser;
            traverser = ((struct memHead*)traverser)->next;
            }
        //We now have the smallest address

        //Remove the element from the list -- special case if element was head
        if(leastAddr == oldListHead)
            {
            oldListHead = oldListHead->next;
            }
        else 
            {
            ((struct memHead*)leastAddr_prev)->next = ((struct memHead*)leastAddr)->next;
            }

        //Move element to new list -- specical case if new list is empty
        if(tempHead == NULL)
            {
            tempHead = (struct memHead*)leastAddr;
            tempTail = (struct memHead*)leastAddr;
            ((struct memHead*)leastAddr)->next = NULL;
            }
        else 
            {
            tempTail->next = (struct memHead*)leastAddr;
            tempTail = tempTail->next;
            ((struct memHead*)leastAddr)->next = NULL;
            }
        }

    //Set the new header of the list
    *listHead = tempHead;
    }
