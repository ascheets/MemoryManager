#include "MemoryManager.h"

using namespace std;

//ctor
MemoryManager :: MemoryManager(char* p_mem, int size)
{
    cout << "Inside MemoryManager, sizeof(Node) is " << sizeof(Node) << endl;

    p_memory = p_mem;
    sizeOfHeap = size;

    void* p_temp;
    
    //setting first node directly
    p_first = (Node*) p_memory;
    p_first -> p_prev = 0;
    
    //pointer arithmetic with temp pointers
    
    //find p_first->p_next
    p_temp = p_memory + size - sizeof(Node);
    p_first -> p_next = (Node*) p_temp;
    
    //setting non ptr data (int, bool)
    p_first -> sizeOfNode = size - (2*sizeof(Node));
    p_first -> isFree = true;

    //setting last node
    p_last = (Node*) p_temp;
    p_last -> p_prev = p_first;
    p_last -> p_next = 0;
    p_last -> sizeOfNode = 0;
    p_last -> isFree = false;    

    //just checking
    //Node* p_firstStart = p_first + 1;
    //Node* p_nextEnd = p_last++;

    //initialize number of nodes
    numNodes = 1;

}

//dump
void MemoryManager :: dump()
{
    //start at p_first, increment printing node data
    Node* p_search = p_first;

    int count = 0;

    while(p_search != 0)
    {
	//so that we don't print out information about tail node
	if(count == numNodes)
	{
	    break;
	}

	//print out relevant information
	cout << "Block " << count + 1 << ": ";

	//if space is free
	if(p_search -> isFree)
	{
	    cout << p_search->sizeOfNode << " bytes of free space" << endl;
	}
	else
	{
	    cout << p_search->sizeOfNode << " bytes of used space" << endl;
	}

	//increment p_search
	p_search = p_search->p_next;

	count++;
    }
    

}

//allocate
void* MemoryManager :: allocate(int size)
{
    //ptr which will eventually be returned
    void* retPtr = 0;

    //start at p_first, increment printing node data
    Node* p_search = p_first;

    //stores the largest block of data going to allocate on
    Node* p_largest = p_first;

    while(p_search != 0)
    {
	//need function, find largest, free block
	if(p_search->sizeOfNode > p_largest->sizeOfNode)
	{
	    //is the space free?
	    if(p_search->isFree)
	    {
		p_largest = p_search;
	    }
	}	

	//increment p_search
	p_search = p_search->p_next;

    }

    //is the space large enough?
    if(p_largest->sizeOfNode > size)
    {
	//allocate

	//need to create new node, at correct location
	//finding address of new node

	//set marker to address of p_largest plus
	//size of one node
	//char* marker = (char*) (p_largest + 1);
		
	//this is the address of the new node
	char* address = ((char*)p_largest) + sizeof(Node) + size;
		
	//save address of old p_largest->p_next
	Node* p_temp = p_largest->p_next;

	//start changing p_largest->p_next to
	//represent new node
	Node* p_newNode = (Node*) address;
	//p_largest->p_next = (Node*) address;
	//set address of p_newNode to be retPtr
	retPtr = (void*) (p_largest + 1);

	//make sure all of node data is correct
	p_largest->isFree = false;
	p_newNode->isFree = true;

	//make sure node sizes are correct
	p_newNode->sizeOfNode = (char*)(p_temp) - (char*)(p_newNode+1);
	p_largest->sizeOfNode = size;		
		
	//update list pointers
	p_newNode->p_prev = p_largest;

	p_largest->p_next = p_newNode;
		
	p_temp->p_prev = p_newNode;
	p_newNode->p_next = p_temp;

	//no special cases since never inserting...
	//before first or after last...?

	//update numNodes in list
	numNodes++;

    }

    return retPtr;
}

//deallocate
void MemoryManager :: deallocate(void* p_mem)
{
    //is the address null?
    if(p_mem == 0)
    {
	return;
    }

    //mark the block as free
    Node* p_toFree = (Node*) p_mem - 1;

    p_toFree->isFree = true;

    //check adjacent blocks, perhaps coalesce?
    if(p_toFree->p_prev != 0 &&
	    p_toFree->p_next != 0)
    {
	if(p_toFree->p_prev->isFree &&
	   p_toFree->p_next->isFree)
	{
	    cout << "Two can be merged" << endl;

	    //keeping p_toFree->p_prev
	    //update size of p_toFree->p_prev
	    p_toFree->p_prev->sizeOfNode = (char*) (p_toFree->p_next->p_next) 
		- (char*) (p_toFree->p_prev+1);

	    //update p_next of p_toFree->p_prev
	    p_toFree->p_prev->p_next = p_toFree->p_next->p_next;

	    //update p_prev of p_toFree->p_next->p_next
	    p_toFree->p_next->p_next->p_prev = p_toFree->p_prev;

	    //set p_toFree and p_toFree->p_next to null
	    p_toFree->p_next = 0;
	    p_toFree = 0;

	    //decrement numNodes
	    numNodes = numNodes - 2;

	}
	//case 2 in my notes
	else if(p_toFree->p_prev->isFree)
	{
	    cout << "One can be merged" << endl;

	    //keeping p_toFree->p_prev
	    //update size of p_toFree->p_prev
	    p_toFree->p_prev->sizeOfNode = (char*) (p_toFree->p_next) - (char*) (p_toFree->p_prev+1);

	    //update p_next of p_toFree->p_prev
	    p_toFree->p_prev->p_next = p_toFree->p_next;

	    //update p_prev of p_toFree->p_next
	    p_toFree->p_next->p_prev = p_toFree->p_prev;

	    //set p_toFree to null
	    p_toFree = 0;

	    //decrement numNodes
	    numNodes--;

	}
	//case 1 in my notes
	else if(p_toFree->p_next->isFree)
	{
	    cout << "One can be merged" << endl;

	    Node* p_toDelete = p_toFree->p_next;

	    //keeping p_toFree
	    //update size of p_toFree
	    p_toFree->sizeOfNode = (char*) (p_toFree->p_next->p_next) - (char*) (p_toFree + 1);

	    //update p_toFree->p_next
	    p_toFree->p_next = p_toFree->p_next->p_next;

	    //udpate p_prev of p_toFree->p_next->p_next
	    p_toFree->p_next->p_prev = p_toFree;

	    //want to get rid of Node pointer?
	    p_toDelete = 0;

	    //decrement numNodes
	    numNodes--;

	}
    }
    //this is case where p_toFree is tail...shouldn't ever be called
    else if(p_toFree->p_prev != 0)
    {
	if(p_toFree->p_prev->isFree)
	{
	    cout << "One can be merged" << endl;
	    cout << "p_toFree is the tail..." << endl;
	}
    }
    //this is case where p_toFree is head...could be called
    else if(p_toFree->p_next != 0)
    {
	if(p_toFree->p_next->isFree)
	{
	    cout << "One can be merged" << endl;
	    cout << "p_toFree is the head..." << endl;

	    //want to keep p_toFree
	    //update size of p_toFree
	    p_toFree->sizeOfNode = (char*) (p_toFree->p_next->p_next) - (char*) (p_toFree + 1);

	    //update p_next of p_toFree
	    p_toFree->p_next = p_toFree->p_next->p_next;

	    //udpate p_prev of new p_next
	    p_toFree->p_next->p_prev = p_toFree;

	    //decrement numNodes
	    numNodes--;
	}
    }    
}
