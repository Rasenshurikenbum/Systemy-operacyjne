//Krzysztof Lis, Piotr Gdowski
//Pracownia P1
#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <vector>
#define DIRTY 1
#define CLEAN 2
#define N 20
using namespace std;
vector <sem_t> forks;
vector <int> forks_state;
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
	thread filozof[N]; 
	forks.resize(N);
	forks_state.resize(N,DIRTY); // wszystkie widelce brudne na poczatku
	for(int i=0;i<N;++i)
	{
	filozof[i] = thread(nix, i);
	sem_init(&forks[i],0,1);

	}
	for(int i=0;i<N;++i)
	{
	filozof[i].join();	
	}
	cout << "\nEND\n";
	return 0;
}
