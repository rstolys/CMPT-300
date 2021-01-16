/*-----------------------------------------------------------------------------
 * test_list.c -- Test program for linked listed
 *
 * 15 Jan 2021	Ryan Stolys
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include "list.h"
#include <stdio.h>
#include <stdlib.h>     //For malloc

/***DEFINES*******************************************************************/
//NONE

/***GLOBAL VARIABLES**********************************************************/
//NONE


/***FUNCTION DECLARATIONS*****************************************************/
//LOCATED IN list.h


/***FUNCTION DEFINITIONS******************************************************/

/*
 * Allocate memory for a node of type struct nodeStruct and intialize
 * it with value item, Return a pointer to the new node
 */
struct nodeStruct* List_createNode(int newItem)
    {
    struct nodeStruct* node; 

    //Allocate memory for next node of size nodeStruct
    node = (struct nodeStruct*) malloc(sizeof(struct nodeStruct));

    //Set the item of the node to newItem
    node->item = newItem;       //Set the item to the newItem value
    node->next = NULL;          //Set the pointer to the next element to NULL
        
    return node;
    }

/*
 * Insert node at head of list
 */
void List_insertHead(struct nodeStruct **headRef, struct nodeStruct *node)
    {
    return;
    }

/*
 * Insert node at tail of list
 */
void List_insertTail(struct nodeStruct **headRef, struct nodeStruct *node)
    {
    return;
    }

/*
 * Count number of node in the list
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes(int item)
    {
    return 0;
    }

/*
 * Return first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode(struct nodeStruct *head, int item)
    {
    return NULL;
    }

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set to a valid node
 * in the list. For example, the client code may have found it by calling 
 * List_findNode(). If the list contains only one node, the head of the list
 * should be et to NULL.
 */
void List_deleteNode(struct nodeStruct **headRef, struct nodeStruct *node)
    {
    return;
    }

/*
 * Sort the list in assending order based on the item field.
 * Any sorting algorthm is fine.
 */
void List_sort(struct nodeStruct **headRef)
    {
    return;
    }
