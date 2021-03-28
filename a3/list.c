#include "list.h"
#include <stdlib.h>

/* 
 * Allocate memory for a node of type strct nodeStruct and initialize it with the value
 * item. Return a pointer to the new node
 */
struct nodeStruct* List_createNode(int item){
    struct nodeStruct* newNode = (struct nodeStruct*) malloc( 1*sizeof(struct nodeStruct) );
    if(newNode == NULL){
        return NULL;
    }
    newNode->item = item;
    newNode->next = NULL;
    return newNode;
}

/* 
 * Insert node at the head of the list
 */
void List_insertHead( struct nodeStruct** headRef, struct nodeStruct* node){
    /* Make head point to new node, and new node point to old head */
    struct nodeStruct* temp = *headRef;
    *headRef = node;
    node->next = temp;
}

/* 
 * Insert nose after the tail of the list
 */
void List_insertTail( struct nodeStruct** headRef, struct nodeStruct* node){
    struct nodeStruct* curNode = *headRef;

    /* If head ref if NULL, no nodes in list, add forst node into list */
    if(curNode == NULL){
        *headRef = node;
        return;
    }
    
    /* First node not NULL, traverse the linked list until next node is NULL (end of the list) */
    while(curNode->next != NULL){
        curNode = curNode->next;
    }

    /* Add the node to the end of the list */
    curNode->next = node;
}

/* 
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes( struct nodeStruct* head ){
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
 * Return the first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode( struct nodeStruct* head, int item ){
    while(head->item != item && head->next != NULL){
        head = head->next;
    }
    if(head->item == item){
        return head;
    } else {
        return NULL;
    }

}

/* 
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set to a valid node
 * in the list. For example, the client code may have found it by calling
 * List_findNode(). If the list contains only one node, the head of the
 * list should be set to NULL
 */
void List_deleteNode( struct nodeStruct** headRef, struct nodeStruct* node ){
    struct nodeStruct* temp = NULL;
    struct nodeStruct* prevNode = NULL;   

    /* Check if head is node, make head point to next node if so */
    if(*headRef == node){
        temp = *headRef;
        *headRef = temp->next;
    } else{
        temp = (*headRef)->next;
        prevNode = *headRef;

        while(temp != node && temp != NULL){
            prevNode = temp; 
            temp = temp->next;
        }
        prevNode->next = temp->next;
    }

    /* Assumption that the node has been found (as stated in problem discription)
       remove node and link the two nodes on either side */

    if(temp != NULL)
        free(temp);
}

/*
 * Sort the list in ascending order based on the item field
 * Any sort algorithm is fine
 */
void List_sort( struct nodeStruct **headRef ){
    struct nodeStruct* notSortedYet = *headRef;
    struct nodeStruct* traverser;
    int tempItem;

    while(notSortedYet != NULL){
        traverser = notSortedYet;

        while(traverser != NULL){
            if(traverser->item < notSortedYet->item){
                tempItem = notSortedYet->item;
                notSortedYet->item = traverser->item;
                traverser->item = tempItem;
            }

            traverser = traverser->next;
        }

        notSortedYet = notSortedYet->next;
    }

}
