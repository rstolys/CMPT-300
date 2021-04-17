/*-----------------------------------------------------------------------------
 *  process_ancestors_test.c -- Test program for process_ancestors
 *
 * 19 Apr 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>


/***DEFINES*******************************************************************/
#define SUCCESS                 0
#define ANCESTOR_NAME_LEN       16

#define _PROCESS_ANCESTORS_     438

#define TEST(arg) test_internal((arg), __LINE__, #arg)

/***GLOBAL VARIABLES**********************************************************/
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


/***FUNCTION DECLARATIONS*****************************************************/
static void test_internal(_Bool success, int lineNum, char* argStr);
static int do_syscall(struct process_info *p_info, long size, long* num_filled);

/***FUNCTION DEFINITIONS******************************************************/

/***********************************************************
 * Testing routines for specific test
 ***********************************************************/
void test_error_checks()
    {
    struct process_info p_info1, p_info2, p_info3;
    long* num_filled1;
	long* num_filled2;
	long* num_filled3;
	int result = 0;

    //Test size errors 
	result = do_syscall(&p_info1,(long) 0, num_filled1);
	TEST(EINVAL == errno);

	result = do_syscall(&p_info1,(long) -1, num_filled1);
	TEST(EINVAL == errno);

	result = do_syscall(&p_info1,(long) -1000, num_filled1);
	TEST(EINVAL == errno);

    //Test invalid p_info
   	result = do_syscall(NULL, 1, num_filled1);
	TEST(EFAULT == errno);
    
	result = do_syscall(&p_info2 + sizeof(struct process_info), 1, num_filled2);
	TEST(EFAULT == errno);
   
	result = do_syscall((struct process_info*)1LL, 1, num_filled3);
	TEST(EFAULT == errno);
   
 	result = do_syscall((struct process_info*)123456789012345689LL, 1, num_filled3);
	TEST(EFAULT == errno);

    //Test invalid num_filled
    	result = do_syscall(&p_info1, 1, NULL);
	TEST(EFAULT == errno);

	result = do_syscall(&p_info2, 1, num_filled3 + sizeof(long));
	TEST(EFAULT == errno);

    	result = do_syscall(&p_info3, 1, (long*)1LL);
	TEST(EFAULT == errno);
    
	result = do_syscall(&p_info3, 1, (long*)123456789012345689LL);
	TEST(EFAULT == errno);
	 }


void test_basic()
    {
    struct process_info p_info;
    long num_filled;
	TEST(0 == do_syscall(&p_info, 1, &num_filled));
    printf("p_info pid result: %ld  vs acutal pid: %ld\n", p_info.pid, (long)getpid());

    struct process_info* p_info2 = malloc(3*sizeof(struct process_info));
    long num_filled2;
	TEST(0 == do_syscall(p_info2, 3, &num_filled2));
    printf("p_info pid result: %ld  vs acutal pid: %ld\n", p_info2[0].pid, (long)getpid());
    
    if(num_filled2 > 1)
        printf("p_info parent pid result: %ld  vs acutal parent pid: %ld\n", p_info2[1].pid, (long)getppid());

    if(num_filled2 > 2)
        printf("p_info 2nd parent pid result: %ld\n", p_info2[2].pid);
    }


/***********************************************************
 * Custom testing framework 
 *      -- copied from array_stats_test program
 *      -- provided by professor
 ***********************************************************/
// Track results:
static int numTests = 0;
static int numTestPassed = 0;

static int current_syscall_test_num = 0;
static int last_syscall_test_num_failed = -1;
static int num_syscall_tests_failed = 0;

// Macro to allow us to get the line number, and argument's text:

// Actual function used to check success/failure:
static void test_internal(_Bool success, int lineNum, char* argStr)
{
	numTests++;
	if (!success) {
		if (current_syscall_test_num != last_syscall_test_num_failed) {
			last_syscall_test_num_failed = current_syscall_test_num;
			num_syscall_tests_failed++;
		}
		printf("-------> ERROR %4d: test on line %d failed: %s\n",
				numTestPassed, lineNum, argStr);
	} else {
		numTestPassed++;
	}
}

static void test_print_summary(void)
    {
	printf("\nExecution finished.\n");
	printf("%4d/%d tests passed.\n", numTestPassed, numTests);
	printf("%4d/%d tests FAILED.\n", numTests - numTestPassed, numTests);
	printf("%4d/%d unique sys-call testing configurations FAILED.\n", num_syscall_tests_failed, current_syscall_test_num);
    }

/***********************************************************
 * Functions to actually make the sys-call and test results
 ***********************************************************/
static int do_syscall(struct process_info *p_info, long size, long* num_filled)
    {
	current_syscall_test_num++;

	printf("\nTest %d: ..Diving to kernel level\n", current_syscall_test_num);
	int result = syscall(_PROCESS_ANCESTORS_, p_info, size, num_filled);
	printf("..Rising to user level w/ result = %d", result);

	if(result < 0) 
        {
		printf(", errno = %d", (int)errno);
	    } 

	printf("\n");
	return result;
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
    test_error_checks();

    test_basic();

	test_print_summary();

	return SUCCESS;
    }







