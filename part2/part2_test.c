/* --------------------------------------------------------------------------------------------------
|                                                                                                    |
|   Title: Architectures & Operating Systems: Coursework 2 - Thread-safe Memory Manager.             |
|                                                                                                    |
|                                        part2 TEST HARNESS                                          |
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
#include "part2.h"
#include <string.h>


//global variables.
Node *HEAD = NULL;
Node *NEXTFITNODE = NULL;


//Function Set as a function pointer so that the different algorithms can be applied.
/*
 * Inputs: size_t variable.
 * Outputs: NONE.
 * Description: function pointer to other algorithm allocate functions.
 */
void*(*allocate)(size_t);

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
void *firstFit(size_t bytes) {
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
 * Inputs: size_t bytes.
 * Outputs: (void*) ->memory. Memory address of allocated memory.
 * Description: Uses the NextFit algorithm to allocate memory from the heap.
 */
void* nextFit(size_t bytes){

    //Only allow valid amount of bytes.
    if ((int)bytes <= 0){
        printf("Invalid amount of bytes. Returning NULL\n");
        return NULL;
    }

    //Set the initial start location to the NEXTFITNODE node.
    Node *nodePointer = NEXTFITNODE;

    //Set the end node to the start node, therefore it will only loop once and not infinitely.
    Node *limitNode = NEXTFITNODE;

    int allocateBool = 1;

    //Loop through every node.
    while (allocateBool == 1) {

        //If the node is free and is large enough for the allocated bytes.
        if (nodePointer->free == 1 && nodePointer->size > (bytes + sizeof(Node))) {

            //Call the allocate function to allocate the node.
            Node *returnNode = allocateNodeWithHole(nodePointer,bytes);

            //update NEXTFITNODE.
            NEXTFITNODE = returnNode->nextNode;

            //return the allocated nodes memory address.
            return nodePointer->memory;
        }

            //if the node is the exact size.
        else if (nodePointer->free == 1 && nodePointer->size == bytes) {

            //Set free to 0 as node is taken.
            printf("Found node of exact size.\n");
            nodePointer->free = 0;

            //update NEXTFITNODE,
            if(nodePointer->nextNode != NULL) {
                NEXTFITNODE = nodePointer->nextNode;
            }
            else{
                NEXTFITNODE = HEAD;
            }

            //return the allocated nodes memory address.
            return nodePointer->memory;

        }
        //if the node is taken.
        else {

            //Look at next node.
            if (nodePointer->nextNode == NULL) {

                //loop through whole list
                nodePointer = HEAD;
            }
            else{
                nodePointer = nodePointer->nextNode;
            }
            if(nodePointer->memory == limitNode->memory){

                printf("!!!! No free nodes, returning NULL !!!!\n");

                //update NEXTFITNODE.
                NEXTFITNODE= HEAD;

                //Return NULL as no valid nodes found.
                return  NULL;
            }


        }
    }
}


/*
 * Inputs: size_t bytes.
 * Outputs: (void*) ->memory. Memory address of allocated memory.
 * Description: Uses the BestFit algorithm to allocate memory from the heap.
 */
void* bestFit(size_t bytes){
    //Only allow valid amount of bytes.
    if ((int)bytes <= 0){
        printf("Invalid amount of bytes. Returning NULL\n");
        return NULL;
    }

    //Set the initial node to the HEAD node.
    //Loop through all the nodes and find the best node that fits.
    int found = 0;
    int bestFit = 1;
    Node *bestNode = NULL;
    Node *nodePointer = HEAD;

    while(bestFit){
        if(nodePointer->free){

            //IF the node is of exact size.
            if (nodePointer->size == bytes){
                bestNode = nodePointer;
                found = 1;
                break;
            }

            //IF the node is greater than bytes
            if(nodePointer->size > (bytes + (sizeof(Node)))){
                found = 1;
                if(bestNode == NULL){
                    bestNode = nodePointer;
                }
                if(nodePointer->size < bestNode->size){
                    bestNode = nodePointer;
                }

            }
            //If there is enough size to create a new node but not assign a size > 0 to it.
            if(nodePointer->size == (bytes +(sizeof(Node)))){
                printf("Invalid node to use. searching next\n");

            }
            //If there is a next node.
            if (nodePointer->nextNode != NULL) {
                nodePointer = nodePointer->nextNode;
            }
            else{
                if(bestNode == NULL){
                    bestNode = nodePointer;
                }
                break;

            }
        }
        //Look at next node.
        else{
            if (nodePointer->nextNode != NULL) {
                nodePointer = nodePointer->nextNode;
            }
            else{
                printf("!!!! No free nodes, returning NULL !!!!\n");

                //Return NULL as no valid nodes found.
		        return NULL;
            }
        }


    }

    //Allocate the best node found with the desired bytes.
    if(found) {
        //If the node is free and is large enough for the allocated bytes.
        if (bestNode->free == 1 && bestNode->size > (bytes + sizeof(Node))) {

            //Call the allocate function to allocate the node.
            Node* returnNode = allocateNodeWithHole(bestNode,bytes);

            //return the allocated nodes memory address.
            return returnNode->memory;
        }


        //if the node is the exact size.
        else if (bestNode->free == 1 && bestNode->size == bytes) {

            printf("found node of exact size.\n");
            //Set free to 0 as node is taken.
            bestNode->free = 0;

            //return the allocated nodes memory address.
            return bestNode->memory;
        }

    }
    //If no suitable nodes found, return NULL.
    else {
        printf("!!!! No free nodes, returning NULL !!!!\n");
        //Return NULL as no valid nodes found.
        return NULL;

    }



}


/*
 * Inputs: size_t bytes.
 * Outputs: (void*) ->memory. Memory address of allocated memory.
 * Description: Uses the WorstFit algorithm to allocate memory from the heap.
 */
void* worstFit(size_t bytes) {
    //Only allow valid amount of bytes.
    if ((int)bytes <= 0){
        printf("Invalid amount of bytes. Returning NULL\n");
        return NULL;
    }


    //Set the initial node to the HEAD node.
    //Loop through all the nodes and find the worst node that fits.
    int found = 0;
    int worstFit = 1;
    Node *worstNode = NULL;
    Node *nodePointer = HEAD;

    while (worstFit) {
        if (nodePointer->free) {

            //IF the node is of exact size.
            if (nodePointer->size == bytes) {
                found = 1;
                if (worstNode == NULL) {
                    worstNode = nodePointer;
                } else if (nodePointer->size > worstNode->size) {
                    worstNode = nodePointer;
                }

            }

                //IF the node is greater than bytes
            else if (nodePointer->size > (bytes + (sizeof(Node)))) {
                found = 1;
                if (worstNode == NULL) {
                    worstNode = nodePointer;
                }
                if (nodePointer->size > worstNode->size) {
                    worstNode = nodePointer;
                }

            } else if (nodePointer->size == (bytes + (sizeof(Node)))) {
                printf("Invalid node to use. searching next\n");

            }

        }
            if (nodePointer->nextNode != NULL) {
                nodePointer = nodePointer->nextNode;
            } else {
                break;
            }

    }

    if(found) {

        //If the node is free and is large enough for the allocated bytes.
        if (worstNode->free == 1 && worstNode->size > (bytes + sizeof(Node))) {

            //Call the allocate function to allocate the node.
            Node* returnNode = allocateNodeWithHole(worstNode,bytes);

	        //return the allocated nodes memory address.
            return returnNode->memory;
        }



        //if the node is the exact size.
        else if (worstNode->free == 1 && worstNode->size == bytes) {
            printf("found node of exact size.\n");
            //Set free to 0 as node is taken.
            worstNode->free = 0;

            //return the allocated nodes memory address.
            return worstNode->memory;

        }

    }
    //If not found.
    else {
        printf("!!!! No free nodes, returning NULL !!!!\n");

	    return NULL;

    }
}


/*
* Inputs: void* memory, size_t size, char* algorithm.
* Outputs: void.
* Description: Initialises the type of allocation algorithm and creates the initial HEAD node.
*/
void initialise(void *memory ,size_t size, char* algorithm){

    //Allocate (size) amount of memory to the heap (called memory in this case).
    //Returns pointer to the start of the address.

    //Allocate the head node the memory it needs.
    HEAD = (Node*)(memory);


    //Assign type of algorithm

    if(strcmp(algorithm,"NextFit")==0){
        printf("Using NextFit algorithm.\n");
        allocate = nextFit;
        NEXTFITNODE = HEAD;

    }
    else if(strcmp(algorithm, "BestFit")==0){
        printf("Using BestFit algorithm.\n");
        allocate = bestFit;

    }
    else if(strcmp(algorithm,"WorstFit")==0){
        printf("Using WorstFit algorithm.\n");
        allocate = worstFit;

    }
    //Default first fit
    else{
        printf("Using FirstFit algorithm.\n");
        allocate = firstFit;

    }


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

                //If NextFit algorithm is set, handle the pointer to it.
                if (NEXTFITNODE != NULL){
                    if(NEXTFITNODE->memory == connectedHolesPointer->nextNode->memory){
                        printf("Moving NEXTFITNODE due to coalace taking place.\n");
                        NEXTFITNODE = connectedHolesPointer;
                    }
                 }

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



int main() {
    /*
     * TEST HARNESS FOR EACH ALGORITHM COMMENTED OUT.
     */

    //Set the initial heap size.
    size_t size = 1024;

    //Allocate memory for the heap.
    void *memory = malloc(size);

    //FirstFit Algorithm test harness
    //The test allocation should go into node x.

    char * algo = "FirstFit";
    initialise(memory,size,algo);

    void *x = allocate(100);
    printf("allocate x output: %d\n===============================\n\nVisual Node output:\n\n",(char*)x);
    output();

    void *y = allocate(50);
    printf("allocate y output: %d\n===============================\n\nVisual Node output:\n\n",(char*)y);
    output();

    void *z = allocate(100);
    printf("allocate z output: %d\n===============================\n\nVisual Node output:\n\n",(char*)z);
    output();

    void *t = allocate(100);
    printf("allocate t output: %d\n===============================\n\nVisual Node output:\n\n",(char*)z);
    output();

    deallocate(x);
    deallocate(z);

    printf("De-allocate x & z: \n===============================\n\nVisual Node output:\n\n");
    output();

    void *test = allocate(70);
    printf("test allocation of 70: \n===============================\n\nVisual Node output:\n\n");
    output();



    //NextFit Algorithm test harness
    //The test allocation should go into node last node, as that is the NEXTFITNODE due to the algorithm.
    /*
    char * algo = "NextFit";
    initialise(memory,size,algo);

    void *x = allocate(100);
    printf("allocate x output: %d\n===============================\n\nVisual Node output:\n\n",(char*)x);
    output();

    void *y = allocate(50);
    printf("allocate y output: %d\n===============================\n\nVisual Node output:\n\n",(char*)y);
    output();

    void *z = allocate(100);
    printf("allocate z output: %d\n===============================\n\nVisual Node output:\n\n",(char*)z);
    output();

    void *t = allocate(100);
    printf("allocate t output: %d\n===============================\n\nVisual Node output:\n\n",(char*)z);
    output();

    deallocate(x);
    deallocate(z);

    printf("De-allocate x & z: \n===============================\n\nVisual Node output:\n\n");
    output();

    void *test = allocate(70);
    printf("test allocation of 70: \n===============================\n\nVisual Node output:\n\n");
    output();
    */


    //BestFit Algorithm Test Harness
    //test1 should go in x, test2 should go in z.
    /*
    char * algo = "BestFit";
    initialise(memory,size,algo);

    void *x = allocate(50);
    void *y = allocate(100);
    void *z = allocate(200);
    void *b = allocate(30);
    printf("Allocate 4 nodes: \n===============================\n\nVisual Node output:\n\n");
    output();

    deallocate(x);
    deallocate(z);

    printf("De-allocate x & z: \n===============================\n\nVisual Node output:\n\n");
    output();

    //Should replace node x;
    void *test1 = allocate(50);
    printf("test allocation of 50: \n===============================\n\nVisual Node output:\n\n");
    output();

    deallocate(test1);
    printf("De-allocation of testAllocation: \n===============================\n\nVisual Node output:\n\n");
    output();

    //Should use node z, and create new hole:
    void *test2 = allocate(150);
    printf("test2 of 150: \n===============================\n\nVisual Node output:\n\n");
    output();
     */


    //WorstFit Algorithm Test Harness
    //test1 should go in the hole after b, and so should test2
    /*
    char * algo = "WorstFit";
    initialise(memory,size,algo);

    void *x = allocate(50);
    void *y = allocate(100);
    void *z = allocate(200);
    void *b = allocate(30);
    printf("Allocate 4 nodes: \n===============================\n\nVisual Node output:\n\n");
    output();

    deallocate(x);
    deallocate(z);

    printf("De-allocate x & z: \n===============================\n\nVisual Node output:\n\n");
    output();

    //Should replace node x;
    void *test1 = allocate(50);
    printf("test allocation of 50: \n===============================\n\nVisual Node output:\n\n");
    output();

    deallocate(test1);
    printf("De-allocation of testAllocation: \n===============================\n\nVisual Node output:\n\n");
    output();

    //Should use node z, and create new hole:
    void *test2 = allocate(150);
    printf("test allocation of 150: \n===============================\n\nVisual Node output:\n\n");
    output();
     */
    free(memory);
    return EXIT_SUCCESS;

}


