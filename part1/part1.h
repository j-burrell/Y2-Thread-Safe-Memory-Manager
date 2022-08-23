
#ifndef CW2_PART1_H
#define CW2_PART1_H

typedef struct NodeStruct{

    int free;
    size_t size;
    void *memory;
    struct NodeStruct* prevNode;
    struct NodeStruct* nextNode;

}Node;


void *allocate(size_t);
void deallocate ( void * memory );
void initialise ( void * memory , size_t size);


#endif //CW2_PART1_H


