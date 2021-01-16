/*-----------------------------------------------------------------------------
 * test_dlist.c -- Test program for doubly linked listed
 *
 * 15 Jan 2021	Ryan Stolys
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include "dlist.h"
#include <assert.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/***DEFINES*******************************************************************/
#define SUCCESS             0
#define FAILURE             1


/***GLOBAL VARIABLES**********************************************************/
//NONE


/***FUNCTION DECLARATIONS*****************************************************/
//NONE


/***FUNCTION DEFINITIONS******************************************************/


///////////////////////////////////////////////////////////////////
/// providedTests-- test program provided in assignment sheet
///
/// @param[in]  argc    number of command line arguements
/// @param[in]  argv    array of command line arguements
///
///////////////////////////////////////////////////////////////////
int providedTests(int argc, char **argv)
    {
    printf("Starting tests...\n");
	struct nodeStruct *head = NULL;

	// Starting count:
	assert(List_countNodes(head) == 0);


	// Create 1 node:
	struct nodeStruct* firstNode = List_createNode(0);
	List_insertHead(&head, firstNode);
	assert(List_countNodes(head) == 1);
	assert(List_findNode(head, 0) == firstNode);
	assert(List_findNode(head, 1) == NULL);

	// Insert tail:
	struct nodeStruct* lastNode = List_createNode(-5);
	List_insertTail(&head, lastNode);
	assert(List_countNodes(head) == 2);
	assert(List_findNode(head, 0) == firstNode);
	assert(List_findNode(head, -5) == lastNode);
	assert(List_findNode(head, 1) == NULL);


	// Verify list:
	struct nodeStruct *current = head;
	assert(current->item == 0);
	assert(current->next != NULL);
	current = current->next;
	assert(current->item == -5);
	assert(current->next == NULL);


	// Sort and verify:
	List_sort(&head);
	current = head;
	assert(current->item == -5);
	assert(current->next != NULL);
	current = current->next;
	assert(current->item == 0);
	assert(current->next == NULL);


	// Delete
	assert(List_countNodes(head) == 2);
	struct nodeStruct *nodeOf0 = List_findNode(head, 0);
	List_deleteNode(&head, nodeOf0);
	assert(List_countNodes(head) == 1);
	assert(List_findNode(head, 0) == NULL);
	assert(List_findNode(head, 1) == NULL);
	current = head;
	assert(current->item == -5);
	assert(current->next == NULL);


	printf("\nExecution finished.\n");
    return SUCCESS;
    }


///////////////////////////////////////////////////////////////////
/// myTests-- test program written by me
///
/// @param[in]  argc    number of command line arguements
/// @param[in]  argv    array of command line arguements
///
///////////////////////////////////////////////////////////////////
int myTests(int argc, char **argv)
    {
    int listSize = 0;

    //Reference to the Reference of our list
    struct nodeStruct** listRef = (struct nodeStruct**) malloc(sizeof(struct nodeStruct*)); 

    //Create a node for our list
    struct nodeStruct* node = List_createNode(5);

    //Verify the value provided is correct
    assert(node->item == 5);

    //Create a reference to the head of the list
    (*listRef) = node;
    listSize++;

    //Verify the list head was constructed properly
    assert(((*listRef)->item) == 5);


    //Create List of length 3
    struct nodeStruct* node2 = List_createNode(7);
    struct nodeStruct* node1 = List_createNode(2);

    List_insertHead(listRef, node2);
    List_insertHead(listRef, node1);
    listSize += 2;

    //Verify list creation 
    assert((*listRef)->item == 2);
    assert((*listRef)->next->item == 7);
    assert((*listRef)->next->next->item == 5);


    //Add another node to the tail 
    struct nodeStruct* node4 = List_createNode(10);
    List_insertTail(listRef, node4);

    //Verfiy node was added properly
    assert((*listRef)->next->next->next->item == 10);


    //Count the nodes in the list 
    assert(List_countNodes(*listRef) == 4);


    //Test finding a specific node and looking for node that doesnt exist
    assert(List_findNode(*listRef, 5) == node);
    assert(List_findNode(*listRef, 10) == node4);
    assert(List_findNode(*listRef, 1) == NULL);

    List_print(*listRef);
    printf("\n\n\n");

    //Sort list
    List_sort(listRef);
    List_print(*listRef);
    printf("\n\n\n");


    //Test deletion of node
    List_deleteNode(listRef, List_findNode(*listRef, 5));
    assert(List_findNode(*listRef, 5) == NULL);
    assert(List_countNodes(*listRef) == 3);

    List_print(*listRef);



    //
    //Free Memory
    //
    List_free(listRef);

    if(listRef != NULL)
        free(listRef);



    printf("Passed all tests\n");

    return SUCCESS;
    }


///////////////////////////////////////////////////////////////////
/// MAIN FUNCTION -- test prgram
///
/// @param[in]  argc    number of command line arguements
/// @param[in]  argv    array of command line arguements
///
///////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
    {
    printf("Beginning providedTests()\n");
    providedTests(argc, argv);
    printf("Completed providedTests()\n\n");

    printf("Beginning myTests()\n");
    myTests(argc, argv);
    printf("Completed myTests()\n\n");

    return SUCCESS;
    }