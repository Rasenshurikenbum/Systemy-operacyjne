#include <iostream>
#include "threads.h"

using namespace std;

void print(void * arg)
{
    int k = *(int*)arg;
    //thread_mutinit(1);
    //thread_lock(1);
    thread_semdown(1);

    cout << "blokuje 1\n";

    for(int i=0;i<4;++i)
    {
        cout << 2*i+k << endl;
        thread_yield();//change context
    }
    //thread_unlock(1);
    thread_semup(1);

    cout << "koniec blokowania 1\n";
}

void print2(void* arg)
{
    int k = *(int*)arg;

    //thread_lock(1);
    //thread_seminit(1, 2);
    thread_semdown(1);

    cout << "blokuje 2\n";

    for(int i=0;i<4;++i)
    {
        cout << 5*i+k << endl;
        thread_yield();//change context
    }
    //thread_unlock(1);
    thread_semup(1);

    cout << "koniec blokowania 2\n";
}

int main()
{
    int x = 3;
    void* k = &x; // conversion int to void*
    thread_seminit(1, 1);
    
    thread_libinit(print, k);
    thread_create(print2, k);
    //thread_create(print, k);
    thread_yield();// start running
}