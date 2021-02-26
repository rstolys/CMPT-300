/*-----------------------------------------------------------------------------
 * stats.h -- Function Declarations for Statistics Module
 *
 * 25 Feb 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

#ifndef STATS_H   
#define STATS_H 

/***INCLUDES******************************************************************/
//NONE

/***DEFINES*******************************************************************/
//NONE

/***GLOBAL VARIABLES**********************************************************/


void stats_init(int num_producers);

void stats_cleanup(void);

void stats_record_produced(int factory_number);

void stats_record_consumed(int factory_number, double delay_in_ms);

void stats_display(void);


#endif /*STATS_H*/