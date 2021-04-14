/*-----------------------------------------------------------------------------
 * process_ancestors.h -- Structure to hold values returned by process_ancestors sys-call
 *
 * 19 Apr 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

#ifndef _PROCESS_ANCESTORS_H
#define _PROCESS_ANCESTORS_H

/***INCLUDES******************************************************************/
//NONE

/***DEFINES*******************************************************************/
//None

/***GLOBAL VARIABLES**********************************************************/
#define ANCESTOR_NAME_LEN 16

struct process_info {
	long pid;                     /* Process ID */
	char name[ANCESTOR_NAME_LEN]; /* Program name of process */
	long state;                   /* Current process state */
	long uid;                     /* User ID of process owner */
	long nvcsw;                   /* # of voluntary context switches */
	long nivcsw;                  /* # of involuntary context switches */
	long num_children;            /* # of children processes */
	long num_siblings;            /* # of sibling processes */
};


asmlinkage long sys_process_ancestors(struct process_info *info_array, long size, long *num_filled);


#endif /*_PROCESS_ANCESTORS_H*/