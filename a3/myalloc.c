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
#include "list.h"


/***DEFINES*******************************************************************/
#define ERROR              -1
#define SUCCESS             0
#define FAILURE             1
//#define DEBUG     //For debugging 

/***GLOBAL VARIABLES**********************************************************/
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

void removeMemHead(struct memHead** listHead, struct memHead* toRemove);


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
    if(NULL == (myalloc.memory = malloc((size_t)myalloc.size)))
        {
        printf("Initialization memory allocation failed.\n");
        exit(ERROR);
        }
    
    memset(myalloc.memory, 0, _size);


    //Set the first header of the memory
    int freeSpace = _size - HEADER_SIZE;
    memcpy(myalloc.memory, &freeSpace, HEADER_SIZE);

    //Create freeList header in memory block
    myalloc.freeList = List_createNode((int*)myalloc.memory);
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

    //Free linked lists... 
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

            //Loop over free memory blocks
            while(availableMem != NULL)
            	{
                //If the desired block fits in the free block
                if(_size <= *(availableMem->curr))
                    {
                    //Set the return address and exit from loop to set the header
                    ptr = availableMem->curr + HEADER_SIZE;
                    break;
                    }
                else
                    {
                    //Move to next free block to check if it will fit element
                    availableMem = availableMem->next;
                    }
            	}

                //Update the freeList and create allocated node in allocList
                if(availableMem != NULL)
                    List_freeToAlloc(&myalloc.freeList, &myalloc.allocList, availableMem, _size);

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
    struct memHead* oldHead = (struct memHead*)((int*)_ptr - HEADER_SIZE);

    //Remove the allocated block and add it to the free list
    List_allocToFree(&myalloc.allocList, &myalloc.freeList, oldHead);

    //Clear allocated memory
    memset(_ptr, 0, *(oldHead->curr));

    //Check for back to back free blocks in free list
    List_combineNeighbours(&myalloc.freeList, oldHead);
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
    List_sort(&myalloc.allocList);
    List_sort(&myalloc.freeList);

    int ptrNum = 0;
    struct memHead* allocHeader = myalloc.allocList;
    struct memHead* freeHeader = myalloc.freeList;
    struct memHead* prevAllocHeader = NULL;

    while(allocHeader != NULL)
        {
        if(freeHeader == NULL || allocHeader->curr < freeHeader->curr)
            {
            //Set the location of the memory before compaction
            _before[ptrNum] = allocHeader->curr + HEADER_SIZE;
       
            if(nextFreeMem == NULL)
                {
                //We are not moving the memory, set the after pointer
                _after[ptrNum] = allocHeader->curr + HEADER_SIZE;
                }
            else 
                {
                //Get the current size of the data block 
                int currSize = *(allocHeader->curr);

                //Copy the data to the new location 
                memmove(nextFreeMem, allocHeader->curr, currSize + HEADER_SIZE);

                //Set the after pointer for the user
                _after[ptrNum] = nextFreeMem + HEADER_SIZE;

                //Update the allocList
                allocHeader->curr = nextFreeMem;

                //Set the location of the nextFreeMem and the headers size
                nextFreeMem += currSize + HEADER_SIZE;
                }
            
            //Update the current alloc header
            prevAllocHeader = allocHeader;
            allocHeader = allocHeader->next;
            ptrNum++;
            }
        else if(allocHeader->curr >= freeHeader->curr)
            {
            //Increase the free memory size
            freeMemSize += *(freeHeader->curr) + HEADER_SIZE;

            if(nextFreeMem == NULL)
                nextFreeMem = freeHeader->curr;
           
            //Move our free header to next node in the free list
            freeHeader = freeHeader->next;
            }
        else 
            {
            //Something is wrong 
            assert(false);
            }
        }

    //Determine total free memory size not already counted
    if(freeHeader != NULL)
        {
        //Calculate rest of free memory size 
        while(freeHeader != NULL)
            {
            //Increase the free memory size
            freeMemSize += *(freeHeader->curr) + HEADER_SIZE;
           
            //Move our free header to next node in the free list
            freeHeader = freeHeader->next;
            }
        }

    //Remove elements from free list
    List_delete(&myalloc.freeList);
   
    //Condense remaining free memeory and set the free memory head
    myalloc.freeList = nextFreeMem;
    if(nextFreeMem != NULL)
        {
        //Set the size of the free block
        memcpy(nextFreeMem, &freeMemSize, HEADER_SIZE);

        //Set the free memory header
        struct memHead* newHeader = List_createNode((int*) nextFreeMem);

        myalloc.freeList = newHeader;
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
            available_memory_size += *(freeHead->curr);     //Increase free memory amount 
            freeHead = freeHead->next;                      //Move to next block of free memory
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
    getAllocatedStats(myalloc.allocList, &allocated_size, &allocated_chunks);
    getFreeMemStats(myalloc.freeList, &free_size, &free_chunks, &smallest_free_chunk_size, &largest_free_chunk_size);

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

    while(currHead != NULL)
        {
        aSize += *(currHead->curr);
        aChunks++;

        currHead = currHead->next;
        }

    //Return values
    *allocSize = aSize;
    *allocChunks = aChunks;
    }


/*******************************************************************
** getFreeMemStats -- determines the allocated size and number of chucks
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
    int sChunk = freeList == NULL ? 0 : *(freeList->curr);
    int lChunk = 0;

    while(currHead != NULL)
        {
        //Increment the total size and number of chunks
        fSize += *(currHead->curr);
        fChunks++;

        //Determine if chunk is smallest
        if(sChunk > *(currHead->curr))
            sChunk = *(currHead->curr);

        //Determine if chunk is largest
        if(lChunk < *(currHead->curr))
            lChunk = *(currHead->curr);

        //Move to next element
        currHead = currHead->next;
        }

    //Return values
    *freeSize = fSize;
    *freeChunks = fChunks;
    *smallestChunk = sChunk;
    *largestChunk = lChunk;
    }
