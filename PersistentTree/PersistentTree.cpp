
#include "stdafx.h"
#include "bst.h"
#include <iostream>

using namespace std;

void printTree(BST<int>, int);

int _tmain(int argc, _TCHAR* argv[])
{
	BST<int> bst, bst2;

	try {
		bst.insert(20);
		bst.insert(10);
		bst.insert(5);
		bst.insert(15);
		bst.insert(17);
		bst.insert(12);
		bst.insert(21);

		bst2.insert(20);
		bst2.insert(10);
		bst2.insert(5);
		bst2.erase(5);

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
		bool found = bst.find(10, 6);
		found = bst.find(17, 3);
		found = bst.find(20, 20);
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