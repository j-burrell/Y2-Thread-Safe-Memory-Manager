/* --------------------------------------------------------------------------------------------------
|                                                                                                    |
|   Title: Architectures & Operating Systems: Coursework 2 - Thread-safe Memory Manager.             |
|                                                                                                    |
|                                        part1                                                       |
|                                                                                                    |
|   Authors: Buzz Embley-Riches 100237137 & James Burrell 100263300.                                 |
|                                                                                                    |
|   Last edit date: 06/12/19                                                                         |
|                                                                                                    |
|   Description: Program to simulate a thread-safe memory manager using a linked list                |
|                implementation.                                                                     |
|                                                                                                    |
 --------------------------------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include "part1.h"



//global variables.
Node *HEAD = NULL;

/*
 * Inputs: Node* nodePointer, size_t bytes.
 * Outputs: Node* (return node).
 * Description: Function takes a node, allocates a size to it, and creates a free node next to it with the remaining space.
 */
Node* allocateNodeWithHole(Node* nodePointer, size_t bytes){
    printf("Found node of suitable size, creating, allocating space and creating node\n");

    //Allocate memory for the struct hole node at the current nodes memory plus a struct.
    Node *nextNode = (Node *) (nodePointer->memory + bytes);

    //memory address at the start of the empty hole node.
    void *memoryStart2 = (((char *) nodePointer->memory) + bytes + sizeof(Node));

    //Assign the hole nodes variables.
    nextNode->memory = memoryStart2;
    nextNode->free = 1;
    nextNode->size = nodePointer->size - bytes - sizeof(Node);
    nextNode->nextNode = nodePointer->nextNode;
    nextNode->prevNode = nodePointer;

    //allocate the taken size.
    nodePointer->size = bytes;

    //Set free bool to 0 as this node is taken.
    nodePointer->free = 0;

    //Point the current node to the next node which is the created hole
    nodePointer->nextNode = nextNode;

    //return the taken nodes memory address.
    return nodePointer;
}


/*
 * Inputs: size_t bytes.
 * Outputs: (void*) ->memory. Memory address of allocated memory.
 * Description: Uses the FirstFit algorithm to allocate memory from the heap.
 */
void *allocate(size_t bytes) {
    //Only allow valid amount of bytes.
    if ((int)bytes <= 0){
        printf("Invalid amount of bytes. Returning NULL\n");
        return NULL;
    }

    //Set the initial start location to the HEAD node.
    Node *nodePointer = HEAD;

    int allocateBool = 1;

    //Loop through every node.
    while (allocateBool == 1) {

        //If the node is free and is large enough for the allocated bytes.
        if (nodePointer->free == 1 && nodePointer->size > (bytes + sizeof(Node))) {

            //Call the allocate function to allocate the node.
            Node* returnNode = allocateNodeWithHole(nodePointer,bytes);
		
	        printf("%d bytes allocated.\n",bytes);

            //Return the memory address of the allocated node.
            return returnNode->memory;
        }

        //if the node is the exact size.
        else if (nodePointer->free == 1 && nodePointer->size == bytes) {

            printf("found node of exact size.\n");

            //Set free to 0 as node is taken.
            nodePointer->free = 0;

            //Return the memory address of the allocated node.
            return nodePointer->memory;

        }
        //if the node is taken.
        else {
            //If the next node is NULL.
            if (nodePointer->nextNode == NULL) {
                printf("!!!! No free nodes, returning NULL !!!!\n");

                //Return NULL as no valid nodes found.
                return NULL;
            }
            //Look at next node, and continue in the loop.
            nodePointer = nodePointer->nextNode;
        }
    }

}


/*
* Inputs: void* memory, size_t size, char* algorithm.
* Outputs: void.
* Description: Initialises the type of allocation algorithm and creates the initial HEAD node.
*/
void initialise(void *memory ,size_t size){

    //Allocate (size) amount of memory to the heap (called memory in this case).
    //Returns pointer to the start of the address.

    //Allocate the head node the memory it needs.
    HEAD = (Node*)(memory);

    //memoryStart is the memory address where data can be stored.
    //This is done so the struct data come before it.
    void *memoryStart = ((char*)HEAD) + sizeof(Node);

    //Assign all the nodes variables.
    HEAD->size = size - sizeof(Node);
    HEAD->memory = memoryStart;
    HEAD->free = 1;
    HEAD->nextNode = NULL;
    HEAD->prevNode = NULL;

}

/*
 * Inputs: void* memory.
 * Outputs: void.
 * Description: Deallocates a node (frees it) based on the input of its memory address.
 */
void deallocate ( void * memory ){

    //Assign new node pointer to HEAD node.
    Node *nodePointer = HEAD;

    //int to act as bool for while loop.
    int deallocateBool = 1;


    while(deallocateBool == 1){

        //if the memory address of dealocation equals the memory address of a node.

        if(nodePointer->memory != memory){

            //if the next node is null, current node is the last in the linked list.
            if (nodePointer->nextNode == NULL){
                deallocateBool = 0;
                break;
            }
            else {

                //look at next node.
                nodePointer = nodePointer->nextNode;
            }
        }

            //else the node is the memory address.
        else{
            //Set the nodes free variable to 1, as node has been deallocated.
            printf("De-allocation successful\n");
            nodePointer->free=1;
            deallocateBool = 0;
            break;
        }
    }

    //Next part coalaces connected holes.

    //int to act as bool for while loop.
    int connectedHoleSearch = 1;

    //Assign a new node pointer to the HEAD node.
    Node *connectedHolesPointer = HEAD;


    while (connectedHoleSearch == 1){

        //if the node is free.
        if (connectedHolesPointer == NULL){
            break;
        }
        if (connectedHolesPointer->free == 1){

            //if the node is null.
            if (connectedHolesPointer->nextNode == NULL){

                //set the loop bool to 0.
                connectedHoleSearch = 0;
                break;
            }

            //if the next node is free.
            if (connectedHolesPointer->nextNode->free == 1) {

                //Increase the current nodes size, to that containing both nodes.
                connectedHolesPointer->size = connectedHolesPointer->size +
                        sizeof(Node)+connectedHolesPointer->nextNode->size;



                //if the next next node is NULL
                if (connectedHolesPointer->nextNode->nextNode == NULL){
                    //Assign a new node pointer to current nodes next next node.
                    Node *nextNode = connectedHolesPointer->nextNode->nextNode;

                    //Assign the current nodes next node to ^.
                    connectedHolesPointer->nextNode = nextNode;
                    connectedHoleSearch = 0;
                    break;
                }

                else {
                    //Assign a new node pointer to current nodes next next node.
                    Node *nextNode = connectedHolesPointer->nextNode->nextNode;

                    //Assign the current nodes next node to ^.
                    connectedHolesPointer->nextNode = nextNode;
                }

            }
            else{

                //Look at next node.
                connectedHolesPointer = connectedHolesPointer->nextNode;
            }

        }
        else{
            //look at next node.
            connectedHolesPointer = connectedHolesPointer->nextNode;

        }

    }

}
/*
 * Inputs: void
 * Outputs: void
 * Description: Outputs all nodes.
 */
void output(){
    Node *point = HEAD;
    int loop = 1;

    while (loop ==1){
        //if next node is null, at the end of the linked list.
        if (!point->nextNode){
            loop = 0;
            printf("Node \n size: %d\n free: %d\n node start: %d\n memory start: %d\n\n",
                    (int)point->size,point->free,(point->memory-sizeof(Node)),point->memory);
            break;
        }else{
            //Output information about a node.
            printf("Node \n size: %d\n free: %d\n node start: %d\n memory start: %d\n\n",
                    (int)point->size,point->free,(point->memory-sizeof(Node)),point->memory);
            //look at next node.
            point = point->nextNode;
        }

    }
}


