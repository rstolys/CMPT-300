/*-----------------------------------------------------------------------------
 *  process_ancestors.c -- Will determine information about the current process
 *
 * 19 Apr 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/syscalls.h>

#include "process_ancestors.h"


/***DEFINES*******************************************************************/
#define SUCCESS 0


/***GLOBAL VARIABLES**********************************************************/
//See process_ancestors.h


/***FUNCTION DECLARATIONS*****************************************************/
//See process_ancestors.h


/***FUNCTION DEFINITIONS******************************************************/


/*******************************************************************
** sys_process_ancestors -- compute array_stats in kernel -- syscall 438
**
** @param[in]  size             The numebr of info_array's
**
** @param[out]  num_filled      The number of structs written by the kernel
** @param[out]  info_array      Array of structs containing information 
**                              about the processes
**
********************************************************************/
SYSCALL_DEFINE3(process_ancestors, struct process_info*, info_array, long, size, long*, num_filled)
    {
    long                        rv = SUCCESS;
    long                        i = 0;
    struct task_struct          *task_stct = current;
    struct process_info         proc_info = NULL:

    struct list_head            *list_element = NULL;
    struct list_head            *list_first = NULL;
    long                        num_elements = 0;
    long                        tempMem = NULL;

    //Print inputs
    printk(KERN_INFO "INPUTS OF SYS_PROCESS_ANCESTORS: *info_array: %p, size: %ld, *num_filled: %p, \n", info_array, size, num_filled); 

    //Need input checking here 
    if(size <= 0)
        {
        printk(KERN_ERR "Invalid input: size\n"); 
        rv = -EINVAL;
        }
    // Output pointer num_filled is invalid
    else if(num_filled == NULL || (0 != copy_from_user(&tempMem, num_filled, sizeof(long))))
        {
        printk(KERN_ERR "Invalid input: num_filled\n"); 
        rv = -EFAULT;
        }
    else 
        {
        while(i < size)
            {
            //Check that info_array element is in valid location 
            if(0 != copy_from_user(&proc_info, &info_array[i], sizeof(struct process_info)))
                {
                printk(KERN_ERR "Invalid process_info element. Element #%ld\n", i); 
                rv = -EFAULT;
                break;
                }
            else 
                {
                //Determine and set information of process
                proc_info.pid = task_stct->pid;
                strcpy(proc_info.name, task_stct->comm);
                proc_info.state = task_stct->state;
                proc_info.uid = (long) task_stct->cred->uid;
                proc_info.nvcsw = task_stct->nvcsw;
                proc_info.nivcsw = task_stct->nivcsw;

                //
                //Determine the number of children processes
                //
                list_element = (task_stct->children != NULL) ?? task_stct->children->next : NULL;
                list_first = task_stct->children;

                //Increment the number of children to 1 if there are any
                if(list_element != NULL) {num_elements++;}

                while(list_element != NULL && list_element != list_first) 
                    {
                    num_elements++;
                    list_element = list_element->next;
                    }

                proc_info.num_children = num_elements;
                    
                //
                //Determine the number of sibling processes
                //
                num_elements = 0; 
                list_element = (task_stct->sibling != NULL) ?? task_stct->sibling->next : NULL;
                list_first = task_stct->sibling;

                //Increment the number of siblings to 1 if there are any
                if(list_element != NULL) {num_elements++;}

                while(list_element != NULL && list_element != list_first) 
                    {
                    num_elements++;
                    list_element = list_element->next;
                    }
                
                proc_info.num_siblings = num_elements;
                
                
                //Set the process_info output for this element
                if(0 != copy_to_user(&info_array[i], &proc_info, sizeof(struct process_info)))
                    {
                    printk(KERN_ERR "Invalid copy of process_info to user\n"); 
                    rv = -EFAULT;
                    break;
                    }

                //Move the task_stct to next parent 
                if(task_stct == task_stct->parent) { break; }
                else { task_stct = task_stct->parent; }

                //Move to next element in array
                i++;   
                }
            }

        //Set the number of structs filled
        if(0 != copy_to_user(num_filled, &i, sizeof(long)))
            {
            printk(KERN_ERR "Invalid copy to num_filled to user\n"); 
            rv = -EFAULT;
            }
        }

   
    printk(KERN_INFO "SYS_PROCESS_ANCESTORS: return value %ld\n", rv); 
    return rv;
    }


/*
struct process_info {
	long pid;                     /* Process ID *
	char name[ANCESTOR_NAME_LEN]; /* Program name of process *
	long state;                   /* Current process state *
	long uid;                     /* User ID of process owner *
	long nvcsw;                   /* # of voluntary cotext switches *
	long nivcsw;                  /* # of involuntary context switches *
	long num_children;            /* # of children processes *
	long num_siblings;            /* # of sibling processes *
};
*/