/*-----------------------------------------------------------------------------
 * test_list.c -- Test program for linked listed
 *
 * 15 Jan 2021	Ryan Stolys
 *
 -----------------------------------------------------------------------------*/

#ifndef LIST_H   
#define LIST_H

/***INCLUDES******************************************************************/
//NONE

/***DEFINES*******************************************************************/
//NONE

/***GLOBAL VARIABLES**********************************************************/

struct nodeStruct 
    {
    int item;
    struct nodeStruct *next;
    };


/***FUNCTION DECLARATIONS*****************************************************/

/*
 * Allocate memory for a node of type struct nodeStruct and intialize
 * it with value item, Return a pointer to the new node
 */
struct nodeStruct* List_createNode(int item);

/*
 * Insert node at head of list
 */
void List_insertHead(struct nodeStruct **headRef, struct nodeStruct *node);

/*
 * Insert node at tail of list
 */
void List_insertTail(struct nodeStruct **headRef, struct nodeStruct *node);

/*
 * Count number of node in the list
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes(int item);

/*
 * Return first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode(struct nodeStruct *head, int item);

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set to a valid node
 * in the list. For example, the client code may have found it by calling 
 * List_findNode(). If the list contains only one node, the head of the list
 * should be et to NULL.
 */
void List_deleteNode(struct nodeStruct **headRef, struct nodeStruct *node);

/*
 * Sort the list in assending order based on the item field.
 * Any sorting algorthm is fine.
 */
void List_sort(struct nodeStruct **headRef);



#endif