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

deque <Thread*> activeQ;              // active threads
deque <Thread*> blockedQ;             // blocked threads (caused by semaphores)
map<int, Semaphore*> semMap;       // semaphores
map<int, Mutex*> mutMap;           // mutexes

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
 int thread_libinit(thread_startfunc_t func, void *arg)
{
    head = new_thread(new Thread, func, arg); // main thread
    return 0;
}

 int thread_create(thread_startfunc_t func, void *arg)
{
    Thread *next = new_thread(new Thread, func, arg);
    activeQ.push_back(next);
    return 0;
}

 int thread_yield()
{
    if (!initialized) // initializing and setting up activeQ
    { 
        initialized = true; // only once
        activeQ.push_back(head);
        current = activeQ.front();
        activeQ.pop_front();
        setcontext(current->cntxt);
        return 0; // well, it shouldn't go further
    }

    if (current->finished && activeQ.empty()) // after all threads we get here
    {
        cout << "fin" << endl;
        exit(0);
        return 0;
    }
    else if (current->finished) // just one of our threads ended working
    {
        current = activeQ.front(); 
        activeQ.pop_front(); // take next, set him to run
        setcontext(current->cntxt);
        return 0;
    }

    activeQ.push_back(current);           // when current thread didn't end work, push it to activeQ
    Thread *tmp = current;                      // tmp for remembering current thread
    current = activeQ.front();            // make first thread from activeQ a current one
    activeQ.pop_front();                  // remove that thread from queue
    swapcontext(tmp->cntxt, current->cntxt);    // swap context between old and new threads' context

    return 0;
}

 int thread_seminit(unsigned int sem, unsigned int value)
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

 int thread_semdown(unsigned int sem)
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
            blockedQ.push_back(current);          // move the current thread to blockedQ

            deque<Thread*>::iterator it = activeQ.begin();
            while (it != activeQ.end()) // iterate through activeQ (move all activeQ with the same semID to blockedQ)
            {
                if ((*it)->semID == sem) // if thread from activeQ has the same semID
                {
                    blockedQ.push_back(*it);   // add that thread to blockedQ
                    it = activeQ.erase(it);    // remove that thread from activeQ
                }
                else // if not then continue iterating
                {
                    ++it;
                }
            }

            Thread *tmp = current;                      // tmp for remembering current thread
            current = activeQ.front();            // make first thread from activeQ a current one
            activeQ.pop_front();                  // remove that thread from queue
            swapcontext(tmp->cntxt, current->cntxt);    // swap context between old and new threads' context
        }

        return 0;
    }

    return -1;
}

int thread_semup(unsigned int sem)
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
            deque<Thread*>::iterator it = blockedQ.begin();
            while (it != blockedQ.end()) // iterate through blockedQ (move the FIRST FOUND thread from blockedQ with the same semID to activeQ)
            {
                if ((*it)->semID == sem) // if thread from blockedQ has the same semID
                {
                    found = true;                     // thread found
                    (*it)->semID = 9999;              // assign a start-position semID

                    activeQ.push_back(*it);     // add that thread to activeQ
                    it = blockedQ.erase(it);    // remove that thread from blockedQ
                    break;                            // thread found, end of the loop
                }
                ++it; // if not then continue iterating 
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

 int thread_lock(unsigned int mut)
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
            blockedQ.push_back(current);            // move the current thread to blockedQ

            deque<Thread*>::iterator it = activeQ.begin();
            while (it != activeQ.end()) // iterate through activeQ (move all activeQ with the same mutID to blockedQ)
            {
                if ((*it)->mutID == mut) // if thread from activeQ has the same mutID
                {
                    blockedQ.push_back(*it);    // add that thread to blockedQ
                    it = activeQ.erase(it);     // remove that thread from activeQ
                }
                ++it; // if not then continue iterating
            }

            Thread *tmp = current;                      // tmp for remembering current thread
            current = activeQ.front();            // make first thread from activeQ a current one
            activeQ.pop_front();                  // remove that thread from queue
            swapcontext(tmp->cntxt, current->cntxt);    // swap context between old and new threads' context
        }

        return 0;
    }

    return -1;
}

 int thread_unlock(unsigned int mut)
{
    if (mutMap.find(mut) != mutMap.end()) // if mutex is already initialized
    {
        deque<Thread*>::iterator it = blockedQ.begin();
        while (it != blockedQ.end()) // iterate through blockedQ (move the FIRST FOUND thread from blockedQ with the same mutID to activeQ)
        {
            if ((*it)->mutID == mut) // if thread from blockedQ has the same mutID
            {
                (*it)->mutID = 9999;             // assign a start-position mutID
                 activeQ.push_back(*it);    // add that thread to activeQ
                it = blockedQ.erase(it);   // remove that thread from blockedQ
                break;                           // thread found, end of the loop
            }
            ++it; // if not then continue iterating 
        }
        if (mutMap.at(mut)->blockingThread == current) // if the current thread is the one that blocked the mutex
        {
            mutMap.at(mut)->blockingThread = NULL;
        }
        else; // if thread that didn't set the mutex wants to reset it, then you probably doing something wrong...
        return 0;
    }
    return -1;
}