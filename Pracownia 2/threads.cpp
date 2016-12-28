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
Thread * head; // główny wątek, czeka na wszystkie inne
Thread * current;
Thread * previous;
bool initialized = false;
//================my funcs====================================
void yield(void* k)
	{
	current = previous;
	}
int start(thread_startfunc_t func, void *arg)
	{
	func(arg);
	current->finished = true;
	//cout << "ended: " << current->cntxt << endl;
	return thread_yield();
	}
int thread_yield(Thread* now) // now to wskaźnik na obecny wątek
	{
	//int a;
	//cin>> a;
	//cout << current << "\t" << now << endl;
	//cout << current->cntxt  << " " << Q.front()->cntxt << " " << current->finished<< " " << Q.size() << endl;
	if(now == previous && !Q.empty())
		{
		Thread* next = Q.front();  // next wskazuje na nastepny watek
		Q.pop();
		current = next;
		setcontext(current->cntxt);
		return 0;
		}
	if(now->finished && Q.empty())
		{
		cout << "fin" << endl;
		exit(0);
		return 0;
		}
	else if(now->finished) // jesli obecny skonczyl dzialac
		{
		//cout << "has been: "<< current->cntxt << endl;
		current = Q.front(); // niech current wskazuje na nastepny watek
		Q.pop();
		//cout << "gonna be: "<< current->cntxt << endl;
		setcontext(current->cntxt);
		return 0;
		}
	Q.push(now);
	//Thread* next = Q.front();  // next wskazuje na nastepny watek
	//Q.pop();
	//getcontext(current->cntxt);
	//current = next;
	//setcontext(current->cntxt);
	swapcontext(current->cntxt, previous->cntxt);
	//cout << "current: "<< current->cntxt << endl;
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
	return t;
	}
//=================interface functions=======================
int thread_libinit(thread_startfunc_t func, void *arg)
	{
	Thread* first = new_thread(new Thread, func, arg);
	previous = new_thread(new Thread, yield, (void*)(new int));
	head = first;
	cout << "master " << first->cntxt << endl;
	return 0;
	}
int thread_create(thread_startfunc_t func, void *arg)
	{
	Thread* next = new_thread(new Thread, func, arg);
	cout << "slave " << next->cntxt << endl;
	Q.push(next);
	return 0;
	}
int thread_yield()
	{
	if(!initialized) // pierwsze uruchomienie całej maszyny
		{
		initialized=true; 
		Q.push(head);
		current = Q.front();
		Q.pop();
		setcontext(current->cntxt);
		}
	return thread_yield(current);
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