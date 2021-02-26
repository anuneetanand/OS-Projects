// Anuneet Anand
// 2018022
// OS Assignment - 2 - Optional 

//apt-get install gcc-multilib
#include "thread.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

// thread metadata
struct thread 
{
	void *esp;
	void *ebp; 						 // base pointer required to free stack
	struct thread *next;
	struct thread *prev;

};

struct thread *ready_list = NULL;     // ready list
struct thread *junk = NULL;			  // junk thread
struct lock *Lock=NULL;				  // lock
struct thread *cur_thread = NULL;     // current thread

// defined in context.s
void context_switch(struct thread *prev, struct thread *next);

// insert the input thread to the end of the ready list.
static void push_back(struct thread *t)
{
	if (ready_list == NULL)
	{
		ready_list = t;
		ready_list->next = ready_list;
		ready_list->prev = ready_list;
	}
	else
	{
		t->next = ready_list;
		t->prev = ready_list->prev;
		ready_list->prev->next = t;
		ready_list->prev = t;
	}
}

// remove the first thread from the ready list and return to caller.
static struct thread *pop_front()
{
	struct thread *ptr = ready_list;
	if (ready_list == NULL)
	{ return ptr; }
	else if (ready_list->next == ready_list)
	{ ready_list = NULL; }
	else
	{
		ready_list->next->prev = ready_list->prev;
		ready_list->prev->next = ready_list->next;
		ready_list = ready_list->next;
	}
	ptr->next = NULL;
	ptr->prev = NULL;
	return ptr;
}

// the next thread to schedule is the first thread in the ready list.
// obtain the next thread from the ready list and call context_switch.
static void schedule()
{
	struct thread *prev = cur_thread;
	struct thread *next = pop_front(); 
	cur_thread = next;
	context_switch(prev, next);
}

// push the cur_thread to the end of the ready list and call schedule
// if cur_thread is null, allocate struct thread for cur_thread
static void schedule1()
{
	if (cur_thread == NULL)
	{
		cur_thread = malloc(sizeof(struct thread));
	}
	push_back(cur_thread); 
	schedule();
}

// allocate stack and struct thread for new thread
// save the callee-saved registers and parameters on the stack
// set the return address to the target thread
// save the stack pointer in struct thread
// push the current thread to the end of the ready list
void create_thread(func_t func, void *param)
{
	unsigned *stack = malloc(4096);
	unsigned *base = stack;
	stack += 1024;
	struct thread *t = malloc(sizeof(struct thread));
	stack--;
	*stack = (unsigned)param;
	stack--;
	*stack = 0;
	stack--;
	*stack = (unsigned)func;
	stack--;
	*stack = 0;
	stack--;
	*stack = 0;
	stack--;
	*stack = 0;
	stack--;
	*stack = 0;
	t->esp = stack;
	t->ebp = base;
	push_back(t);
}

// call schedule1
void thread_yield()
{
	schedule1();
}

// call schedule
void thread_exit()
{
	if(junk!=NULL)
	{
		free(junk->ebp);
		free(junk);
		junk = NULL;
	}
	junk = cur_thread;
	schedule();
}

// call schedule1 until ready_list is null
void wait_for_all()
{
	while(ready_list!=NULL)
	{
		schedule1();
	}
	if(Lock!=NULL)
	{
		while(((struct thread *)Lock->wait_list)!=NULL);
	}
}

void sleep(struct lock *lock)
{
	if(Lock==NULL)
	{
		Lock = lock;
	}
	if (((struct thread *)lock->wait_list) == NULL)
	{
		lock->wait_list = cur_thread;
		((struct thread *)lock->wait_list)->next = ((struct thread *)lock->wait_list);
		((struct thread *)lock->wait_list)->prev = ((struct thread *)lock->wait_list);
	}
	else
	{
		cur_thread->next = ((struct thread *)lock->wait_list);
		cur_thread->prev = ((struct thread *)lock->wait_list)->prev;
		((struct thread *)lock->wait_list)->prev->next = cur_thread;
		((struct thread *)lock->wait_list)->prev = cur_thread;
	}
	schedule();
}

void wakeup(struct lock *lock)
{
	if(Lock!=NULL)
	{
		Lock = lock;
	}
	if (((struct thread *)lock->wait_list)!=NULL)
	{
		struct thread *ptr = ((struct thread *)lock->wait_list);
		if (((struct thread *)lock->wait_list) == NULL)
		{ ptr = NULL; }
		else if (((struct thread *)lock->wait_list)->next == ((struct thread *)lock->wait_list))
		{ lock->wait_list = NULL; }
		else
		{
			((struct thread *)lock->wait_list)->next->prev = ((struct thread *)lock->wait_list)->prev;
			((struct thread *)lock->wait_list)->prev->next = ((struct thread *)lock->wait_list)->next;
			lock->wait_list = ((struct thread *)lock->wait_list)->next;
		}
		ptr->next = NULL;
		ptr->prev = NULL;
		push_back(ptr);
	}
}

