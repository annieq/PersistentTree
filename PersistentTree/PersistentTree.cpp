
#include "stdafx.h"
#include "bst.h"
#include <iostream>
#include <ctime>
#include <iomanip>

using namespace std;

void printTree(BST<int>, int);

int _tmain(int argc, _TCHAR* argv[])
{
	BST<int> bst, bst2;
	clock_t t;
	cout << fixed << setprecision(6);

	try {
		t = clock();
		bst.insert(20);
		t = clock() - t;
		cout << "Wstawianie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";
		t = clock();
		bst.insert(10);
		t = clock() - t;
		cout << "Wstawianie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";
		t = clock();
		bst.insert(5);
		t = clock() - t;
		cout << "Wstawianie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";
		t = clock();
		bst.insert(15);
		t = clock() - t;
		cout << "Wstawianie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";
		t = clock();
		bst.insert(17);
		t = clock() - t;
		cout << "Wstawianie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";
		t = clock();
		bst.insert(12);
		t = clock() - t;
		cout << "Wstawianie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";
		t = clock();
		bst.insert(21);
		t = clock() - t;
		cout << "Wstawianie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";
		
		bst2.insert(20);
		bst2.insert(10);
		bst2.insert(5);

		t = clock();
		bst2.erase(5);
		t = clock() - t;
		cout << "Usuwanie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";
		t = clock();
		bst2.erase(10);
		t = clock() - t;
		cout << "Usuwanie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";
		t = clock();
		bst2.erase(20);
		t = clock() - t;
		cout << "Usuwanie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";

		for (int i=0; i<7; ++i)
			printTree(bst, i);
		cout << " ----------- Drugie drzewo --------\n\n";
		for (int i=0; i<4; ++i)
			printTree(bst2, i);
	}
	catch (int e) {
		cout << "Wyjatek przy wstawianiu/usuwaniu: " << e << endl;
	}
	try {
		
		t = clock();
		bool found = bst.find(10, 6);
		t = clock() - t;
		cout << "Szukanie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";
		t = clock();
		found = bst.find(17, 3);
		t = clock() - t;
		cout << "Szukanie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";
		t = clock();
		found = bst.find(20, 20);
		t = clock() - t;
		cout << "Szukanie: " << ((float)t)/CLOCKS_PER_SEC << " s.\n";

	}
	catch (int e) {
		cout << "Wyjatek przy przeszukiwaniu: " <<e << endl;
	}

	system("pause");
	return 0;
}

void printTree(BST<int> bst, int ver)
{
	BST<int>::iterator it = bst.begin(ver);
	cout << "== Drzewo w chwili " << ver << ":\n";
	cout << it.value();
	while (it.next() != nullptr)
	{
		cout << ",\t" << it.value() ;
	}
	cout << "\n\n";
}