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
queue <Thread*> Q;
Thread * previous;
Thread * current;
bool initialized = false;
//================my funcs====================================
int start(thread_startfunc_t func, void *arg)
	{
	func(arg);
	current -> finished = true;
	cout << "end" << endl;
	return 0;
	}
Thread* new_thread(Thread* t, thread_startfunc_t func, void *arg)
	{
	t->finished = false;
	t->cntxt = new ucontext_t;
	getcontext(t->cntxt);
	t->cntxt->uc_stack.ss_sp = new char[STACK_SIZE];
	t->cntxt->uc_stack.ss_size = STACK_SIZE;
	t->cntxt->uc_stack.ss_flags = 0;
	t->cntxt->uc_link=NULL;
	makecontext(t->cntxt, (void (*)()) start, 2, func, arg);
	Q.push(t);
	return t;
	}
int thread_yield(Thread* now)
	{
	// cout << current->cntxt << endl;
	if(now->finished && Q.empty())
		{
		cout << "fin"; 
		return 0;
		}
	if(now->finished)
		{
		current = Q.front();
		Q.pop();
		setcontext(current->cntxt);
		exit(0);
		}
	if(Q.empty()) return 0; // single thread running
	Q.push(now);
	Thread* next = Q.front();
	Q.pop();
	//cout << "kupka" << endl;
	swapcontext(now->cntxt, next->cntxt);
	//cout << "kupka" << endl;
	//current = next;
	//cout << Q.size();
	return 0;
	
	}
//=================interface functions=======================
int thread_libinit(thread_startfunc_t func, void *arg)
	{
	Thread* first = new_thread(new Thread, func, arg);
	current = first;
	previous = NULL;
	cout << "master " << first->cntxt << endl;
	return 0;
	}
int thread_create(thread_startfunc_t func, void *arg)
	{
	Thread* next = new_thread(new Thread, func, arg);
	cout << "slave " << next->cntxt << endl;
	return 0;
	}

int thread_yield()
	{
	if(!initialized) // pierwsze uruchomienie całej maszyny
		{
		initialized=true; 
		setcontext(current->cntxt);
		}
	return thread_yield(current);
	}/*
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