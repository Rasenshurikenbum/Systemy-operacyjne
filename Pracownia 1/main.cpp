//Krzysztof Lis, Piotr Gdowski
//Pracownia P1
#include <iostream>
#include <unistd.h>
#include <thread>
#define N 20
using namespace std;
/*
jak ja bym to widział:
klasa widelec 
	- brudny/czysty
	- mutex
	- id filozofa trzymajacego go

watki - filozofowie
	- operacje: poproś, myśl, jedz, wyczysc

potem dorzucimy serwetki podobnie.


*/
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
