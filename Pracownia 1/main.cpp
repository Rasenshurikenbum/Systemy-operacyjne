//Krzysztof Lis, Piotr Gdowski
//Pracownia P1
#include <iostream>
#include <unistd.h>
#include <thread>
#define N 20
using namespace std;
void nix(int n)
	{
	cout << n << endl;
	}
int main()
{
	thread filozof[N]; // 20 watkow
	for(int i=0;i<N;++i)
	{
	filozof[i] = thread(nix, i);
	}
	for(int i=0;i<N;++i)
	{
	filozof[i].join();	
	}
	cout << "\nEND\n";
	return 0;
}
