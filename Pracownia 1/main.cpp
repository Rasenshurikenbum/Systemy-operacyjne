//Krzysztof Lis, Piotr Gdowski
//Pracownia P1

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
int __gcd(int a, int b)
	{
	if(b==0) return a;
	if(b>a) return __gcd(b,a);
	return __gcd(b, a%b);
}
int main()
{
	cout << "Hello world!\n";
	for(int i=0;i<10;++i) cout << __gcd(4,i) << endl;
	return 0;
}
