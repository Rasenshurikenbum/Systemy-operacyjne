//Krzysztof Lis, Piotr Gdowski
//Pracownia P1

#include <iostream>
#include <thread>
#include <semaphore.h>
#include <unistd.h>

#define N 20						// liczba filozofow
#define MEALS 100						// liczba posilkow do zjedzenia
#define BOXES 5						// liczba pudelek z serwetkami
#define LEFT (phil_id - 1 + N) % N
#define RIGHT phil_id
#define DIRTY 0
#define CLEAN 1

// Kolejnosc filozofow (Pi) i widelcow (Wi): P0 W0 P1 W1 P2 W2 ... P{N-1} W{N-1} P0

using namespace std;

sem_t crit_sect;			// sekcja krytyczna
sem_t fork_in_use[N];		// semafory widelcow
sem_t napkin[BOXES]; 		// 5 semaforow na serwetki

int fork_owner[N];			// nr filozofa posiadającego i-ty widelec
int fork_state[N];			// stan widelca (<brudny> | <czysty>)
//bool owns_nap[N];
int meals_to_go[N];			// ilosc posilkow do zjedzenia i-tego filozofa

void start(int);
void eat(int);
void take_fork(int);
void take_napkin(int);
void put_all(int);
int meals();

int main()
{
	thread philosophers[N];
	sem_init(&crit_sect, 0, 1);

	for (int i = 0; i < BOXES; i++)
	{
		sem_init(&napkin[i], 0, 2); // dwie serwetki dostepne w kazdym pudelku
	}

	for (int i = 0; i < N; i++)
	{
		fork_owner[i] = i % (N - 1);			// wreczenie widelca filozofowi o nizszym id
		sem_init(&fork_in_use[i], 0, 1);		// tworzenie semaforow widelcow
		fork_state[i] = DIRTY;					// wszystkie widelce brudne
		meals_to_go[i] = MEALS;					// przypisanie ilosci posilkow do zjedzenia

		//owns_nap[i] = false;					// nikt nie ma serwetki
	}
	for(int i=0;i<N;++i) philosophers[i] = thread(start, i);		// tworzenie watkow
	for (int i = 0; i < N; i++)
	{
		philosophers[i].join();
	}

	cout << "DONE!" << endl;
	return 0;
}

void start(int phil_id)
{
	while (meals_to_go[phil_id] != 0)
	{
		take_fork(phil_id); // bierzemy widelce

		if (fork_state[LEFT] == CLEAN && fork_state[RIGHT] == CLEAN && fork_owner[LEFT] == phil_id && fork_owner[RIGHT] == phil_id) // kiedy filozof ma 2 czyste widelce
		{
			take_napkin(phil_id);		// biore serwetke
			eat(phil_id);				// jem
			put_all(phil_id);			// oddajemy widelce i serwetki
		}
	}
}

void take_fork(int phil_id)
{
	if (fork_owner[LEFT] != phil_id) // jesli filozof nie ma lewego widelca
	{
		sem_wait(&fork_in_use[LEFT]);		// czekam, az widelec bedzie "brudny" (sasiad skonczy jesc); po czekaniu ustawiam czekanie na nastepnego
		fork_owner[LEFT] = phil_id;			// sasiad przekazuje widelec
		fork_state[LEFT] = CLEAN;			// zmieniam status widelca na "czysty"
	}
	
	if (fork_owner[RIGHT] != phil_id) // jesli filozof nie ma prawego widelca
	{
		sem_wait(&fork_in_use[RIGHT]);		// czekam, az widelec bedzie "brudny" (sasiad skonczy jesc); po czekaniu ustawiam czekanie na nastepnego
		fork_owner[RIGHT] = phil_id;		// sasiad przekazuje widelec
		fork_state[RIGHT] = CLEAN;			// zmieniam status widelca na "czysty"
	}
}

void eat(int phil_id)
{
	// symulacja jedzenia
	usleep(5);

	sem_wait(&crit_sect);	// komunikat

	meals_to_go[phil_id]--;	// zjedzony posilek
	printf("%2d. filozof zjadl (%d/%d), razem (%d/%d)\n", phil_id, MEALS - meals_to_go[phil_id], MEALS, meals(), N*MEALS);

	sem_post(&crit_sect);	// koniec komunikatu
}

void take_napkin(int phil_id)
{
	sem_wait(&napkin[phil_id / 4]);		// czekam, az serwetka bedzie wolna; po czekaniu ja zabieram
	//owns_nap[phil_id] = true;			// filozof id ma serwetkę	
}

void put_all(int phil_id) // tak naprawde nie odklada, tylko zmienia na brudne (ale trzyma dalej u siebie)
{
	fork_state[LEFT] = DIRTY;			// lewy widelec jest teraz "brudny"
	fork_state[RIGHT] = DIRTY;			// prawy widelec jest teraz "brudny"

	sem_post(&fork_in_use[LEFT]);		// odblokowujemy lewy widelec sasiadowi
	sem_post(&fork_in_use[RIGHT]);		// odblokowujemy prawy widelec sasiadowi
	sem_post(&napkin[phil_id / 4]);		// odblokowujemy serwetke

	//owns_nap[phil_id]=false; // filozof id nie ma serwetki
}
int meals()
	{
	int ile = N*MEALS;
	for(int i=0;i<N;++i) ile-=meals_to_go[i];
	return ile;
	}
