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
#include <pthread.h>
#include <stdint.h>

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
    _Bool initalized;
};

struct Myalloc myalloc;

pthread_mutex_t mut; 


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
    if(myalloc.initalized)
        {
        printf("myalloc already initialized for %d bytes of memory\n", myalloc.size);
        }
    else if(_size > HEADER_SIZE)    //Size must be at least 8 bytes
        {
        if(0 != pthread_mutex_init(&mut, NULL))
            {
            printf("Intialization Failed: Mutex init failed\n");
            exit(ERROR);
            }

        //Define myalloc parameters
        myalloc.aalgorithm = _aalgorithm;
        myalloc.size = _size;
        myalloc.allocList = NULL;

        //Allocate memory
        if(NULL == (myalloc.memory = malloc((size_t)myalloc.size)))
            {
            printf("Initialization memory allocation failed.\n");
            exit(ERROR);
            }
        
        //Pre-fault memory
        memset(myalloc.memory, 0, _size);

        //Set the first header of the memory
        int64_t freeSpace = (int64_t)(_size - HEADER_SIZE);
        memcpy(myalloc.memory, &freeSpace, HEADER_SIZE);

        //Create freeList header in memory block
        myalloc.freeList = List_createNode((int64_t*)myalloc.memory);
        }
    else 
        {
        printf("Memory block requested is too small\n");
        exit(ERROR);
        }
    }

/*******************************************************************
** destroy_allocator -- frees all allocated memory
**
** @param[in]  none           
**
********************************************************************/
void destroy_allocator() 
    {
    //Release the memory
    free(myalloc.memory);
    
    //Release free linked list
    struct memHead* freeBlock = myalloc.freeList;
    struct memHead* toFree = myalloc.freeList;
    while(freeBlock != NULL)
        {
        freeBlock = freeBlock->next;
        free(toFree);
        toFree = freeBlock;
        }

    //Release allocated linked list
    struct memHead* allocBlock = myalloc.allocList;
    toFree = myalloc.allocList;
    while(allocBlock != NULL)
        {
        allocBlock = allocBlock->next;
        free(toFree);
        toFree = allocBlock;
        }
    }


/*******************************************************************
** allocate -- allocated a specific size of memory 
**
** @param[in]  _size        size of memory to be allocated           
**
********************************************************************/
void* allocate(int _size) 
    {
    //Lock section
    pthread_mutex_lock(&mut);

    void* ptr = NULL;
    struct memHead* freeBlock = myalloc.freeList;
    struct memHead* availableMem = NULL;

    switch(myalloc.aalgorithm)
        {
        case FIRST_FIT:
            {
            //Need sorted list 
            List_sort(&myalloc.freeList);

            //Reassign free block in case head changed 
            freeBlock = myalloc.freeList;

            //Loop over free memory blocks
            while(freeBlock != NULL)
            	{
                //If the desired block fits in the free block
                if( _size == *(freeBlock->curr) || (_size + HEADER_SIZE) <= *(freeBlock->curr))
                    {
                    //Set the return address and exit from loop to set the header
                    ptr = (void*)((char*)freeBlock->curr + HEADER_SIZE);
                    availableMem = freeBlock;
                    break;
                    }
                else
                    {
                    //Move to next free block to check if it will fit element
                    freeBlock = freeBlock->next;
                    }
            	}

            break;
            }
        case BEST_FIT:
            {
            int64_t smallestDiff = myalloc.size;    //Set to some default value that will be overwritten
            struct memHead* freeBlock = myalloc.freeList;
            availableMem = NULL;

            //Loop over free memory blocks
            while(freeBlock != NULL)
            	{
                //If the requested block size is closest to the availible memory chunk size
                int64_t sizeDiff = *(freeBlock->curr) - (int64_t)(_size + HEADER_SIZE);
                if(sizeDiff <= smallestDiff && sizeDiff >= 0)
                    {
                    availableMem = freeBlock;
                    ptr = (void*)((char*)freeBlock->curr + HEADER_SIZE);
                    smallestDiff = *(freeBlock->curr) - (_size + HEADER_SIZE);
                    }
                else if(*(freeBlock->curr) - (int64_t)_size == 0)
                    {
                    availableMem = freeBlock;
                    ptr = (void*)((char*)freeBlock->curr + HEADER_SIZE);
                    smallestDiff = 0;
                    break;      //Can't find anything
                    }
                
                //Move to next block
                freeBlock = freeBlock->next;
            	}
            break;
            }
        case WORST_FIT:
            {
            int64_t largestDiff = 0;    //Set to some default value that will be overwritten
            struct memHead* freeBlock = myalloc.freeList;
            availableMem = NULL;

            //Loop over free memory blocks
            while(freeBlock != NULL)
            	{
                //If the requested block size is farthest to the availible memory chunk size
                if(*(freeBlock->curr) - (int64_t)(_size + HEADER_SIZE) >= largestDiff)
                    {
                    availableMem = freeBlock;
                    ptr = (void*)((char*)freeBlock->curr + HEADER_SIZE);
                    largestDiff = *(freeBlock->curr) - (_size + HEADER_SIZE);
                    }
                else if(*(freeBlock->curr) - (int64_t)_size == 0 && largestDiff == 0)
                    {
                    availableMem = freeBlock;
                    ptr = (void*)((char*)freeBlock->curr + HEADER_SIZE);
                    }

                //Move to next block
                freeBlock = freeBlock->next;
            	}
            break;
            }
        default:
            {
            printf("Invalid algorithm provided. Cannot allocate memory\n");
            }
        }

    //Update the freeList and create allocated node in allocList
    if(availableMem != NULL)
        List_freeToAlloc(&myalloc.freeList, &myalloc.allocList, availableMem, _size);

    //Unlock section
    pthread_mutex_unlock(&mut);

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
    if(_ptr != NULL)
        {
        //Lock section
        pthread_mutex_lock(&mut);

        //Get reference to header
        struct memHead* allocBlock = List_findAllocNode(&myalloc.allocList, _ptr);

        //If we couldn't find the node in the alloc list
        if(allocBlock == NULL)
            {
            printf("Memory could not be found. Unable to deallocate\n");
            exit(ERROR);
            }

        //Remove the allocated block and add it to the free list
        List_allocToFree(&myalloc.allocList, &myalloc.freeList, allocBlock);

        //Clear allocated memory
        memset(_ptr, 0, *(allocBlock->curr));

        //Check for back to back free blocks in free list
        List_combineNeighbours(&myalloc.freeList, allocBlock);

        //Unlock section
        pthread_mutex_unlock(&mut);
        }
    else 
        {
        printf("Memory to deallocate does not exist.\n");
        }
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
    //Lock section
    pthread_mutex_lock(&mut);

    int64_t freeMemSize = 0;
    void* nextFreeMem = NULL;           //This points to the header location

    //Sort the free and alloc header lists
    List_sort(&myalloc.allocList);
    List_sort(&myalloc.freeList);

    int ptrNum = 0;
    struct memHead* allocHeader = myalloc.allocList;
    struct memHead* freeHeader = myalloc.freeList;

    while(allocHeader != NULL)
        {
        if(freeHeader == NULL || allocHeader->curr < freeHeader->curr)
            {
            //Set the location of the memory before compaction
            _before[ptrNum] = (void*)((char*)allocHeader->curr + HEADER_SIZE);
       
            if(nextFreeMem == NULL)
                {
                //We are not moving the memory, set the after pointer
                _after[ptrNum] = (void*)((char*)allocHeader->curr + HEADER_SIZE);
                }
            else 
                {
                //Get the current size of the data block 
                int64_t currSize = *(allocHeader->curr);

                //Copy the data to the new location 
                memmove(nextFreeMem, allocHeader->curr, currSize + HEADER_SIZE);

                //Set the after pointer for the user
                _after[ptrNum] = (void*)((char*)nextFreeMem + HEADER_SIZE);

                //Update the allocList
                allocHeader->curr = (int64_t*)nextFreeMem;

                //Set the location of the nextFreeMem and the headers size
                nextFreeMem = (void*)((char*)nextFreeMem + currSize + HEADER_SIZE);
                }
            
            //Update the current alloc header
            allocHeader = allocHeader->next;
            ptrNum++;
            }
        else if(allocHeader->curr >= freeHeader->curr)
            {
            //Increase the free memory size
            freeMemSize += *(freeHeader->curr) + HEADER_SIZE;

            if(nextFreeMem == NULL)
                nextFreeMem = (void*)freeHeader->curr;
           
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
            freeMemSize += *(freeHeader->curr);
           
            //Move our free header to next node in the free list
            freeHeader = freeHeader->next;
            }
        }

    //If nextFreeMem is NULL then all the free memory is at the end of the memory block
    //      no chnage to free list is needed
    if(nextFreeMem != NULL)
        {
        //Remove elements from free list
        List_delete(&myalloc.freeList);
    
        //Condense remaining free memeory and set the free memory head
//        myalloc.freeList = nextFreeMem;

        //Set the size of the free block
        memcpy(nextFreeMem, &freeMemSize, HEADER_SIZE);

        //Set the free memory header
        struct memHead* newHeader = List_createNode((int64_t*) nextFreeMem);

        myalloc.freeList = newHeader;
        memset(myalloc.freeList->curr + HEADER_SIZE, 0, freeMemSize - HEADER_SIZE);
        }
    
    //Unlock section
    pthread_mutex_unlock(&mut);

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
    //Lock section
    pthread_mutex_lock(&mut);

    int64_t available_memory_size = 0;

    //Calculate available memory size
    struct memHead* freeHead = myalloc.freeList;

    while(freeHead != NULL)
        {
        available_memory_size += *(freeHead->curr);     //Increase free memory amount 
        freeHead = freeHead->next;                      //Move to next block of free memory
        }
    
    //Unlock section
    pthread_mutex_unlock(&mut);

    return (int)available_memory_size;
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

    //Lock section
    pthread_mutex_lock(&mut);

    // Calculate the statistics
    getAllocatedStats(myalloc.allocList, &allocated_size, &allocated_chunks);
    getFreeMemStats(myalloc.freeList, &free_size, &free_chunks, &smallest_free_chunk_size, &largest_free_chunk_size);

    //Unlock section
    pthread_mutex_unlock(&mut);

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
    int64_t aSize = 0;
    int aChunks = 0;

    while(currHead != NULL)
        {
        aSize += *(currHead->curr);
        aChunks++;

        currHead = currHead->next;
        }

    //Return values
    *allocSize = (int)aSize;
    *allocChunks = aChunks;
    }


/*******************************************************************
** getFreeMemStats -- determines the allocated size and number of chucks
**
** @param[in]   freeList        Linked list of free blocks 
** @param[out]  freeSize        size of free memory
** @param[out]  freeChunks      number of free chunks 
** @param[out]  smallestChunk   smallest free chunk
** @param[out]  largestChunk    largest free chunk 
**
********************************************************************/
void getFreeMemStats(struct memHead* freeList, int* freeSize, int* freeChunks, int* smallestChunk, int* largestChunk) 
    {
    struct memHead* currHead = freeList;
    int64_t fSize = 0;
    int fChunks = 0;
    int64_t sChunk = freeList == NULL ? 0 : *(freeList->curr);
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
    *freeSize = (int)fSize;
    *freeChunks = fChunks;
    *smallestChunk = (int)sChunk;
    *largestChunk = lChunk;
    }
