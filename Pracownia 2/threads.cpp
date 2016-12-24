#include <iostream>
#include <queue>
#include <ucontext.h>
#include "threads.h"
using namespace std;
typedef void (*thread_startfunc_t)();
#define STACK_SIZE 262144 /* size of each thread's stack */

struct Thread {
	ucontext_t * cntxt;
	bool finished;
	unsigned int semaphore;
};
queue <Thread*> Q;
ucontext_t* previous;
Thread * current;
//bool initializated = false;
//================my funcs====================================
int start(thread_startfunc_t func, void *arg)
	{
	func();
	return 0;
	}
/*
ucontext_t* make_thread(ucontext_t* A, thread_startfunc_t f, void *arg)
	{
		getcontext(A); // nowy kontekst
		char *stack=new char[STACK_SIZE]; 
		A->uc_stack.ss_sp=stack;
		A->uc_stack.ss_size=STACK_SIZE;
		A->uc_stack.ss_flags=0;
		A->uc_link=NULL;
		makecontext(A,f,0);
		Q.push(A);
		return A; 
	}
	*/	
//=================interface functions=======================
int thread_libinit(thread_startfunc_t func, void *arg)
	{
	Thread* first = new Thread;
	first->finished = false;
	first->cntxt = new ucontext_t;
	getcontext(first->cntxt);
	first->cntxt->uc_stack.ss_sp = new char[STACK_SIZE];
	first->cntxt->uc_stack.ss_size = STACK_SIZE;
	first->cntxt->uc_stack.ss_flags = 0;
	first->cntxt->uc_link=NULL;
	makecontext(first->cntxt, (void (*)()) start, 2, func, arg);
	Q.push(first);
	
	return 0;
	}
	/*
	if(initializated) return -1; // error, lib is initializated
	initializated = true;
	ucontext_t* thread = make_thread(new ucontext_t, func, arg);
	cntxt_cntxt = thread;
	cout << thread << "0\n";
	setcontext(thread); // call exactly once
	return thread_yield();
	}
int thread_create(thread_startfunc_t func, void *arg)
	{
	cout << "sfdhgdfh1\n";
	ucontext_t* thread = make_thread(new ucontext_t, func, arg);
	cout << thread << "sfdhgdfh1\n";
	return 0;
	}
int thread_yield()
	{
	cout << Q.size() << "\t" << current_cntxt << endl;
	Q.push(current_cntxt);
	ucontext_t* next = Q.front();
	swapcontext(current_cntxt,next);
	cout << Q.size() << "\t" << current_cntxt << endl;
	Q.pop();
	current_cntxt = next;
	return 0;
	}
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