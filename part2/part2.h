
#ifndef CW2_PART2_H
#define CW2_PART2_H

typedef struct NodeStruct{

    int free;
    size_t size;
    void *memory;
    struct NodeStruct* prevNode;
    struct NodeStruct* nextNode;

}Node;


void*(*allocate)(size_t);
void deallocate ( void * memory );
void initialise ( void * memory , size_t size, char* algorithm);

void *firstFit(size_t bytes);
void *nextFit(size_t bytes);
void *bestFit(size_t bytes);
void *worstFit(size_t bytes);

#endif //CW2_PART2_H


