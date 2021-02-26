/*-----------------------------------------------------------------------------
 * bbuff.h -- Function Declarations for Bounded buffer module
 *
 * 25 Feb 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

#ifndef BBUFF_H   
#define BBUFF_H 

/***INCLUDES******************************************************************/
#include <stdbool.h>

/***DEFINES*******************************************************************/
//NONE

/***GLOBAL VARIABLES**********************************************************/
#define ERROR -1



void bbuff_init(void);

void bbuff_blocking_insert(void* item);

void* bbuff_blocking_extract(void);

_Bool bbuff_is_empty(void);


#endif /*BBUFF_H*/
