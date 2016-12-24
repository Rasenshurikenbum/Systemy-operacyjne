#include <iostream>
#include <queue>
#include "threads.h"
#include <ucontext.h>
using namespace std;
typedef void (*thread_startfunc_t)();
#define STACK_SIZE 262144 /* size of each thread's stack */
queue <ucontext_t *> Q;
ucontext_t* current_cntxt;
bool initializated = false;
//================my funcs====================================
ucontext_t* make_thread(ucontext_t* A, thread_startfunc_t f, void *arg)
	{
		getcontext(A); // nowy kontekst
		char *stack=new char[STACK_SIZE]; 
		A->uc_stack.ss_sp=stack;
		A->uc_stack.ss_size=STACK_SIZE;
		A->uc_stack.ss_flags=0;
		makecontext(A,f,0);
		return A; 
	}
//==============interface functions=======================
int thread_libinit(thread_startfunc_t func, void *arg)
	{
	if(initializated) return -1; // error, lib is initializated
	initializated = true;
	ucontext_t* thread = make_thread(new ucontext_t, func, arg);
	current_cntxt = thread;
	setcontext(thread); // call exactly once
	return 0;
	}
int thread_create(thread_startfunc_t func, void *arg)
	{
	ucontext_t* thread = make_thread(new ucontext_t, func, arg);
	cout << thread << "\n";
	return 0;
	}
int thread_yield()
	{
	cout << Q.size() << "\t" << current_cntxt << endl;
	Q.push(current_cntxt);
	ucontext_t* next = Q.front();
	current_cntxt = next;
	Q.pop();
	swapcontext(current_cntxt,next);
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

