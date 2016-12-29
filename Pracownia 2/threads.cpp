#include <iostream>
#include <queue>
#include <ucontext.h>
#include "threads.h"
using namespace std;
struct Thread {
	ucontext_t * cntxt;
	bool finished;
	unsigned int semaphore;
};
queue <Thread*> Q; // q with all threads
Thread * head; // main thread, added to q at the end
Thread * current; // pointer to currently running thread
Thread * previous; // 'ghost' thread, helps keeping clean while swithcing context
bool initialized = false; 
//================my funcs====================================
void yield(void* k)
	{
	current = previous;
	}
int start(thread_startfunc_t func, void *arg)
	{
	func(arg); // do my func
	current->finished = true;
	return thread_yield(); // go back and run another threaf
	}
Thread* new_thread(Thread* t, thread_startfunc_t func, void *arg) // return new Thread with initialized all properties (stack, context etc)
	{
	t->finished = false;
	t->cntxt = new ucontext_t;
	getcontext(t->cntxt);
	t->cntxt->uc_stack.ss_sp = new char[STACK_SIZE];
	t->cntxt->uc_stack.ss_size = STACK_SIZE;
	t->cntxt->uc_stack.ss_flags = 0;
	t->cntxt->uc_link=NULL;
	makecontext(t->cntxt, (void (*)()) start, 2, func, arg); // start is our running function
	return t;
	}
//=================interface functions=======================
int thread_libinit(thread_startfunc_t func, void *arg)
	{
	head = new_thread(new Thread, func, arg); // main thread
	previous = new_thread(new Thread, yield, (void*)(new int));// yield() just call thread_yield(), argument is never used
	return 0;
	}
int thread_create(thread_startfunc_t func, void *arg)
	{
	Thread* next = new_thread(new Thread, func, arg);
	Q.push(next);
	return 0;
	}
int thread_yield()
	{
	if(!initialized) 
		{ // initializing and setting up q of threads
		initialized=true; // only once
		Q.push(head);
		current = Q.front();
		Q.pop();
		setcontext(current->cntxt);
		return 0;// well, it shouldn't go further
		}
	if(current == previous && !Q.empty()) 
		{// if 'previous' was running, we need to push next thread from q
		Thread* next = Q.front();  
		Q.pop();
		current = next;
		setcontext(current->cntxt);// without saving 'previous' context
		return 0;
		}
	if(current->finished && Q.empty()) // after all threads we get here
		{
		cout << "fin" << endl;
		exit(0);
		return 0;
		}
	else if(current->finished) // just one of our threads ended working
		{
		current = Q.front(); 
		Q.pop(); // take next, set him to run
		setcontext(current->cntxt);
		return 0;
		}
	Q.push(current); // when current thread didn't end work, push it to q and call previous
	swapcontext(current->cntxt, previous->cntxt);//saving current context
	return 0;
	}
/*
int thread_lock(unsigned int lock)
	{
return 0;
	
	}
int thread_unlock(unsigned int lock)
	{
return 0;
	
	}
int thread_wait(unsigned int lock, unsigned int cond)
	{
return 0;
	
	}
int thread_signal(unsigned int lock, unsigned int cond)
	{
return 0;
	
	}
int thread_broadcast(unsigned int lock, unsigned int cond)
	{
return 0;
	
	}
void start_preemptions(bool async, bool sync, int random_seed)
	{

	}
*/