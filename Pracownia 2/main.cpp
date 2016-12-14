#include <iostream>
#include "threads.cpp"
#include <functional>
using namespace std;
void print(int x) {cout << x; }

int main()
{
	thread x = thread<void,int>(print, 4);
}
