/*-----------------------------------------------------------------------------
 * stats.c -- Statistics module
 *
 * 25 Feb 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "bbuff.h"


/***DEFINES*******************************************************************/
#define FACTORY_NUM     "Factory#"
#define MADE_NUM        "#Made"
#define EATEN_NUM       "#Eaten"
#define MIN_DELAY       "Min Delay[ms]"
#define AVG_DELAY       "Avg Delay[ms]"
#define MAX_DELAY       "Max Delay[ms]"

/***GLOBAL VARIABLES**********************************************************/
struct data_t {
    int made;
    int eaten;
    double minDelay;
    double maxDelay;
    double totalDelay;
};

struct data_t* stats;
int numFactories; 


/***FUNCTION DECLARATIONS*****************************************************/
//See stats.h


/***FUNCTION DEFINITIONS******************************************************/

/*******************************************************************
** stats_init -- initialize the statistics module
**
** @param[in]  
**
********************************************************************/
void stats_init(int num_producers) 
    {
    numFactories = num_producers;

    //Generate array for stats
    if(NULL == (stats = malloc(sizeof(struct data_t) * num_producers)))
        {
        printf("Failed on memory allocation of stats module\n");
        exit(ERROR);
        }

    //Intialize array values
    for(int f = 0; f < num_producers; f++)
        {
        stats[f].made = 0;
        stats[f].eaten = 0;
        stats[f].totalDelay = 0.0;
        stats[f].maxDelay = 0.0;
        stats[f].minDelay = 1000000000.0;
        }

    
    return;
    }


/*******************************************************************
** stats_cleanup -- deallocate any memory
**
** @param[in]  
**
********************************************************************/
void stats_cleanup(void) 
    {
    free(stats);        //Release memory for stats array
    return;
    }


/*******************************************************************
** stats_record_produced -- ??
**
** @param[in]  
**
********************************************************************/
void stats_record_produced(int factory_number) 
    {
    //Increment the number of candies produced by the factory
    stats[factory_number].made++;      
    return;
    }


/*******************************************************************
** stats_record_consumed -- ??
**
** @param[in]  
**
********************************************************************/
void stats_record_consumed(int factory_number, double delay_in_ms) 
    {
    //Increment the number of candies eaten
    stats[factory_number].eaten++; 
    stats[factory_number].totalDelay += delay_in_ms;

    //Determine if this delay was a min or max delay 
    if(stats[factory_number].minDelay > delay_in_ms)
        {
        stats[factory_number].minDelay = delay_in_ms;
        }
    else if(stats[factory_number].maxDelay < delay_in_ms)
        {
        stats[factory_number].maxDelay = delay_in_ms;
        }
    
    return;
    }


/*******************************************************************
** stats_display -- ??
**
** @param[in]  
**
********************************************************************/
void stats_display(void) 
    {
    //Print Title
    printf("%8s%7s%8s%15s%15s%15s\n", FACTORY_NUM, MADE_NUM, EATEN_NUM, MIN_DELAY, AVG_DELAY, MAX_DELAY);

    for(int f = 0; f < numFactories; f++)
        {
        double avgDelay = stats[f].totalDelay / (double) stats[f].eaten;
        printf("%8d%7d%8d%15.5f%15.5f%15.5f\n", f, stats[f].made, stats[f].eaten, stats[f].minDelay, avgDelay, stats[f].maxDelay);
        }

    return;
    }