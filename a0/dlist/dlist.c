/*-----------------------------------------------------------------------------
 * dlist.c -- Functions for doubly linked listed
 *
 * 15 Jan 2021	Ryan Stolys
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include "dlist.h"
#include <stdio.h>
#include <stdlib.h>     //For malloc

/***DEFINES*******************************************************************/
//NONE

/***GLOBAL VARIABLES**********************************************************/
//NONE


/***FUNCTION DECLARATIONS*****************************************************/
//LOCATED IN dlist.h


/***FUNCTION DEFINITIONS******************************************************/

/**
 * Allocate memory for a node of type struct nodeStruct and intialize
 * it with value item, Return a pointer to the new node
 **/
struct nodeStruct* List_createNode(int newItem)
    {
    struct nodeStruct* node; 

    //Allocate memory for next node of size nodeStruct
    node = (struct nodeStruct*) malloc(sizeof(struct nodeStruct));

    if(node != NULL)
        {
        //Set the item of the node to newItem
        node->item = newItem;       //Set the item to the newItem value
        node->previous = NULL;      //Set the previous pointer to NULL
        node->next = NULL;          //Set the next pointer to NULL
        }

    return node;
    }

/**
 * Insert node at head of list
 **/
void List_insertHead(struct nodeStruct **headRef, struct nodeStruct *node)
    {
    //Set the previous pointer of the old head to our new node
    if(*headRef != NULL)
        (*headRef)->previous = node;


    //Set the next and previous nodes of the new head node
    node->previous = NULL; 
    node->next = (*headRef); 

    //Set the head reference to the new head of the list 
    (*headRef) = node;

    return;
    }


/**
 * Insert node at tail of list
 **/
void List_insertTail(struct nodeStruct **headRef, struct nodeStruct *node)
    {
    struct nodeStruct* tempNode = (*headRef); 

    //Traverse list until we get to an element with next == NULL
    while(tempNode->next != NULL)
        {
        tempNode = tempNode->next;
        }

    //Assign the old tail next pointer in the list to the new tail
    tempNode->next = node;

    //Assign the pointers of our new tail 
    node->previous = tempNode;
    node->next = NULL;

    return;
    }


/**
 * Count number of node in the list
 * Return 0 if the list is empty, i.e., head == NULL
 **/
int List_countNodes(struct nodeStruct *head)
    {
    int numNodes = 0; 
    struct nodeStruct* currentNode = head;

    while(currentNode != NULL)
        {
        currentNode = currentNode->next;
        numNodes++;
        }
    
    return numNodes;
    }

/**
 * Return first node holding the value item, return NULL if none found
 **/
struct nodeStruct* List_findNode(struct nodeStruct *head, int item)
    {
    struct nodeStruct* currentNode = head;

    while(currentNode != NULL && currentNode->item != item)
        {
        currentNode = currentNode->next;
        }
    //Will terminate when we run out of elements or we find the correct element

    return currentNode;
    }

/**
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set to a valid node
 * in the list. For example, the client code may have found it by calling 
 * List_findNode(). If the list contains only one node, the head of the list
 * should be set to NULL.
 **/
void List_deleteNode(struct nodeStruct **headRef, struct nodeStruct *node)
    {
    struct nodeStruct* currentNode = *headRef;

    //NOTE: based on funtion description we are supposed to find the node 
    //  but NULL condition is included in case error is made to avoid crash
    while(currentNode != NULL && currentNode->item != node->item)
        {
        currentNode = currentNode->next;        //Move to next node to continue search
        }
    
    //We found the node and can free it 
    if(currentNode != NULL)
        {
        if(currentNode->previous == NULL)        //Then the node found was the head
            {
            (*headRef) = currentNode->next;     //Will be NULL if only 1 element in list or next element
            currentNode->previous = NULL;       //Set the previous pointer to null since it is new head
            }
        else 
            {
            //Set next and previous pointers of elements next to deleted node
            currentNode->previous->next = currentNode->next;

            //If this is the tail then there is no next node
            if(currentNode->next != NULL)
                currentNode->next->previous = currentNode->previous;
            }

        //Free the memory pointer to by the currentNode
        free(currentNode);
        }

    return;
    }

/**
 * Sort the list in assending order based on the item field.
 * Any sorting algorthm is fine.
 **/
void List_sort(struct nodeStruct **headRef)
    {
    int numNodes = List_countNodes(*headRef);

    struct nodeStruct* currentNode;
    int tempVal;

    //Implement bubble sort
    for(int n = 0; n < numNodes - 1; n++)
        {
        currentNode = (*headRef);        //Set current node to start of list

        for(int e = 0; e < numNodes - n - 1; e++)
            {
            if(currentNode->item > currentNode->next->item)
                {
                //Swap elements
                tempVal = currentNode->next->item;
                currentNode->next->item = currentNode->item;
                currentNode->item = tempVal;
                }
            
            currentNode = currentNode->next;        //Move to next node in sequence
            }
        }

    return;
    }


/****************************************************************
 * NON_REQUIED FUNCTIONS
 ****************************************************************/

/**
 * Will delete all element of the list and free associated memory.
 * The head ref will point to null but will still exist. 
 * It is the callers responsibility to free the headRef
 **/
void List_free(struct nodeStruct **headRef)
    {
    //Set tempNode to head of list and freeNode to null
    struct nodeStruct* tempNode = (*headRef); 
    struct nodeStruct* freeNode = NULL;

    while(tempNode != NULL)
        {
        freeNode = tempNode;
        tempNode = tempNode->next;

        free(freeNode);
        }

    return;
    }


/**
 * Will print all the elements of the list in order to verify sorted order
 * and list creation
 **/
void List_print(struct nodeStruct *head)
    {
    int itemNum = 0;
    struct nodeStruct* currentNode = head;

    while(currentNode != NULL)
        {
        printf("Item: %d \t Value: %d\n", itemNum, currentNode->item);
        currentNode = currentNode->next;
        itemNum++;
        }

    if(itemNum == 0)
        {
        printf("List has zero items\n\n");
        }
        
    return;
    }