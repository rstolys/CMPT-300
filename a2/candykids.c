/*-----------------------------------------------------------------------------
 * candykids.c -- Main program for assignment 2
 *
 * 25 Feb 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#include "bbuff.h"
#include "stats.h"


/***DEFINES*******************************************************************/
#define ERROR              -1
#define SUCCESS             0
#define FAILURE             1

//For debugging 
//#define DEBUG


/***GLOBAL VARIABLES**********************************************************/
typedef struct {
    int factory_number;
    double creation_ts_ms;
} candy_t;

_Bool generateCandy = false;


/***FUNCTION DECLARATIONS*****************************************************/
void* factory(void* factory_id);

void* kid(void* param);

double current_time_in_ms(void);


/***FUNCTION DEFINITIONS******************************************************/

/*******************************************************************
** factory -- candy generation factory
**
** @param[in]  factory_id   The id of the factory
**
********************************************************************/
void* factory(void* factory_id)
    {
    srand(time(NULL));   // Initialize rand function
    int factNum = *((int*)factory_id);

    while(generateCandy)
        {
        //Create candy
        candy_t* newCandy = malloc(sizeof(candy_t));
        newCandy->factory_number = factNum;
        newCandy->creation_ts_ms = current_time_in_ms();

        //Insert to buffer
        bbuff_blocking_insert(newCandy);

        //Record insertion
        stats_record_produced(factNum);

        sleep(rand() % 4);   //sleep for 0 to 3 seconds        
        }

    return NULL;
    }


/*******************************************************************
** kid -- candy eating kid
**
** @param[in]  none
**
********************************************************************/
void* kid(void* param)
    {
    srand(time(NULL));   // Initialize rand function
    candy_t* candy = NULL;
    double tConsumed;

    // Allow kid thread to be cancelled at any time
    int oldCancelType;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldCancelType);

    while(true)
        {
        //Get candy
        candy = (candy_t*) bbuff_blocking_extract();
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldCancelType);
        tConsumed = current_time_in_ms();

        if(candy != NULL)
            {
            //Record candy consumption
            stats_record_consumed(candy->factory_number, (tConsumed - candy->creation_ts_ms));

            free(candy);
            candy = NULL;
            }

        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldCancelType);
        sleep(rand() % 2);   //sleep for 0 to 1 seconds
        }
    
    pthread_exit(NULL);
    }


/*******************************************************************
** MAIN FUNCTION -- test program
**
** @param[in]  argc    number of command line arguements
** @param[in]  argv    array of command line arguements
**
********************************************************************/
int main(int argc, char **argv)
    {
    long int factories;
    long int kids;
    long int seconds;

    pthread_t* factThread_id;
    int* factory_num;
    
    pthread_t* kidThread_id;

    //
    // Extract Agruments
    //
    if(argc <= 3 || argc > 4)
        {
        //Print error message
        printf("You provided %d arguements. We require 4\n", argc);
        exit(ERROR);
        }
    else 
        {
        factories = (long) atoi(argv[1]);
        kids = (long) atoi(argv[2]);
        seconds = (long) atoi(argv[3]);

        //Check arguments
        if(factories < 1 || kids < 1  || seconds < 1 )
            {
            //print error message
            printf("There must be 1 or more factories, kids, and time in seconds.\nYou have provided %ld, %ld, and %ld respectively.\n", factories, kids, seconds);
            exit(ERROR);
            }
        }
   
    //
    // Initialize modules
    //
    bbuff_init();
    stats_init(factories);
    generateCandy = true;     //Set global variable to indicate to threads to keep working


    //
    // Launch candy-factory
    //
    //Create factThready_id array
    if(NULL == (factThread_id = malloc(sizeof(pthread_t) * factories)) || 
        NULL == (factory_num = malloc(sizeof(int) * factories))) 
        {
        //Print error message - not enough memory
        exit(ERROR);
        }

    for(int f = 0; f < factories; f++)
        {
        factory_num[f] = f;
        //Create thread with factory number f
        int failed = pthread_create(&factThread_id[f], NULL, factory, (void*) &factory_num[f]);

        if(failed)
            {
            //Indicate some error occured
            exit(ERROR);        //Is this the right response?
            }
        }      


    //
    // Launch kid threads
    //
    if(NULL == (kidThread_id = malloc(sizeof(pthread_t) * kids)))    //Create kidThread_id array
        {
        //Print error message - not enough memory
        exit(ERROR);
        }

    for(int k = 0; k < kids; k++)
        {
        //Create thread with kid number k
        int failed = pthread_create(&kidThread_id[k], NULL, kid, NULL);

        if(failed)
            {
            //Indicate some error occured
            exit(ERROR);        //Is this the right response?
            }
        }


    //
    // Wait for requested time
    //
    for(int s = 0; s < seconds; s++)
        {
        sleep(1);
        //Print the number of seconds that has elapsed
        printf("Time %ds\n", (s+1));
        }


    //  
    // Stop cody-factory threads
    //
    //Tell them to stop then join them back to main thread
    generateCandy = false;        //Indicate to threads to stop their processing
    for(int f = 0; f < factories; f++)
        {
        pthread_join(factThread_id[f], NULL);
        }


    //
    // Wait until no more candy
    //
    printf("Waiting for buffer to empty\n");
    while(!bbuff_is_empty()) { /*Wait for kids to eat*/ }



    //
    // Stop kid threads
    //
    //Cancel thread execution then join them back to main thread
    for(int f = 0; f < kids; f++)
        {
        printf("Cancelling Kid %d\n", f);
        pthread_cancel(kidThread_id[f]);
        }

    for(int f = 0; f < kids; f++)
        {
        pthread_join(kidThread_id[f], NULL);
        }


    //
    // Print stats
    //
    stats_display();


    //
    // Cleanup allocated memory
    //
    stats_cleanup();
    free(factThread_id);
    free(factory_num);
    free(kidThread_id);


    return SUCCESS;
    }


/*******************************************************************
** current_time_in_ms -- will print message to screen
**
** @param[in]  none
**
********************************************************************/
double current_time_in_ms(void)
   {
   struct timespec now;
   clock_gettime(CLOCK_REALTIME, &now);
   return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
   }

