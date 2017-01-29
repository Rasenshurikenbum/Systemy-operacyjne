#include <iostream>
#include <deque>
#include <iterator>
#include <map>
#include <ucontext.h>
#include "threads.h"

using namespace std;


struct Thread
{
	ucontext_t * cntxt;
	bool finished;
	unsigned int semID;
};

struct Semaphore
{
    int semID;
    int semValue;
};

deque <Thread*> activeThreads;				// active threads
deque <Thread*> blockedThreads;				// blocked threads (caused by semaphores)
map<unsigned int, Semaphore*> semMap;		// semaphores

Thread *head;								// main thread, added to q at the end
Thread *current;							// pointer to currently running thread
bool initialized = false; 	


//================my funcs====================================
int start(thread_startfunc_t func, void *arg)
{
	func(arg);					// do my func
	current->finished = true;
	return thread_yield();		// go back and run another thread
}

Thread *new_thread(Thread *t, thread_startfunc_t func, void *arg) // return new Thread with initialized all properties (stack, context, etc)
{
	t->finished = false;
	t->cntxt = new ucontext_t;
	getcontext(t->cntxt);
	t->cntxt->uc_stack.ss_sp = new char[STACK_SIZE];
	t->cntxt->uc_stack.ss_size = STACK_SIZE;
	t->cntxt->uc_stack.ss_flags = 0;
	t->cntxt->uc_link = NULL;
	makecontext(t->cntxt, (void (*)()) start, 2, func, arg); // start is our running function
	return t;
}

//=================interface functions=======================
extern int thread_libinit(thread_startfunc_t func, void *arg)
{
	head = new_thread(new Thread, func, arg); // main thread
	return 0;
}

extern int thread_create(thread_startfunc_t func, void *arg)
{
	Thread *next = new_thread(new Thread, func, arg);
	activeThreads.push_back(next);
	return 0;
}

extern int thread_yield()
{
	if (!initialized) // initializing and setting up q of threads
	{ 
		initialized=true; // only once
		activeThreads.push_back(head);
		current = activeThreads.front();
		activeThreads.pop_front();
		setcontext(current->cntxt);
		return 0; // well, it shouldn't go further
	}

	if (current->finished && activeThreads.empty()) // after all threads we get here
	{
		cout << "fin" << endl;
		exit(0);
		return 0;
	}
	else if (current->finished) // just one of our threads ended working
	{
		current = activeThreads.front(); 
		activeThreads.pop_front(); // take next, set him to run
		setcontext(current->cntxt);
		return 0;
	}

	activeThreads.push_back(current);							// when current thread didn't end work, push it to q and call previous
	Thread *tmp = current;						// tmp for remembering old thread
	current = activeThreads.front();
	activeThreads.pop_front();
	swapcontext(tmp->cntxt, current->cntxt);	//saving current context
	return 0;
}

extern int thread_seminit(unsigned int sem, unsigned int value)
{
    if (semMap.find(sem) != semMap.end()) // if semaphore isn't already initialized
    {
        Semaphore *currentSemaphore;
        currentSemaphore->semID = sem;
    	currentSemaphore->semValue = value;

		semMap.insert(make_pair(currentSemaphore->semID, currentSemaphore));

		return 0;
    }

    return -1;
}

extern int thread_semup(unsigned int sem)
{
	if (semMap.find(sem) != semMap.end()) // if semaphore isn't already initialized
    {
    	if ((semMap.at(sem)->semValue) == 0)
    	{
        	int count = 0;
	        //iterate thorugh blockedThreads, and transfer all threads with semID=sem to activeThreads

	        deque<Thread*>::iterator it = blockedThreads.begin();
	        while (it != blockedThreads.end())
	        {
            	if (((*it)->semID) == sem)
            	{
	                count++;
	                (*it)->semID = 9917; //Random Number
	                activeThreads.push_back(*it);
	                it = blockedThreads.erase(it);
                	break;
            	}
            	else
            	{
                	++it;
				}  
        	}
        	
        	if (count==0)
        	{
            	(semMap.at(sem)->semValue)++;
        	}
    	}    
    	else
    	{
        	(semMap.at(sem)->semValue)++;
		}

		return 0;
    }

    return -1;
}

extern int dthreads_semdown(unsigned int sem)
{
	if (semMap.find(sem) != semMap.end()) // if semaphore isn't already initialized
    {
		if ((semMap.at(sem)->semValue) >= 0)
		{
        	if (semMap.at(sem)->semValue == 0)
        	{
            	deque<Thread*>::iterator it = activeThreads.begin();

            	while (it != activeThreads.end())
            	{
                	if ((*it)->semID == sem)
                	{
                    	(*it)->semID = sem;
                    	blockedThreads.push_back(*it);
                    	it = activeThreads.erase(it); 
                	}
                	else
                	{
                    	++it;
                	}
            	}

	            current->semID = sem;
	            blockedThreads.push_back(current);
        	}
        	else
        	{
            	(semMap.at(sem)->semValue)--;
			}

    		return 0;
    	}
	}

    return -1;
}