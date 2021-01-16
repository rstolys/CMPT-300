/*-----------------------------------------------------------------------------
 * dlist.h -- Function Declarations for doubly linked list
 *
 * 16 Jan 2021	Ryan Stolys
 *
 -----------------------------------------------------------------------------*/

#ifndef List_H   
#define List_H

/***INCLUDES******************************************************************/
//NONE

/***DEFINES*******************************************************************/
//NONE

/***GLOBAL VARIABLES**********************************************************/

struct nodeStruct 
    {
    int item;
    struct nodeStruct *previous;
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
int List_countNodes(struct nodeStruct *head);

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


/****************************************************************
 * NON_REQUIED FUNCTIONS
 ****************************************************************/

/*
 * Will delete all element of the list and free associated memory.
 * The head ref will point to null but will still exist. 
 * It is the callers responsibility to free the headRef
 */
void List_free(struct nodeStruct **headRef);


/*
 * Will print all the elements of the list in order to verify sorted order
 * and list creation
 */
void List_print(struct nodeStruct *head);



#endif