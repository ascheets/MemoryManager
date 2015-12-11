#pragma once
#include <iostream>

using namespace std;

class MemoryManager
{
 public:
    MemoryManager(char* p_mem, int size);
    void dump();
    void* allocate(int size);
    void deallocate(void* p_mem);

 private:
    struct Node
    {
	Node* p_prev;
	Node* p_next;
	int sizeOfNode;
	bool isFree;
    };
    
    Node* p_first;
    Node* p_last;
    char* p_memory;
    int sizeOfHeap;    
    int numNodes;
};
