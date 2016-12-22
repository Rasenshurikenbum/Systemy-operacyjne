#include <iostream>
#include <queue>
#include "threads.h"
#include <ucontext.h>
using namespace std;
typedef void (*thread_startfunc_t) (void *);
#define STACK_SIZE 262144	/* size of each thread's stack */
struct props 
	{
		queue <ucontext_t *> Q;
		bool initializated = false;
	} sth;
ucontext_t* make_thread(thread_startfunc_t f, void *arg)
	{
		ucontext_t *A;
		getcontext(A);
		char *stack=new char[STACK_SIZE];
		A->uc_stack.ss_sp=stack;
		A->uc_stack.ss_size=STACK_SIZE;
		A->uc_stack.ss_flags=0;
		makecontext(A,f,0); // na razie 0, trzeba zrobic tak, by sie dalo przekazac wiecej argumentow
		return A;
	}
//==============interface functions=======================
int thread_libinit(thread_startfunc_t func, void *arg)
	{
	if(sth.initializated) return -1; // error, lib is initializated
	sth.initializated = true;
	ucontext_t* thread = make_thread(func, arg);
	sth.Q.push(thread);
	return 0; 
	}
int thread_create(thread_startfunc_t func, void *arg)
	{
return 0;
	
	}
int thread_yield(void)
	{
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

