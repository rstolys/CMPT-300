/*-----------------------------------------------------------------------------
 * main.c -- Main program for assignment 3
 *
 * 1 Apr 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include <stdio.h>
#include "myalloc.h"


/***DEFINES*******************************************************************/
#define ERROR              -1
#define SUCCESS             0
#define FAILURE             1

//For debugging 
//#define DEBUG


/***GLOBAL VARIABLES**********************************************************/


/***FUNCTION DECLARATIONS*****************************************************/


/***FUNCTION DEFINITIONS******************************************************/


/*******************************************************************
** MAIN FUNCTION -- test program
**
** @param[in]  argc    number of command line arguements
** @param[in]  argv    array of command line arguements
**
********************************************************************/
int main(int argc, char* argv[]) 
    {
    initialize_allocator(100, FIRST_FIT);
    // initialize_allocator(100, BEST_FIT);
    // initialize_allocator(100, WORST_FIT);
    printf("Using first fit algorithm on memory size 100\n");

    int* p[50] = {NULL};
    for(int i=0; i<10; ++i) 
        {
        p[i] = allocate(sizeof(int));
        if(p[i] == NULL) 
            {
            printf("Allocation failed\n");
            continue;
            }

        *(p[i]) = i;
        printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
        }

    print_statistics();

    for (int i = 0; i < 10; ++i) 
        {
        if(i % 2 == 0)
            continue;

        printf("Freeing p[%d]\n", i);
        deallocate(p[i]);
        p[i] = NULL;
        }

    printf("available_memory %d", available_memory());

    void* before[100] = {NULL};
    void* after[100] = {NULL};
    compact_allocation(before, after);

    print_statistics();

    // You can assume that the destroy_allocator will always be the 
    // last funciton call of main function to avoid memory leak 
    // before exit

    destroy_allocator();

    return SUCCESS;
    }
