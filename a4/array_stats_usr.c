/*-----------------------------------------------------------------------------
 *  array_stats.c -- Array Stats Sys Call
 *
 * 19 Apr 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "array_stats_usr.h"


/***DEFINES*******************************************************************/
#define SUCCESS 0

/***GLOBAL VARIABLES**********************************************************/
//See array_stats.h


/***FUNCTION DECLARATIONS*****************************************************/
//See array_stats.h


/***FUNCTION DEFINITIONS******************************************************/

/*******************************************************************
** sys_array_stats -- compute array_stats in kernel -- syscall 437
**
** @param[in]  data         the input array pointer
** @param[in]  size         the size of the array to be analyzed
**
** @param[out]  stats       the output array_stats
**
********************************************************************/
long sys_array_stats(struct array_stats *stats, long *data, long size)
    {
    //Define local variables
    int                     rv = SUCCESS;
    long                    tempMem = 0;
    struct array_stats      arrStats = {0};

    //Print inputs
    printf("INPUTS OF SYS_ARRAY_STATS: *stats: %p, *data: %p, size: %ld\n", stats, data, size); 

    memcpy(&tempMem, data, sizeof(long));
    memcpy(&arrStats, stats, sizeof(struct array_stats));

    //Check value size of array 
    if(size <= 0)
        {
        printf("Invalid input: size\n"); 
        rv = -EINVAL;
        }
    // Input data is NULL or invalid for user
    else if(data == NULL)
        {
        printf("Invalid input: data\n"); 
        rv = -EFAULT;
        }
    // Output pointer is null or invalid for user
    else if(stats == NULL)
        {
        printf("Invalid input: stats\n"); 
        rv = -EFAULT;
        }
    else 
        {
        //All of our inputs are valid, we can access the array values to determine statistics

        //Initialize array_stats values with first value in array
        arrStats.min = tempMem;
        arrStats.max = tempMem;
        arrStats.sum = tempMem;

        for(long i = 1; i < size; i++)
            {
            memcpy(&tempMem, data + i, sizeof(long));
            
            //Check and set min value
            if(arrStats.min > tempMem)
                arrStats.min = tempMem;

            //Check and set max value
            if(arrStats.max < tempMem)
                arrStats.max = tempMem;

            //Increase the sum of the array values
            arrStats.sum += tempMem;
            }

        //Set the array stats output if we are here without here (rv is still == SUCCESS)
        memcpy(stats, &arrStats, sizeof(struct array_stats));
        }

    return rv;
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
    struct array_stats testStat = {0}; 
    long dataIn[1] = {1};

    int result = sys_array_stats(&testStat, dataIn, 1);

    printf("RESULT IS: %d\n", result);
    printf("STATS ARE: min: %ld, max: %ld, sum: %ld\n", testStat.min, testStat.max, testStat.sum);

    return 0;
    }