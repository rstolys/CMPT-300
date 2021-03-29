
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "myalloc.h"

/* 
 * Allocate memory for a node of type strct nodeStruct and initialize it with the value
 * item. Return a pointer to the new node
 */
/*******************************************************************
** List_createNode -- create a new node for a linked list
**
** @param[in]  size                 address of the curr value in memHead 
** @param[out] struct_memHead*      Newly created memHead noede 
**
********************************************************************/
struct memHead* List_createNode(int* size)
    {
    struct memHead* newNode = (struct memHead*) malloc( 1*sizeof(struct memHead) );

    if(newNode != NULL)
        {
        newNode->curr = size;
        newNode->next = NULL;
        }
    
    return newNode;
    }

/* 
 * Insert node at the head of the list
 */
void List_insertHead( struct memHead** headRef, struct memHead* node){
    /* Make head point to new node, and new node point to old head */
    struct memHead* temp = *headRef;
    *headRef = node;
    node->next = temp;
}

/* 
 * Insert nose after the tail of the list
 */
void List_insertTail( struct memHead** headRef, struct memHead* node){
    struct memHead* currNode = *headRef;

    /* If head ref if NULL, no nodes in list, add forst node into list */
    if(currNode == NULL){
        *headRef = node;
        return;
    }
    
    //First node not NULL, traverse the linked list until next node is NULL (end of the list)
    while(currNode->next != NULL){
        currNode = currNode->next;
    }

    //Add the node to the end of the list
    currNode->next = node;
    node->next = NULL;
}

/* 
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes( struct memHead* head ){
    if(head == NULL){
        return 0;
    } else {
        int i = 1;
        while( head->next != NULL){
            head = head->next;
            i++;
        }
        return i;
    }
}

/* 
 * Return the first node where desired size is less than availible size
 */
struct memHead* List_findFreeNode( struct memHead* head, int desireSize ){
    while(*(head->curr) > desireSize && head->next != NULL){
        head = head->next;
    }
    if(*(head->curr) <= desireSize){
        return head;
    } else {
        return NULL;
    }
}

/* 
 * Return the node that matches the address provided.
 *      The address provided points to data element in front of header
 */
/*******************************************************************
** List_findAllocNode -- Return the node that matches the header address provided.
**                          The address provided points to data element in front of header
**
** @param[in]  allocRef     address to linked list of allocated block memHeads   
** @param[in]  data         address of data of desired block
**
********************************************************************/
struct memHead* List_findAllocNode(struct memHead** allocRef, void* data)
    {
    struct memHead* temp = *allocRef;

    //Find the the alloc node that points to our data or node doesn't exist
    while(temp != NULL && (void*)((char*)temp->curr + HEADER_SIZE) != data)
        {
        temp = temp->next;
        }

    //Return desired node or NULL 
    return temp;
    }


/*******************************************************************
** List_allocToFree -- Find the node to remove from the alloc list
**                          moves node to the free list
**
** @param[in]  allocRef     address to linked list of allocated block memHeads   
** @param[in]  freeRef      address to linked list of free block memHeads
** @param[in]  node         node in allocated linked list that points to memory we want to free    
**
********************************************************************/
void List_allocToFree( struct memHead** allocRef, struct memHead** freeRef, struct memHead* node )
    {
    struct memHead* temp = NULL;
    struct memHead* prevNode = NULL;   

    //if node to remove is header node
    if(*allocRef == node)
        {
        //Move head node to next element
        temp = *allocRef;
        *allocRef = temp->next;
        } 
    else        //Node is not the head element
        {
        temp = (*allocRef)->next;
        prevNode = *allocRef;

        //Find node to remove
        while(temp != node && temp != NULL)
            {
            prevNode = temp; 
            temp = temp->next;
            }

        //Remove the node from the list
        prevNode->next = temp->next;
        }

    //Insert node to tail of free list
    List_insertTail(freeRef, node); 
    }


/*******************************************************************
** List_freeToAlloc -- Find the node to remove from the free list
**                          Alters free list to reflect allocted memory by removing 
**                          block or moving block to new start of free memory chunk
**                          Creates new node and adds it to alloc list
**
** @param[in]  freeRef      address to linked list of free block memHeads   
** @param[in]  allocRef     address to linked list of allocated block memHeads
** @param[in]  node         node in free linked list that points to memory we want to allocate
** @param[in]  size         size of memory to be allocated        
**
********************************************************************/
void List_freeToAlloc(struct memHead** freeRef, struct memHead** allocRef, struct memHead* node, int size)
    {
    struct memHead* temp = NULL;
    struct memHead* prevNode = NULL;   

    //Check node to allocate is not the head
    if(*freeRef != node)
        {
        temp = (*freeRef)->next;
        prevNode = *freeRef;

        //Find node to remove
        while(temp != node && temp != NULL)
            {
            prevNode = temp; 
            temp = temp->next;
            }

        //Check the size of the node we found
        if(size == *(node->curr)) 
            {
            //Remove the node from the list
            prevNode->next = temp->next;

            //Insert the the free list node to the alloc list 
            List_insertTail(allocRef, node);
            }
        else 
            {
            int freeSize = *(node->curr);       //Get the size of the free space
            int* headerAddr = node->curr;       //Get the address of the header

            //Move the header pointer to the new location
            node->curr = (int*)((char*)node->curr + size + HEADER_SIZE);
            
            //Set the new size of the free block
            int newSize = freeSize - size - HEADER_SIZE;
            memcpy(node->curr, &newSize, HEADER_SIZE);

            //Set the size of the allocated block, create new node to add to the alloc list
            memcpy(headerAddr, &size, HEADER_SIZE);
            struct memHead* allocNode = List_createNode(headerAddr);

            //Make sure create node worked properly
            if(allocNode == NULL)
                {
                printf("No memory available on system\n");
                exit(-1);
                }
            else 
                List_insertTail(allocRef, allocNode);
            }
        }
    else        //The node to allocate is the first node in the free list
        {
        //The size is equal to availible size -- allocate block, remove node from free list
        if(size == *(node->curr)) 
            {
            //Remove the node from the list
            *freeRef = node->next;

            //Insert the the free list node to the alloc list 
            List_insertTail(allocRef, node);
            }
        else        //The available free memory is greater than the needed space
            {
            int freeSize = *(node->curr);       //Get the size of the free space
            int* headerAddr = node->curr;       //Get the address of the header

            //Move the header pointer to the new location
            node->curr = (int*)((char*)node->curr + size + HEADER_SIZE);

            //Set the new size of the free block
            int newSize = freeSize - size - HEADER_SIZE;
            memcpy(node->curr, &newSize, HEADER_SIZE);

            //printf("\n(*freeRef)->curr: %p, node->curr: %p\n", (*freeRef)->curr, node->curr);
            //printf("*(*freeRef)->curr: %d, *(node->curr): %d\n", *((*freeRef)->curr), *(node->curr));
            //printf("freeSize: %d, size: %d\n", freeSize, size);

            //Set the size of the allocated block and create node to add to alloc list
            memcpy(headerAddr, &size, HEADER_SIZE);
            struct memHead* allocNode = List_createNode(headerAddr);

            //Make sure create node worked properly
            if(allocNode == NULL)
                {
                printf("No memory available on system\n");
                exit(-1);
                }
            else 
                List_insertTail(allocRef, allocNode);
            }
        }
    }


/*******************************************************************
** List_combineNeighbours -- Determine if there are consecutive free blocks and combine them together 
**
** @param[in]  listRef      address to linked list of memHeads   
** @param[in]  oldHead      address to block to be combined with neighbours     
**
********************************************************************/
void List_combineNeighbours(struct memHead** listRef, struct memHead* oldHead)
    {
    struct memHead* prevNode = NULL;
    struct memHead* currNode = *listRef;
    struct memHead* nextNode = (*listRef)->next;

    //Find the current node and it's neighbours in the list
    while(currNode != NULL)
        {
        if(currNode == oldHead)
            break;
        else 
            {
            prevNode = currNode;
            currNode = currNode->next;
            nextNode = (nextNode == NULL) ? NULL : nextNode->next;
            }
        }
    
    //For pointer safety
    if(currNode != NULL)
        {
        //Determine if the next block is availible to combine
        if(nextNode != NULL && (void*)((char*)currNode->curr + *(currNode->curr) + HEADER_SIZE) == (void*)nextNode)
            {
            //Combine these two blocks
            *(currNode->curr) += *(nextNode->curr);
            currNode->next = nextNode->next;
            memset(nextNode, 0, HEADER_SIZE);       //Clear the header from the memory block
            
            //Free the memory for the nextNode 
            free(nextNode);
            }

        //Determine if the previous block is availible to combine
        if(prevNode != NULL && (void*)((char*)prevNode->curr + *(prevNode->curr) + HEADER_SIZE) == (void*)currNode) 
            {
            //Combine the two blocks 
            *(prevNode->curr) += *(currNode->curr);
            prevNode->next = currNode->next;
            memset(nextNode, 0, HEADER_SIZE);       //Clear the header from the memory block

            //Free the memory for the currNode 
            free(nextNode);
            }
        }
    }


/*******************************************************************
** List_delete -- Delete all the elements from the list
**
** @param[in]  listRef      address to linked list of memHeads to be deleted    
**
********************************************************************/
void List_delete( struct memHead** listRef ) {
    struct memHead* temp = NULL;
    struct memHead* nextNode = *listRef;

    //Loop through list and remove all elements
    while(nextNode != NULL) {
        temp = nextNode;
        nextNode = nextNode->next;

        //Free current node
        free(temp);
    }

    //Set our list to null -- to be sure 
    *listRef = NULL;
}


/*******************************************************************
** compact_allocation -- groups allocated memory blocks to compact memory block
**
** @param[in]   listHead       head of linked list to be sorted
**
********************************************************************/
void List_sort( struct memHead **listHead )
    {
    struct memHead* oldListHead = *listHead;
    struct memHead* tempHead = NULL;
    struct memHead* tempTail = NULL;
    struct memHead* traverser;
    struct memHead* traverser_prev = NULL;
    void* leastAddr = oldListHead->curr;
    void* leastAddr_prev = NULL;

    //While our oldList still points to memHeads
    while(oldListHead != NULL)
        {
        traverser = oldListHead;

        //Find the smallest address 
        while(traverser != NULL)
            {
            if(traverser->curr <= (int*)leastAddr)
                {
                leastAddr_prev = (traverser_prev == NULL) ? NULL : traverser_prev->curr;
                leastAddr = traverser->curr;              //Set the smallest address
                }

            traverser_prev = traverser;
            traverser = traverser->next;
            }
        //We now have the smallest address

        //Remove the element from the list -- special case if element was head
        if(leastAddr == oldListHead->curr)
            {
            oldListHead = oldListHead->next;
            }
        else 
            {
            ((struct memHead*)leastAddr_prev)->next = ((struct memHead*)leastAddr)->next;
            }

        //Move element to new list -- specical case if new list is empty
        if(tempHead == NULL)
            {
            tempHead = (struct memHead*)leastAddr;
            tempTail = (struct memHead*)leastAddr;
            ((struct memHead*)leastAddr)->next = NULL;
            }
        else 
            {
            tempTail->next = (struct memHead*)leastAddr;
            tempTail = tempTail->next;
            ((struct memHead*)leastAddr)->next = NULL;
            }
        }

    //Set the new header of the list
    *listHead = tempHead;
    }
