//Krzysztof Lis, Piotr Gdowski
//Pracownia P1

#include <iostream>
#include <thread>
#include <semaphore.h>
#include <unistd.h>

#define N 20						// liczba filozofow
#define MEALS 100					// liczba posilkow do zjedzenia
#define BOXES 5						// liczba pudelek z serwetkami
#define LEFT (phil_id - 1 + N) % N
#define RIGHT phil_id
#define DIRTY 0
#define CLEAN 1

// Kolejnosc filozofow (Pi) i widelcow (Wi): P0 W0 P1 W1 P2 W2 ... P{N-1} W{N-1} P0

using namespace std;

sem_t crit_sect;			// sekcja krytyczna
sem_t fork_in_use[N];		// semafory widelcow
sem_t napkin[BOXES];		// 5 semaforow na serwetki

int fork_owner[N];			// nr filozofa posiadającego i-ty widelec
int fork_state[N];			// stan widelca (<brudny> | <czysty>)
int meals_to_go[N];			// ilosc posilkow do zjedzenia i-tego filozofa

void start(int);
void eat(int);
void take_fork(int);
void take_napkin(int);
void put_all(int);


int main()
{
	thread philosophers[N];			// watki (filozofowie)
	sem_init(&crit_sect, 0, 1);		// semafor sekcji krytycznej (do printfa)

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
	}

	for (int i = 0; i < N; i++)
	{
		philosophers[i] = thread(start, i);
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
	while (meals_to_go[phil_id] != 0) // dopoki filozof ma co jesc
	{
		take_fork(phil_id); // filozof bierze widelce

		if (fork_state[LEFT] == CLEAN && fork_state[RIGHT] == CLEAN &&
			fork_owner[LEFT] == phil_id && fork_owner[RIGHT] == phil_id) // kiedy filozof ma 2 czyste widelce
		{
			take_napkin(phil_id);		// bierze serwetke
			eat(phil_id);				// je
			put_all(phil_id);			// oddaje widelce i serwetke
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

void take_napkin(int phil_id)
{
	sem_wait(&napkin[phil_id / 4]);			// czeka, az serwetka bedzie wolna; po czekaniu ja zabiera
}

void eat(int phil_id)
{
	usleep(1);					// symulacja jedzenia

	sem_wait(&crit_sect);		// sekcja krytyczna (dla printfa)
	meals_to_go[phil_id]--;		// zjedzony posilek
	printf("%2d. filozof zjadl (%d/%d)\n", phil_id, MEALS - meals_to_go[phil_id], MEALS);
	sem_post(&crit_sect);
}

void put_all(int phil_id) // tak naprawde nie odklada, tylko zmienia na brudne (ale trzyma dalej u siebie)
{
	fork_state[LEFT] = DIRTY;			// lewy widelec jest teraz "brudny"
	fork_state[RIGHT] = DIRTY;			// prawy widelec jest teraz "brudny"

	sem_post(&fork_in_use[LEFT]);		// odblokowuje lewy widelec sasiadowi
	sem_post(&fork_in_use[RIGHT]);		// odblokowuje prawy widelec sasiadowi
	sem_post(&napkin[phil_id / 4]);		// odblokowuje serwetke
}