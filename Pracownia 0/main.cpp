//Krzysztof Lis, pracownia P0

/*Napisz program w języku C lub C++ uruchamiający dwa lub więcej wątków potomnych.
Z każdego wątku potomnego wypisz dowolny (różny dla różnych wątków) napis na standardowe wyjście i zakończ wątek.
Z wątku-rodzica wypisz dowolny napis (różny od napisów wypisywanych w wątkach potomnych) po zakończeniu wykonania
wszystkich wątków potomnych i zakończ wątek. Wszystkie wypisywane napisy powinny być zakończone znakiem nowej linii
'\n' lub po wypisaniu każdego napisu należy wyczyścić strumień (cout.flush()).*/


#include <iostream>
#include <thread>

using namespace std;

void print1()
{
	cout << "1st\n";
}

void print2()
{
	cout << "2nd\n";
}

void print3()
{
	cout << "3rd\n";
}


int main()
{
	thread first(print1);
	thread second(print2);
	thread third(print3);

	first.join();
	second.join();
	third.join();

	cout << "DONE!\n";
	
	return 0;
}