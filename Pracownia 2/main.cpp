#include <iostream>
#include "threads.h"
using namespace std;
void print(void * arg)
{
    int k = *(int*)arg;
    //thread_semdown(1);
    thread_lock(1);
    cout << "locked 1\n";
    for(int i=0;i<4;++i)
    {
        cout << 2*i+k << endl;
        thread_yield();//change context
    }

    //thread_semup(1);
    thread_unlock(1);
    cout << "unlocked 1\n";
}
void print2(void* arg)
{
    int k = *(int*)arg;
    //thread_semdown(1);
    //thread_semdown(2);
    thread_lock(1);
    thread_lock(2);
    cout << "locked 2\n";
    for(int i=0;i<4;++i)
    {
        cout << 5*i+k << endl;
        thread_yield();//change context
    }
    //thread_semup(1);
    //thread_semup(2);
    thread_unlock(1);
    thread_unlock(2);
    cout << "unlocked 2\n";
}
void print3(void* arg)
{
    int k = *(int*)arg;
    //thread_semdown(1);
   // thread_semdown(2);
   thread_lock(2);
    cout << "locked 3\n";
    for(int i=0;i<4;++i)
    {
        cout << 6*i-k << endl;
        thread_yield();//change context
    }
    //thread_semup(1);
    //thread_semup(2);
    thread_unlock(2);
    cout << "unlocked 3\n";
}
int main()
{
    int x = 3;
    void* k = &x; // conversion int to void*
    //thread_seminit(1, 2);
    //thread_seminit(2, 1);
    
    thread_libinit(print, k);
    thread_create(print, k);
    thread_create(print2, k);
    thread_create(print3, k);
    thread_create(print2, k);
    thread_yield();// start running
}