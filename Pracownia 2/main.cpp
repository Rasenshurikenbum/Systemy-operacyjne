#include <iostream>
#include "threads.h"
using namespace std;
void print()
{
    cout << 3;
    //thread_yield();
    cout << 5;
}
void print2()
    {
    cout << 4;
    thread_yield();
    cout << 6;
    }
int main()
{
    thread_libinit(print,NULL);
    //thread_create(print2, NULL);
}
