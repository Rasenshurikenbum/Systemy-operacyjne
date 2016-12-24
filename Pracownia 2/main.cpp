#include <iostream>
#include "threads.h"
using namespace std;
void print()
{
    for(int i=0;i<4;++i)
    {
        cout << 2*i << endl;
        thread_yield();
    }
}
void print2()
{
    for(int i=0;i<4;++i)
    {
        cout << 5*i << endl;
        thread_yield();
    }
}
int main()
{
    thread_libinit(print, NULL);
    thread_create(print2, NULL);
cout << "dsmfmfhkmgfkhmgfh\n";
	
}
