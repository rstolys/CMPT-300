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
#include <sys/syscall.h>


/***DEFINES*******************************************************************/
#define SUCCESS                 0
#define ANCESTOR_NAME_LEN       16

#define _PROCESS_ANCESTORS_     438


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


/***FUNCTION DEFINITIONS******************************************************/

/***********************************************************
 * Testing routines for specific test
 ***********************************************************/
void test_error_checks()
{
    struct process_info p_info1, p_info2, p_info3;
    long* num_filled1, num_filled2, num_filled3;

    //Test size errors 
	do_syscall_failing(&p_info1, 0, num_filled1, -EINVAL);
    do_syscall_failing(&p_info2, -1, num_filled2, -EINVAL);
    do_syscall_failing(&p_info3, -1000, num_filled3, -EINVAL);

    //Test invalid p_info
    do_syscall_failing(NULL, 1, num_filled1, -EFAULT);
    do_syscall_failing(&p_info2 + sizeof(struct process_info), 1, num_filled2, -EFAULT);
    do_syscall_failing((struct process_info*)1LL, 1, num_filled3, -EFAULT);
    do_syscall_failing((struct process_info*)123456789012345689LL, 1, num_filled3, -EFAULT);

    //Test invalid num_filled
    do_syscall_failing(&p_info1, 1, NULL, -EFAULT);
    do_syscall_failing(&p_info2, 1, num_filled3 + sizeof(long), -EFAULT);
    do_syscall_failing(&p_info3, 1, (long*)1LL, -EFAULT);
    do_syscall_failing(&p_info3, 1, (long*)123456789012345689LL, -EFAULT);
}

void test_basic()
{
    struct process_info p_info;
    long* num_filled;
	do_syscall_working(&p_info, 1, num_filled);

    struct process_info* p_info2 = malloc(3*sizeof(struct process_info));
    long* num_filled2;
	do_syscall_working(p_info2, 3, num_filled2);
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
#define TEST(arg) test_internal((arg), __LINE__, #arg)

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
	int my_errno = errno;
	printf("..Rising to user level w/ result = %d", result);
	if (result < 0) {
		printf(", errno = %d", my_errno);
	} else {
		my_errno = 0;
	}
	printf("\n");
	return my_errno;

}
static void do_syscall_working(struct process_info* p_info, long size, long* num_filled)
    {
	int result = do_syscall(p_info, size, num_filled);

	TEST(result == 0);
    }

static void do_syscall_failing(struct process_info* p_info, long size, long* num_filled, long ret_code)
    {
	int result = do_syscall(p_info, size, num_filled);
	TEST(result == ret_code);
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
	return 0;
    }







