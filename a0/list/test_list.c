/*-----------------------------------------------------------------------------
 * test_list.c -- Test program for linked listed
 *
 * 15 Jan 2021	Ryan Stolys
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include "list.h"
#include <assert.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/***DEFINES*******************************************************************/
#define SUCCESS             0
#define FAILURE             1


/***GLOBAL VARIABLES**********************************************************/
//NONE


/***FUNCTION DECLARATIONS*****************************************************/
//NONE


/***FUNCTION DEFINITIONS******************************************************/


///////////////////////////////////////////////////////////////////
/// MAIN FUNCTION -- test prgram
///
/// @param[in]  argc    number of command line arguements
/// @param[in]  argv    array of command line arguements
///
///////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
    {
    struct nodeStruct* node = List_createNode(5);

    //Verify the value provided is correct
    assert(node->item == 5);

    if(node != NULL)
        free(node);

    printf("Passed all tests");

    return SUCCESS;
    }