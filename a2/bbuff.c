/*-----------------------------------------------------------------------------
 * bbuff.c -- Bounded Buffer module
 *
 * 25 Feb 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>


#include "bbuff.h"


/***DEFINES*******************************************************************/
#define BUFFER_SIZE 10


/***GLOBAL VARIABLES**********************************************************/
struct buffer_t {
    int candyInBuf;
    int insertPos;
    int extractPos;
    void* candyBuf[BUFFER_SIZE];
} buffer;

pthread_mutex_t mutex;


/***FUNCTION DECLARATIONS*****************************************************/
//See bbuff.h


/***FUNCTION DEFINITIONS******************************************************/


/*******************************************************************
** bbuff_init -- initalize the buffer for insertions and extractions
**
** @param[in]  none
**
********************************************************************/
void bbuff_init(void) 
    {
    //Set buffer info variables
    buffer.candyInBuf = 0;
    buffer.insertPos = 0;
    buffer.extractPos = 0;

    //Create buffer mutex
    if(pthread_mutex_init(&mutex, NULL) != 0)
        {
        printf("\n mutex init failed\n");
        exit(ERROR);
        }
    
    return;
    }


/*******************************************************************
** bbuff_blocking_insert -- insert candy element to buffer
**
** @param[in]  item     the candy element to be inserted into the buffer
**
********************************************************************/
void bbuff_blocking_insert(void* item) 
    {
    while(true)
        {
        while(buffer.candyInBuf == BUFFER_SIZE) {/*Wait for buffer to empty*/}

        // get access to buffer mutex
        pthread_mutex_lock(&mutex); 
        
        //Check to make sure the buffer is not full again
        if(buffer.candyInBuf == BUFFER_SIZE)
            {
            pthread_mutex_unlock(&mutex);       // Release buffer mutex if it is full again
            continue;
            }
        // you got the mutex and the buffer is not full
        else 
            {
            // Insert item into buffer
            buffer.candyBuf[buffer.insertPos] = item;
            buffer.candyInBuf++;
            buffer.insertPos = (buffer.insertPos+1) % BUFFER_SIZE;

            // Release buffer mutex
            pthread_mutex_unlock(&mutex);
            break;
            }
        }

    return;
    }


/*******************************************************************
** bbuff_blocking_extract -- Extracts candy element from the buffer
**
** @param[in]  none
**
********************************************************************/
void* bbuff_blocking_extract(void)
    {
    void* item;

    while(true)
        {
        while(buffer.candyInBuf == 0)
            {
            /*Wait for buffer to fill up but ensure the thread can be cancelled at this time*/
            }

        // get access to buffer mutex
        pthread_mutex_lock(&mutex);
        
        //Check to make sure the buffer is not empty now
        if(buffer.candyInBuf == 0)
            {
            pthread_mutex_unlock(&mutex);       // Release buffer mutex if it is empty again
            continue;
            }
        // you got the mutex and the buffer is not empty
        else 
            {
            // Extract item from buffer
            item = buffer.candyBuf[buffer.extractPos];
            buffer.candyInBuf--;
            buffer.extractPos = (buffer.extractPos+1) % BUFFER_SIZE;

            // Release buffer mutex
            pthread_mutex_unlock(&mutex);
            break;
            }
        }

    return item;
    }

/*******************************************************************
** bbuff_is_empty -- Determines if there is candy in the buffer
**
** @param[in]  none
**
********************************************************************/
_Bool bbuff_is_empty(void)
    {
    _Bool isEmpty = false;

    pthread_mutex_lock(&mutex);             // Get access to buffer mutex
    isEmpty = (buffer.candyInBuf == 0);
    pthread_mutex_unlock(&mutex);           // Release buffer mutex

    return isEmpty;
    }

/*******************************************************************
** bbuff_cleanup -- will release any allocated memory/ mutuxes
**
** @param[in] none
**
********************************************************************/
void bbuff_cleanup(void)
    {
    pthread_mutex_destroy(&mutex);
    return;
    }
