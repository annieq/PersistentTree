
#include "stdafx.h"
#include "bst.h"
#include <iostream>

using namespace std;

void printTree(BST<int>, int);

int _tmain(int argc, _TCHAR* argv[])
{
	BST<int> bst;

	bst.insert(20);
	bst.insert(10);
	bst.insert(5);
	bst.insert(15);
	bst.insert(17);
	bst.insert(12);
	bst.insert(21);

	printTree(bst, 0);
	printTree(bst, 1);
	printTree(bst, 2);
	printTree(bst, 3);
	printTree(bst, 4);
	printTree(bst, 5);
	printTree(bst, 6);

	system("pause");
	return 0;
}

void printTree(BST<int> bst, int ver)
{
	BST<int>::iterator it = bst.begin(ver);
	cout << "== Drzewo w chwili " << ver << ":\n";
	cout << it.value();
	while (it.next(ver) != nullptr)
	{
		cout << ",\t" << it.value() ;
	}
	cout << "\n\n";
}