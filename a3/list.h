/*-----------------------------------------------------------------------------
 * list.h -- Function Declarations for Statistics Module
 *
 * 1 Apr 2021	Ryan Stolys, Jayden Cole
 *
 -----------------------------------------------------------------------------*/

#ifndef __LIST_H__
#define __LIST_H__

/***INCLUDES******************************************************************/
//NONE

/***DEFINES*******************************************************************/


/***GLOBAL VARIABLES**********************************************************/

/* 
 * Allocate memory for a node of type strct nodeStruct and initialize it with the value
 * item. Return a pointer to the new node
 */
struct memHead* List_createNode(int* size);

/* 
 * Insert node at the head of the list
 */
void List_insertHead( struct memHead** headRef, struct memHead* node);

/* 
 * Insert nose after the tail of the list
 */
void List_insertTail( struct memHead** headRef, struct memHead* node);

/* 
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes( struct memHead* head );

/* 
 * Return the first node where desired size is less than availible size
 */
struct memHead* List_findFreeNode( struct memHead* head, int desireSize );

/* 
 * Return the node that matches the address provided.
 *      The address provided points to data element in front of header
 */
struct memHead* List_findAllocNode( struct memHead** allocRef, void* data );

/* 
 * Find the node to remove from the alloc list
 *  moves node to the free list
 */
void List_allocToFree( struct memHead** allocRef, struct memHead** freeRef, struct memHead* node );


/* 
 * Find the node to remove from the free list
 *  Alters free list to reflect allocted memory by removing 
 *  block or moving block to new start of free memory chunk
 *  Creates new node and adds it to alloc list
 */
void List_freeToAlloc( struct memHead** freeRef, struct memHead** allocRef, struct memHead* node, int size );

/* 
 * Determine if there are consecutive free blocks and combine them together 
 */
void List_combineNeighbours(struct memHead** listRef, struct memHead* oldHead);

/* 
 * Delete all the elements from the list
 */
void List_delete( struct memHead** listRef );


/*******************************************************************
** compact_allocation -- groups allocated memory blocks to compact memory block
**
** @param[in]   listHead       head of linked list to be sorted
**
********************************************************************/
void List_sort( struct memHead **listHead );


#endif /*__LIST_H__*/