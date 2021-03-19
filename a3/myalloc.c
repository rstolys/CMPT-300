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
#include "myalloc.h"


/***DEFINES*******************************************************************/
//#define DEBUG     //For debugging 


/***GLOBAL VARIABLES**********************************************************/
struct Myalloc {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    // Some other data members you want, 
    // such as lists to record allocated/free memory
};

struct Myalloc myalloc;


/***FUNCTION DECLARATIONS*****************************************************/
//See myalloc.h

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
    assert(_size > 0);
    myalloc.aalgorithm = _aalgorithm;
    myalloc.size = _size;
    myalloc.memory = malloc((size_t)myalloc.size);

    // Add some other initialization 
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

    // free other dynamic allocated memory to avoid memory leak
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

    // Allocate memory from myalloc.memory 
    // ptr = address of allocated memory

    return ptr;
    }


/*******************************************************************
** deallocate -- deallocated memory pointed to by _ptr 
**
** @param[in]  _ptr     pointer to memory to be freed          
**
********************************************************************/
void deallocate(void* _ptr) {
    assert(_ptr != NULL);

    // Free allocated memory
    // Note: _ptr points to the user-visible memory. The size information is
    // stored at (char*)_ptr - 8.
}


/*******************************************************************
** compact_allocation -- groups allocated memory blocks to compact memory block
**
** @param[in]  _before      array of pointers to start of memory blocks 
** @param[in]  _after       array of pointers to end of memory blocks   
**
********************************************************************/
int compact_allocation(void** _before, void** _after) 
    {
    int compacted_size = 0;

    // compact allocated memory
    // update _before, _after and compacted_size

    return compacted_size;
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
    // Calculate available memory size
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
    int smallest_free_chunk_size = myalloc.size;
    int largest_free_chunk_size = 0;

    // Calculate the statistics

    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
    }



