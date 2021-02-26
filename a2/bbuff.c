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
    int lastCandyPos;
    void* candyBuf[BUFFER_SIZE];
} buffer;

pthread_mutex_t mutex;


/***FUNCTION DECLARATIONS*****************************************************/
//See bbuff.h


/***FUNCTION DEFINITIONS******************************************************/


/*******************************************************************
** bbuff_init -- ??
**
** @param[in]  
**
********************************************************************/
void bbuff_init(void) 
    {
    buffer.candyInBuf = 0;
    buffer.lastCandyPos = 0;
    if (pthread_mutex_init(&mutex, NULL) != 0)
        {
        printf("\n mutex init failed\n");
        exit(ERROR);
        }
    return;
    }


/*******************************************************************
** bbuff_blocking_insert -- ??
**
** @param[in]  
**
********************************************************************/
void bbuff_blocking_insert(void* item) 
    {
    // Get access to buffer
    pthread_mutex_lock(&mutex);

    // Insert item into buffer
    buffer.candyBuf[buffer.lastCandyPos] = item;
    buffer.candyInBuf = 0;
    buffer.lastCandyPos = (buffer.lastCandyPos+1) % 10;

    // Release buffer
    pthread_mutex_unlock(&mutex);

    return;
    }


/*******************************************************************
** bbuff_blocking_extract -- ??
**
** @param[in]  
**
********************************************************************/
void* bbuff_blocking_extract(void)
    {
    return NULL;
    }

/*******************************************************************
** bbuff_is_empty -- ??
**
** @param[in]  
**
********************************************************************/
_Bool bbuff_is_empty(void)
    {
    return true;
    }

/*******************************************************************
** bbuff_cleanup -- ??
**
** @param[in]
**
********************************************************************/
void bbuff_cleanup(void)
    {
    pthread_mutex_destroy(&mutex);
    return;
    }
