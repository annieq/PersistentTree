//// PersistentTree.cpp : Defines the entry point for the console application.
////
//
//#include "stdafx.h"
//
//#include "bst.h"
//#include <iostream>
//
//using namespace std;
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//	system("pause");
//	return 0;
//}
//

// zbp3_ak.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "bst.h"
#include <iostream>
#include <time.h>

using namespace std;

// obiekt funkcyjny
class mySorter
{
public:
	int operator()(int x, int y)
	{
		if (x > y)
			return -1;
		if (x < y)
			return 1;
		else
			return 0;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	BST<int> myBst;
	srand((int)time(NULL));

	cout << "=== MIN, MAX ===\n";
	try
	{
		cout << "Max value: " << myBst.max() << "\n";
		cout << "Min value: " << myBst.min() << "\n";
	}
	catch (int e)
	{
		cout << "Exception: " << e << "\n";
	}

	cout << "\n=== INSERTING ===\n";
	// losowe wartosci do wstawienia
	int values[15];
	for (int i=0; i<15; ++i)
		values[i] = rand() % 50;

	for (int i=0; i<15; ++i)
	{
		cout << "Inserting: " << values[i] << "...  ";
		if (myBst.insert(values[i]))
			cout << "Inserted.\n";
		else
			cout << "Can't insert " << values[i] << "!\n";
	}
	for (int i=2; i<5; ++i)
	{
		cout << "Inserting: " << values[i] << "...  ";
		if (myBst.insert(values[i]))
			cout << "Inserted.\n";
		else
			cout << "Can't insert " << values[i] << "!\n";
	}

	cout << "\n=== Print root: ===\n";
	myBst.printPart();

	// szukanie
	cout << "\n=== SEARCHING ===\n";
	cout << "Searching: " << values[3] << "...  ";
	if (myBst.find(values[3]))
		cout << "Found!\n";
	else
		cout << "Not found.\n";

	cout << "Searching: " << 51 << "...  ";
	if (myBst.find(51))
		cout << "Found!\n";
	else
		cout << "Not found.\n";
	
	cout << "\n=== MIN, MAX ===\n";
	try 
	{
		cout << "Max value: " << myBst.max() << "\n";
		cout << "Min value: " << myBst.min() << "\n";
	}
	catch (int e)
	{
		cout << "Exception: " << e << "\n";
	}

	// usuwanie
	cout << "\n=== ERASING ===\n";
	cout << "Erasing: " << values[6] << "...  ";
	if (myBst.erase(values[6]))
		cout << "Erased.\n";
	else
		cout << "Can't erase " << values[6] << "!\n";

	cout << "Erasing: " << 51 << "...  ";
	if (myBst.erase(51))
		cout << "Erased.\n";
	else
		cout << "Can't erase " << 51 << "!\n";


	// ZADANIE 2
	cout << "\n=== PART 2 ===\n";
	BST<int, mySorter> bst2;

	for (int i=0; i<15; ++i)
	{
		cout << "Inserting: " << values[i] << "...  ";
		if (bst2.insert(values[i]))
			cout << "Inserted.\n";
		else
			cout << "Can't insert " << values[i] << "!\n";
	}
	cout << "\n=== Print root: ===\n";
	bst2.printPart();
	cout << "Searching: " << values[3] << "...  ";
	if (bst2.find(values[3]))
		cout << "Found!\n";
	else
		cout << "Not found.\n";
	try 
	{
		cout << "Max value: " << bst2.max() << "\n";
		cout << "Min value: " << bst2.min() << "\n";
	}
	catch (int e)
	{
		cout << "Exception: " << e << "\n";
	}


	system("pause");
	return 0;
}