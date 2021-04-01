/*-----------------------------------------------------------------------------
 * main.c -- Main program for assignment 3
 *
 * 1 Apr 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include "myalloc.h"


/***DEFINES*******************************************************************/
#define ERROR              -1
#define SUCCESS             0
#define FAILURE             1

//For debugging 
//#define DEBUG


/***GLOBAL VARIABLES**********************************************************/
_Bool allocateMemory_gv = false;

/***FUNCTION DECLARATIONS*****************************************************/
void* alloc_factory(void* param);

/***FUNCTION DEFINITIONS******************************************************/

/*******************************************************************
** test1 -- test program, basic checks
**
** @param[in] _aalgorithm       algorithm to use for allocation
**
********************************************************************/
void test1(enum allocation_algorithm _aalgorithm) 
    {
    //
    //Initialize too little -- exits, only test once
    //
    //initialize_allocator(4, _aalgorithm);

    //
    // Request too much memory
    //
    initialize_allocator(100, _aalgorithm);
    void* tooMuch = allocate(150);
    assert(tooMuch == NULL);

    //
    // Intialize allocator again
    //
    initialize_allocator(100, _aalgorithm);
    //Should print "myalloc already initialized for 100 bytes of memory";

    //Cleanup from last test 
    destroy_allocator();


    //
    // Unknown _algorithm
    //
    initialize_allocator(100, -1);
    void* invalid_A = allocate(150);
    assert(invalid_A == NULL);

    //Cleanup from last test 
    destroy_allocator();
    }


/*******************************************************************
** test2 -- test program, allocate and deallocate tests
**
** @param[in] _aalgorithm       algorithm to use for allocation
**
********************************************************************/
void test2(enum allocation_algorithm _aalgorithm) 
    {
    //
    // Basic test allocate and deallocate
    //
    initialize_allocator(50, _aalgorithm);
    int* p[3] = {NULL};
    //Allocate all
    for(int i = 0; i < 3; i++)
        {
        p[i] = allocate(sizeof(int));
        }

    //Check memory available 
    assert(available_memory() == 6);

    for(int i = 0; i < 3; i++)
        {
        deallocate(p[i]);
        }

    //Check memory available
    assert(available_memory() == 42);

    destroy_allocator();


    //
    // Allocate 3, free middle and print statistics
    //
    initialize_allocator(50, _aalgorithm);
    int* p2[3] = {NULL};
    //Allocate all
    for(int i = 0; i < 3; i++)
        {
        p2[i] = allocate(sizeof(int));
        *(p2[i]) = i;
        }

    //Deallocate the middle
    deallocate(p2[1]);

    //Check memory available 
    assert(available_memory() == 10);

    //Allocate another block of memory
    p2[1] = allocate(sizeof(int)); *(p2[1]) = 4;
    printf("Location of Pointers:\n");
    printf("p[0]: %p\n", p2[0]);
    printf("p[1]: %p\n", p2[1]);
    printf("p[2]: %p\n", p2[2]);

    //Compact memory 
    void* before[5] = {NULL};
    void* after[5] = {NULL};
    compact_allocation(before, after);

    //Check memory available 
    assert(available_memory() == 6);

    //Ensure pointers match up
    assert(p2[0] == before[0]);
    assert(0 == *(int*)(after[0]));
    p2[0] = after[0];

    assert(p2[1] == before[1]);
    assert(4 == *(int*)(after[1]));
    p2[1] = after[1];

    assert(p2[2] == before[2]);
    assert(2 == *(int*)(after[2]));
    p2[2] = after[2];
    

    printf("New location of Pointers:\n");
    printf("p[0]: %p\n", p2[0]);
    printf("p[1]: %p\n", p2[1]);
    printf("p[2]: %p\n", p2[2]);

    destroy_allocator();
    }


/*******************************************************************
** test3 -- test program, testing algorithm differences
**
** @param[in] _aalgorithm       algorithm to use for allocation
**
********************************************************************/
void test3(enum allocation_algorithm _aalgorithm) 
    {
    //
    // Test of algorithms
    //
    initialize_allocator(100, _aalgorithm);
    int* p[6] = {NULL};
    //Allocate first 3
    for(int i = 0; i < 3; i++)
        {
        p[i] = allocate(sizeof(int));
        }

    //Deallocate middle block
    deallocate(p[1]);

    //Allocate second 3
    for(int i = 3; i < 6; i++)
        {
        p[i] = allocate(sizeof(int));
        }

    //Deallocate second middle block
    deallocate(p[4]);

    //Check available memory
    switch(_aalgorithm)
        {
        case FIRST_FIT:
            assert(available_memory() == 36);
            break;
        case BEST_FIT:
            assert(available_memory() == 36);
            break;
        case WORST_FIT:
            assert(available_memory() == 28);
            break;
        }

    destroy_allocator();


    //
    // Test difference in allocation methods, get failure in one
    //
    initialize_allocator(100, _aalgorithm);
    int* p2[6] = {NULL};
    //Allocate first 3
    for(int i = 0; i < 3; i++)
        {
        p2[i] = allocate(sizeof(int));
        }

    //Deallocate middle block
    deallocate(p2[1]);
    deallocate(p2[2]);

    //Check memory availible 
    assert(available_memory() == 80);
        //This will test the combine neighbours function

    //Allocate 2 chars
    char* p3[2] = {NULL};
    p3[0] = allocate(sizeof(char));
    p3[1] = allocate(sizeof(char));

    char* originalPos = p3[0];

    //Allocate 1 int
    p2[1] = allocate(sizeof(int));

    //Remove the first char
    deallocate(p3[0]);

    // Add char back in
    p3[0] = allocate(sizeof(char));

    //Check that memory 
    switch(_aalgorithm)
        {
        case FIRST_FIT:
        case BEST_FIT:
            assert(p3[0] == originalPos);
            assert(available_memory() == 50);
            break;
        case WORST_FIT:
            assert(p3[0] != originalPos);
            assert(available_memory() == 42);
            break;
        }

    //Try to Allocate 4 intergers 
    for(int i = 2; i < 6; i++)
        {
        p2[i] = allocate(sizeof(int));
        }

    //Check memory allocation
    switch(_aalgorithm)
        {
        case FIRST_FIT:
        case BEST_FIT:
            assert(p2[5] != NULL);
            assert(available_memory() == 2);
            break;
        case WORST_FIT:
            assert(p2[5] == NULL);
            assert(available_memory() == 6);
            break;
        }

    //
    // Determine difference between First and Best fit
    //
    int* expectedFirst = p2[0];
    int* expectedBest = p2[5];

    //Deallocate both chars and first int
    deallocate(p3[0]);
    deallocate(p3[1]);
    deallocate(p2[0]);

    //Deallocate inteter
    deallocate(p2[5]);
    p2[5] = NULL;

    //Allocate integer 
    p2[5] = allocate(sizeof(int));

    //Test the difference 
    switch(_aalgorithm)
        {
        case FIRST_FIT:
            assert(p2[5] == expectedFirst);
            break;
        case BEST_FIT:
            assert(p2[5] == expectedBest);
            break;
        default:
            break;
        }


    //
    // Test the statistics functions
    //
    print_statistics();
    printf("\n");
    //FIRST_FIT Expecting: 
    //"Allocated size = 20\n"
    //"Allocated chunks = 5\n"
    //"Free size = 24\n"
    //"Free chunks = 2\n"
    //"Largest free chunk size = 14\n"
    //"Smallest free chunk size = 10\n"

    //BEST_FIT Expecting: 
    //"Allocated size = 20\n"
    //"Allocated chunks = 5\n"
    //"Free size = 24\n"
    //"Free chunks = 2\n"
    //"Largest free chunk size = 22\n"
    //"Smallest free chunk size = 2\n"

    //WORST_FIT Expecting: 
    //"Allocated size = 20\n"
    //"Allocated chunks = 5\n"
    //"Free size = 16\n"
    //"Free chunks = 3\n"
    //"Largest free chunk size = 10\n"
    //"Smallest free chunk size = 1\n"


    destroy_allocator();
    }


/*******************************************************************
** test4 -- test program, testing multithreading
**
** @param[in] _aalgorithm       algorithm to use for allocation
**
********************************************************************/
void test4(enum allocation_algorithm _aalgorithm) 
    {
    pthread_t* factory_num;
    allocateMemory_gv = true;

    if(NULL == (factory_num = malloc(sizeof(pthread_t) * 2)))
        {
        //Print error message - not enough memory
        exit(ERROR);
        }

    initialize_allocator(100, _aalgorithm);

    for(int f = 0; f < 2; f++)
        {
        //Create thread with factory number f
        int failed = pthread_create(&(factory_num[f]), NULL, alloc_factory, NULL);

        if(failed)
            {
            //Indicate some error occured
            exit(ERROR);
            }
        }    

    //Wait for threads to do stuff
    sleep(2);

    //Call threads back to us
    allocateMemory_gv = false;        //Indicate to threads to stop their processing
    for(int f = 0; f < 2; f++)
        {
        pthread_join(factory_num[f], NULL);
        }

    destroy_allocator();
    }


/*******************************************************************
** alloc_factory -- memory generation factory
**
** @param none
**
********************************************************************/
void* alloc_factory(void* param)
    {
    int* p1 = NULL;
    int* p2 = NULL;

    while(allocateMemory_gv)
        {
         //Allocate Memory
        if(p1 == NULL)
            p1 = allocate(sizeof(int));

        if(p2 == NULL)
            p2 = allocate(sizeof(int));

        //Deallocate Memory
        deallocate(p1);
        deallocate(p2);
        p1 = NULL;
        p2 = NULL;
        }

    return NULL;
    }

/*******************************************************************
** MAIN FUNCTION -- test program
**
** @param[in]  argc    number of command line arguements
** @param[in]  argv    array of command line arguements
**
********************************************************************/
int main(int argc, char* argv[]) 
    {
    test1(FIRST_FIT);
    test1(BEST_FIT);
    test1(WORST_FIT);

    test2(FIRST_FIT);
    test2(BEST_FIT);
    test2(WORST_FIT);

    test3(FIRST_FIT);
    test3(BEST_FIT);
    test3(WORST_FIT);

    test4(FIRST_FIT);
    test4(BEST_FIT);
    test4(WORST_FIT);
   
    initialize_allocator(100, FIRST_FIT);
    //initialize_allocator(100, BEST_FIT);
    //initialize_allocator(100, WORST_FIT);
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

    printf("available_memory %d\n", available_memory());

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
