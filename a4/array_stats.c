/*-----------------------------------------------------------------------------
 *  array_stats.c -- Array Stats Sys Call
 *
 * 19 Apr 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include <stdio.h>
#include <errno.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "array_stats.h"


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
asmlinkage long sys_array_stats(struct array_stats *stats, long *data, long size)
    {
    //Define local variables
    int                     rv = SUCCESS;
    long                    tempMem;
    struct array_stats*     arrStats;

    //Print inputs
    printk(KERN_INFO "INPUTS OF SYS_ARRAY_STATS: *stats: %p, *data: %p, size: %ld\n", stats, data, size); 

    //Check value size of array 
    if(size <= 0)
        {
        printk(KERN_ERR "Invalid input: size\n"); 
        rv = -EINVAL;
        }
    // Input data is NULL or invalid for user
    else if(data == NULL || (0 != copy_from_user(&tempMem, data, sizeof(long))))
        {
        printk(KERN_ERR "Invalid input: data\n"); 
        rv = -EFAULT;
        }
    // Output pointer is null or invalid for user
    else if(stats == NULL || (0 != copy_from_user(arrStats, stats, sizeof(struct array_stats))))
        {
        printk(KERN_ERR "Invalid input: stats\n"); 
        rv = -EFAULT;
        }
    else 
        {
        //All of our inputs are valid, we can access the array values to determine statistics

        //Initialize array_stats values with first value in array
        arrStats->min = tempMem;
        arrStats->max = tempMem;
        arrStats->sum = tempMem;

        for(long i = 1; i < size; i++)
            {
            if(0 != copy_from_user(&tempMem, data + i, sizeof(long)))
                {
                printk(KERN_ERR "Invalid data element in array. Element #%ld\n", i); 
                rv = -EFAULT;
                break;
                }
            else 
                {
                 //Check and set min value
                if(arrStats->min > tempMem)
                    arrStats->min = tempMem;

                //Check and set max value
                if(arrStats->max < tempMem)
                    arrStats->max = tempMem;

                //Increase the sum of the array values
                arrStats->sum += tempMem;
                }
            }

        //Set the array stats output if we are here without here (rv is still == SUCCESS)
        if(rv == SUCCESS && 0 != copy_to_user(stats, arrStats, sizeof(struct array_stats)))
            {
            printk(KERN_ERR "Invalid copy of array_stats to user\n"); 
            rv = -EFAULT;
            }
        }

    return rv;
    }