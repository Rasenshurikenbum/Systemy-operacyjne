#include <iostream>
#include "threads.h"
using namespace std;
void print(void * arg)
{
    int k = *(int*)arg;
    for(int i=0;i<4;++i)
    {
        cout << 2*i+k << endl;
        thread_yield();
    }
}
void print2(void* arg)
{
    int k = *(int*)arg;
    for(int i=0;i<4;++i)
    {
        cout << 5*i+k << endl;
        thread_yield();
    }
}
int main()
{
    int x = 3;
    void* k = &x;
    thread_libinit(print, k);
    //thread_create(print2, k);
    //thread_create(print, k);
    //thread_create(print2, k);
    thread_yield();
}