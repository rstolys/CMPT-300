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

/***FUNCTION DECLARATIONS*****************************************************/

void* factory(void* factory_id);

void* kid(void* param);

void myWrite(int fd, char* buf);

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
    return NULL;
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
    if(argc < 3 || argc > 4)
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
            exit(ERROR);
            }
        }
   
    //
    // Initialize modules
    //
    bbuff_init();
    stats_init(factories);


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
    // Wait for resquested time
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
    for(int f = 0; f < factories; f++)
        {
        pthread_cancel(factThread_id[f]);
        }

    for(int f = 0; f < factories; f++)
        {
        pthread_join(factThread_id[f], NULL);
        }


    //
    // Wait until no more candy
    //
    while(!bbuff_is_empty()) {/*Wait for kids to eat*/}


    //
    // Stop kid threads
    //
    //Cancel thread execution then join them back to main thread
    for(int f = 0; f < kids; f++)
        {
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
    free(kidThread_id);


    return SUCCESS;
    }


/*******************************************************************
** myWrite -- will print message to screen
**
** @param[in]  fd       file descriptor to write to
** @param[in]  buf      the character array to be printed
**
********************************************************************/
/*
void myWrite(int fd, char* buf) 
    {
    write(fd, buf, strlen(buf));

    return;
    }
*/

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