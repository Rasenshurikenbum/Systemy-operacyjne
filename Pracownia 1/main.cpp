//Krzysztof Lis, Piotr Gdowski
//Pracownia P1

/*Zapoznaj się z klasycznymi problemami synchronizacji. Podstawa materiału zawarta jest
w rozdziałach 5 i 6 z książki Stallingsa ("Concurrency: Mutual Exclusion and Synchronization"
oraz "Concurrency: Deadlock and Starvation"). Problemy synchronizacji są obszernie opisane w internecie,
poniżej odnośnik do przykładowej listy: http://nob.cs.ucdavis.edu/classes/ecs150-2008-02/handouts/sync/sync-problems.html.
Tę listę warto uzupełnić o takie problemy (warianty powyższych) jak Cigarette Smokers czy The Barbershop.*/

/*Wybrany temat:
Dwudziestu filozofów siedzi przy stole i każdy wykonuje jedną z dwóch czynności – albo je, albo rozmyśla. 
Stół jest okrągły, przed każdym z nich znajduje się miska ze spaghetti, a pomiędzy każdą sąsiadującą parą 
filozofów leży widelec, a więc każda osoba ma przy sobie dwie sztuki - po swojej lewej i prawej stronie. 
Ponieważ jedzenie potrawy jest trudne przy użyciu jednego widelca, zakłada się, że każdy filozof korzysta 
z dwóch. Dodatkowo nie ma możliwości skorzystania z widelca, który nie znajduje się bezpośrednio przed 
daną osobą. Oprócz tego każdy filozof potrzebuje serwetki, aby kulturalnie mógł zjeść posiłek nie brudząc się.
Na stole znajduje się 5 pudełek z dwoma serwetkami w każdym, po skończonym posiłku filozof oddaje serwetkę do pudełka.
Do i-tego pudełka z serwetkami dostęp mają filozofowie 4(i-1)+1, 4(i-1)+2, 4(i-1)+3, 4(i-1)+4.

Rozszerzenie: Zwiększenie liczby postaci i zasobów do 20 + manewry z serwetkami.*/


#include <iostream>
#include <thread>

using namespace std;

int main()
{
	cout << "Hello world!\n";
	
	return 0;
}