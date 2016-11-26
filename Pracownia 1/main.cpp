//Krzysztof Lis, Piotr Gdowski
//Pracownia P1

#include <iostream>
#include <thread>
#include <semaphore.h>
#include <unistd.h>

#define N 2000
#define MEALS 2000
#define LEFT (phil_id - 1 + N) % N
#define RIGHT phil_id

// Kolejnosc filozofow (Pi) i widelcow (Wi): P0 W0 P1 W1 P2 W2 ... P{N-1} W{N-1} P0

using namespace std;

sem_t crit_sect;			// sekcja krytyczna
sem_t fork_in_use[N];		// semafory widelcow
int cur_owner[N];			// id filozofow posiadaczy

void start(int phil_id);
void eat();
void take_fork(int phil_id);
void put_fork(int phil_id);


int main()
{
	thread philosophers[N];
	sem_init(&crit_sect, 0, 1);

	for (int i = 0; i < N; i++)
	{
		cur_owner[i] = i % (N - 1);				// wreczenie widelca filozofowi o nizszym id
		sem_init(&fork_in_use[i], 0, 1);		// tworzenie semaforow

		philosophers[i] = thread(start, i);		// tworzenie watkow
	}

	for (int i = 0; i < N; i++)
	{
		philosophers[i].join();
	}

	cout << "DONE!" << endl;
	return 0;
}


void start(int phil_id)
{
	for (int i = MEALS; i > 0; i--)
	{
		if (cur_owner[LEFT] != phil_id && cur_owner[RIGHT] != phil_id) // jesli filozof nie ma 2 widelcow
		{
			take_fork(phil_id);
		}
		
		eat();
		put_fork(phil_id);

		sem_wait(&crit_sect);
		printf("%2d. filozof zjadl (%d/%d)\n", phil_id, MEALS - i + 1, MEALS);
		sem_post(&crit_sect);
	}
}

void eat()
{
	// symulacja jedzenia
	usleep(0);
}

void take_fork(int phil_id)
{
	sem_wait(&fork_in_use[LEFT]);		// czekam, az widelec bedzie "brudny" (sasiad skonczy jesc); potem zmieniam status widelca na "czysty"
	cur_owner[LEFT] = phil_id;			// sasiad przekazuje widelec

	sem_wait(&fork_in_use[RIGHT]);		// czekam, az widelec bedzie "brudny" (sasiad skonczy jesc); potem zmieniam status widelca na "czysty"
	cur_owner[RIGHT] = phil_id;			// sasiad przekazuje widelec
}

void put_fork(int phil_id) // tak naprawde nie odklada, tylko zmienia na brudne (ale trzyma dalej u siebie)
{
	sem_post(&fork_in_use[LEFT]);
	sem_post(&fork_in_use[RIGHT]);
}