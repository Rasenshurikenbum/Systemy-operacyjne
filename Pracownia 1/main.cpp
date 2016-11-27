//Krzysztof Lis, Piotr Gdowski
//Pracownia P1

#include <iostream>
#include <thread>
#include <semaphore.h>
#include <unistd.h>

#define N 20
#define MEALS 10
#define LEFT (phil_id - 1 + N) % N
#define RIGHT phil_id
#define DIRTY 0
#define CLEAN 1

// Kolejnosc filozofow (Pi) i widelcow (Wi): P0 W0 P1 W1 P2 W2 ... P{N-1} W{N-1} P0

using namespace std;

sem_t crit_sect;			// sekcja krytyczna
sem_t fork_in_use[N];		// semafory widelcow
int fork_owner[N];			// nr filozofa posiadającego ity widelec
int fork_state[N];
int meals=0;
void start(int);
void eat(int,int);
void take_fork(int);
void put_fork(int);

int main()
{
	thread philosophers[N];
	sem_init(&crit_sect, 0, 1);
	for (int i = 0; i < N; i++)
	{
		fork_owner[i] = i % (N - 1);				// wreczenie widelca filozofowi o nizszym id
		sem_init(&fork_in_use[i], 0, 1);		// tworzenie semaforow
		philosophers[i] = thread(start, i);		// tworzenie watkow
		fork_state[i] = DIRTY; // wszystkie widelce brudne
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
		/*if(i != MEALS && fork_state[LEFT]+fork_state[RIGHT] == 2*DIRTY 
			&& fork_owner[LEFT] == phil_id && fork_owner[RIGHT] == phil_id) 
				{
				cout << "NIKT NIE CHCE MOICH WIDELCÓW!" << endl;
				put_fork(phil_id);
				}*/
		// jak ma dwa brudne, to musi je oddac
		take_fork(phil_id); // bierzemy widelce
		if(fork_state[LEFT]+fork_state[RIGHT] == 2*CLEAN) // jemy, ale tylko czystymi
			{
			eat(phil_id,i); 
			fork_state[LEFT]=fork_state[RIGHT] = DIRTY;
			put_fork(phil_id); // oddajemy widelce
			}
		else 
			{
			//char c;
			i++; // kolejna kolejka.
			//if(i<MEALS) exit(0);
			//cout<< i << endl;
			//cout << i << " " << phil_id << ": NIKT NIE CHCE MOICH WIDELCÓW!" << endl;
			//cin>> c;
			}
	}
}

void eat(int id, int i)
{
	// symulacja jedzenia
	sem_wait(&crit_sect); // komunikat
	meals++;
	printf("%2d. filozof zjadl (%d/%d) RAZEM (%d/%d)\n", id, MEALS - i + 1, MEALS, meals,MEALS*N);
	sem_post(&crit_sect);
	usleep(1);
}

void take_fork(int phil_id)
{
	if (fork_owner[LEFT] != phil_id) // jesli filozof nie ma lewego widelca
	{
		sem_wait(&fork_in_use[LEFT]);		// czekam, az widelec bedzie "brudny" (sasiad skonczy jesc); potem zmieniam status widelca na "czysty"
		fork_owner[LEFT] = phil_id;			// sasiad przekazuje widelec
	//	sem_wait(&crit_sect);
		fork_state[LEFT] = CLEAN;
	//	sem_post(&crit_sect);
		
	}
	else if (fork_owner[RIGHT] != phil_id) // jesli filozof nie ma prawego widelca
	{
		sem_wait(&fork_in_use[RIGHT]);		// czekam, az widelec bedzie "brudny" (sasiad skonczy jesc); potem zmieniam status widelca na "czysty"
		fork_owner[RIGHT] = phil_id;
	//	sem_wait(&crit_sect);
		fork_state[RIGHT] = CLEAN;
	//	sem_post(&crit_sect);
				// sasiad przekazuje widelec
	}
}

void put_fork(int phil_id) // tak naprawde nie odklada, tylko zmienia na brudne (ale trzyma dalej u siebie)
{
	sem_post(&fork_in_use[LEFT]);
	sem_post(&fork_in_use[RIGHT]);
}
