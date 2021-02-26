// Anuneet Anand
// 2018022
// OS - Assignment 1 
// Memory Allocator 

#define _GNU_SOURCE
#define PAGE_SIZE 4096
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include "memory.h"

// Node
typedef struct Node
{
	int32_t size;
	int32_t bytes;
	struct Node *next;
} Node;

Node *memory_space[10];
int mi = 0; 

// To Initialise Memory Space Array
void set_up_memory_space()
{
	for(int i=0;i<10;i++)
		memory_space[i] = NULL;
	mi = 1;
}

// Given
static void *alloc_from_ram(size_t size)
{
	assert((size % PAGE_SIZE) == 0 && "size must be multiples of 4096");
	void* base = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
	if (base == MAP_FAILED)
	{
		printf("Unable to allocate RAM space\n");
		exit(0);
	}
	return base;
}

// Given
static void free_ram(void *addr, size_t size)
{
	munmap(addr, size);
}

// To Add Nodes To Front Of LinkedList
void add_Node(Node *head, Node *ptr)
{
	if (head != NULL) {ptr->next = head;}
	head = ptr;
}

// To Add Nodes Of A New Page To Linkedlist
void add_page(int n)
{
	int m = n;
	Node *cur = (Node*)alloc_from_ram(4096); 
	cur->next = NULL;
	cur->size = 16; 
	cur->bytes = 4080;
	
	if (n == 8)
	{cur->size = 4080;}
	else 
	{
		while(n>0)
		{cur->size = cur->size * 2;n--;}
	}
	
	int number_of_Nodes = (int) (4080/cur->size);
	int x = (cur->size)/16;
	Node *temp = cur + 1;

	while (number_of_Nodes)
	{
		temp->size = 0; temp->bytes = cur->size; temp->next = NULL;
		if(memory_space[m]==NULL) 
		{memory_space[m]=temp;}
		else 
		{add_Node(memory_space[m],temp);}
		temp = temp + x;
		number_of_Nodes--;
	}	
}

// To Remove A Node From LinkedList For Using
Node *remove_Node(int n) 
{
	if (memory_space[n] == NULL) { return NULL;}
	else
	{
		Node *cur = memory_space[n];
		memory_space[n] = memory_space[n]->next;
		cur->next = NULL;
		Node *meta = (Node*)((((long long)(cur))>>12)<<12);
		meta->bytes = meta->bytes - meta->size;
		return cur;
	}
} 

// To Remove All Nodes From LinkedList Which Are Not In Use
void free_Node(Node *meta, int n)
{
	Node *prev = memory_space[n];
	Node *cur = NULL;
	if (prev!=NULL)
	{cur = prev->next;}
	while(cur!=NULL)
	{
		Node *temp = (Node*)((((long long)(cur))>>12)<<12);
		if (temp == meta)
		{prev->next = cur->next;}
		cur=cur->next;
		prev=prev->next;
	}
	if ((memory_space[n]!=NULL)&&(memory_space[n]=meta))
	{memory_space[n]=NULL;}
}

// To Find Nearest Power Of Two
int find_nearest_power(size_t size)
{
	int i = 1;
	if (size && !(size & (size - 1))) {i = (int) size;} 
    while (i < size)  {i = i<<1;} 
    return i;
}

// To Calculate Index For memory_space Array
int find_index(size_t size)
{
	int j=0;
	int i = find_nearest_power(size);
	if (i<16) {i = 16;}
    while (i>16) {i = i / 2; j++;}
    return j; 
}

// To Free A Given Memory Location
void myfree(void *ptr)
{
	Node *p = ((Node*)ptr)-1;
	if (p->size>4080) {free_ram(p,p->size);}
	else
	{
		Node *meta = (Node*)((((long long)(ptr))>>12)<<12);
		int x = find_index(meta->size);
		add_Node(memory_space[x],ptr);
		meta->bytes = meta->bytes + meta->size;
		if (meta->bytes==4080)
		{
			Node *dump = meta;
			free_Node(meta,x);
			free_ram(dump,dump->size);
		}
	}
}

// To Allocate Memory For User's Demand 
void *mymalloc(size_t size)
{
	Node *m;
	if (mi == 0)
	{set_up_memory_space();}
	if (size<=4080)
	{
		int x = find_index(size);
		m = remove_Node(x);
		if (m == NULL)
		{
			add_page(x);
			m = remove_Node(x);
		}
	}
	else
	{	
		size = size + 16;
		if (size%4096!=0) {size = ((int)(size/4096))*4096 + 4096;}
		Node *md = alloc_from_ram(size);
		md->size = size;
		md->bytes = size;
		m = md + 1;
		add_Node(memory_space[9],m);
	}
	return m;
}

// END OF CODE
