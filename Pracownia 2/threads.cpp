#include <iostream>
#include <deque>
#include <iterator>
#include <map>
#include <ucontext.h>
#include "threads.h"

using namespace std;


struct Thread
{
    ucontext_t *cntxt;
    bool finished;
    unsigned int semID;
    unsigned int mutID;
};

struct Semaphore
{
    int semID;
    int semValue;
};

struct Mutex
{
    int mutID;
    Thread *blockingThread;
};

deque <Thread*> activeThreads;              // active threads
deque <Thread*> blockedThreads;             // blocked threads (caused by semaphores)
map<unsigned int, Semaphore*> semMap;       // semaphores
map<unsigned int, Mutex*> mutMap;           // mutexes

Thread *head;                               // main thread, added to q at the end
Thread *current;                            // pointer to currently running thread
bool initialized = false;     


//================my funcs====================================
int start(thread_startfunc_t func, void *arg)
{
    func(arg);                    // do my func
    current->finished = true;
    return thread_yield();        // go back and run another thread
}

Thread *new_thread(Thread *t, thread_startfunc_t func, void *arg) // return new Thread with all properties initialized (stack, context, etc.)
{
    t->finished = false;
    t->semID = 9999; // non-usable semID
    t->mutID = 9999; // non-usable mutID
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
    if (!initialized) // initializing and setting up activeThreads
    { 
        initialized = true; // only once
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

    activeThreads.push_back(current);           // when current thread didn't end work, push it to activeThreads
    Thread *tmp = current;                      // tmp for remembering current thread
    current = activeThreads.front();            // make first thread from activeThreads a current one
    activeThreads.pop_front();                  // remove that thread from queue
    swapcontext(tmp->cntxt, current->cntxt);    // swap context between old and new threads' context

    return 0;
}

extern int thread_seminit(unsigned int sem, unsigned int value)
{
    if (semMap.find(sem) == semMap.end()) // if semaphore isn't already initialized
    {
        Semaphore *currentSemaphore = new Semaphore;
        currentSemaphore->semID = sem;
        currentSemaphore->semValue = value;

        semMap.insert(make_pair(currentSemaphore->semID, currentSemaphore));

        return 0;
    }

    return -1;
}

extern int thread_semdown(unsigned int sem)
{
    if (semMap.find(sem) != semMap.end()) // if semaphore is already initialized
    {
        if (semMap.at(sem)->semValue > 0) // if semaphore is still open
        {
            (semMap.at(sem)->semValue)--;
        }
        else if (semMap.at(sem)->semValue == 0)         // if semaphore is closed
        {
            current->semID = sem;                       // assign semID to current thread
            blockedThreads.push_back(current);          // move the current thread to blockedThreads

            deque<Thread*>::iterator it = activeThreads.begin();
            while (it != activeThreads.end()) // iterate through activeThreads (move all activeThreads with the same semID to blockedThreads)
            {
                if ((*it)->semID == sem) // if thread from activeThreads has the same semID
                {
                    blockedThreads.push_back(*it);   // add that thread to blockedThreads
                    it = activeThreads.erase(it);    // remove that thread from activeThreads
                }
                else // if not then continue iterating
                {
                    ++it;
                }
            }

            Thread *tmp = current;                      // tmp for remembering current thread
            current = activeThreads.front();            // make first thread from activeThreads a current one
            activeThreads.pop_front();                  // remove that thread from queue
            swapcontext(tmp->cntxt, current->cntxt);    // swap context between old and new threads' context
        }

        return 0;
    }

    return -1;
}

extern int thread_semup(unsigned int sem)
{
    if (semMap.find(sem) != semMap.end()) // if semaphore is already initialized
    {
        if (semMap.at(sem)->semValue > 0) // if semaphore wasn't closed
        {
            (semMap.at(sem)->semValue)++;
        }
        else if (semMap.at(sem)->semValue == 0) // if semaphore was closed
        {
            bool found = false;

            deque<Thread*>::iterator it = blockedThreads.begin();
            while (it != blockedThreads.end()) // iterate through blockedThreads (move the FIRST FOUND thread from blockedThreads with the same semID to activeThreads)
            {
                if ((*it)->semID == sem) // if thread from blockedThreads has the same semID
                {
                    found = true;                     // thread found
                    (*it)->semID = 9999;              // assign a start-position semID

                    activeThreads.push_back(*it);     // add that thread to activeThreads
                    it = blockedThreads.erase(it);    // remove that thread from blockedThreads
                    break;                            // thread found, end of the loop
                }
                else // if not then continue iterating
                {
                    ++it;
                }  
            }
            
            if (!found) // if no thread with the same semID was found
            {
                (semMap.at(sem)->semValue)++;
            }
        }

        return 0;
    }

    return -1;
}

extern int thread_mutinit(unsigned int mut)
{
    if (mutMap.find(mut) == mutMap.end()) // if mutex isn't already initialized
    {
        Mutex *currentMutex = new Mutex;
        currentMutex->mutID = mut;
        currentMutex->blockingThread = NULL;

        mutMap.insert(make_pair(currentMutex->mutID, currentMutex));

        return 0;
    }

    return -1;
}

extern int thread_mutdown(unsigned int mut)
{
    if (mutMap.find(mut) != mutMap.end()) // if mutex is already initialized
    {
        if (mutMap.at(mut)->blockingThread == NULL) // if mutex isn't set
        {
            mutMap.at(mut)->blockingThread = current;
        }
        else // if mutex is set
        {
            current->mutID = mut;                         // assign mutID to current thread
            blockedThreads.push_back(current);            // move the current thread to blockedThreads

            deque<Thread*>::iterator it = activeThreads.begin();
            while (it != activeThreads.end()) // iterate through activeThreads (move all activeThreads with the same mutID to blockedThreads)
            {
                if ((*it)->mutID == mut) // if thread from activeThreads has the same mutID
                {
                    blockedThreads.push_back(*it);    // add that thread to blockedThreads
                    it = activeThreads.erase(it);     // remove that thread from activeThreads
                }
                else // if not then continue iterating
                {
                    ++it;
                }
            }

            Thread *tmp = current;                      // tmp for remembering current thread
            current = activeThreads.front();            // make first thread from activeThreads a current one
            activeThreads.pop_front();                  // remove that thread from queue
            swapcontext(tmp->cntxt, current->cntxt);    // swap context between old and new threads' context
        }

        return 0;
    }

    return -1;
}

extern int thread_mutup(unsigned int mut)
{
    if (mutMap.find(mut) != mutMap.end()) // if mutex is already initialized
    {
        deque<Thread*>::iterator it = blockedThreads.begin();
        while (it != blockedThreads.end()) // iterate through blockedThreads (move the FIRST FOUND thread from blockedThreads with the same mutID to activeThreads)
        {
            if ((*it)->mutID == mut) // if thread from blockedThreads has the same mutID
            {
                (*it)->mutID = 9999;             // assign a start-position mutID

                activeThreads.push_back(*it);    // add that thread to activeThreads
                it = blockedThreads.erase(it);   // remove that thread from blockedThreads
                break;                           // thread found, end of the loop
            }
            else // if not then continue iterating
            {
                ++it;
            }  
        }

        if (mutMap.at(mut)->blockingThread == current) // if the current thread is the one that blocked the mutex
        {
            mutMap.at(mut)->blockingThread = NULL;
        }
        else
        {
            // if thread that didn't set the mutex wants to reset it, then you probably doing something wrong...
        }

        return 0;
    }

    return -1;
}