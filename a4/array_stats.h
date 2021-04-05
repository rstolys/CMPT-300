/*-----------------------------------------------------------------------------
 * array_stats.h -- Array Stats Sys Call Header File
 *
 * 19 Apr 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

#ifndef _ARRAY_STATS_H_
#define _ARRAY_STATS_H_

/***INCLUDES******************************************************************/
//NONE

/***DEFINES*******************************************************************/
//None


/***GLOBAL VARIABLES**********************************************************/
struct array_stats {
  long min;
  long max;
  long sum;
};


asmlinkage long sys_array_stats(struct array_stats *stats, long *data, long size);

#endif /*_ARRAY_STATS_H_*/