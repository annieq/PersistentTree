#pragma once

#include <memory>
#include <vector>

using namespace std;

template <class T>
class defComp
{
public:
	int operator()(T t1, T t2)
	{		
		if (t1 < t2)
			return -1;
		if (t2 < t1)
			return 1;
		return 0;
	}
};

template <class T, class Comparator = defComp<T>>
class BST
{
private:
	struct ModificationBox;

	struct Node
	{
		shared_ptr<Node> _left;
		shared_ptr<Node> _right;
		shared_ptr<Node> _parent;

		T _value;		// wartosc elementu
		ModificationBox _modbox;	// pole modyfikacji
	} ;

	struct ModificationBox
	{
		int _time;				// chwila modyfikacji (-1 oznacza brak modyfikacji)
		int _field;				// co bylo zmodyfikowane (-1 lewe dziecko, 1 prawe dziecko, 0 nic)
		shared_ptr<Node> _ptr;	// wskaznik na nowe dziecko

		ModificationBox()
		{
			_time = -1;
			_field = 0;
			_ptr = nullptr;
		}
	} ;

	shared_ptr<Node> _root;	// korzen drzewa
	vector<shared_ptr<Node>> _timestamps; // lista przechowujaca wskazniki na korzen drzewa w chwili t

	Comparator _cmp;		// wskaznik na obiekt funkcyjny

public:
	BST()
	{
		_root = nullptr;
		_timestamps.clear();

		_cmp = Comparator();
	}

	bool insert(T value)
	{
		if (find(value))
			return false;

		shared_ptr<Node> y(nullptr);
		shared_ptr<Node> x(_root);

		shared_ptr<Node> e = make_shared<Node>(Node());
		e->_value = value;

		while (x != nullptr)
		{
			y = x;
			if (_cmp(e->_value, x->_value) == -1)
				x = x->_left;
			else
				x = x->_right;
		}
		e->_parent = y;
		if (y == nullptr)
		{
			_root = e;
			_timestamps.push_back(_root);
		}
		else 
		{
			if (_cmp(e->_value, y->_value) == -1)
			{
				//y->_left = e;
				if (y->_modbox._time == -1) // pole modyfikacji jest wolne
				{
					y->_modbox._time = _timestamps.size(); // todo: spr. czy size czy capacity czy co tam; ustawia aktualna wartosc t
					y->_modbox._field = -1;
					y->_modbox._ptr = e;
					_timestamps.push_back(_root);
				}
				else
				{
					// kopiowanie
					shared_ptr<Node> z = nullptr;
					while (y->_modbox._time != -1)
					{
						z = make_shared<Node>(Node());
						z = y; // kopiowanie wezla - do sprawdzenia!
						y = y->_parent;
					}
					if (y == nullptr) // doszlismy do korzenia
					{
						_timestamps.push_back(z);
					}
					else
					{
						y->_modbox._time = _timestamps.size(); // size czy co innego?
						y->_modbox._field = (_cmp(z->_value, y->_value)) ? -1 : 1; // do spr
						y->_modbox._ptr = z;
						_timestamps.push_back(_root);
					}
				}
			}
			else
			{
				//y->_right = e;
				if (y->_modbox._time == -1) // pole modyfikacji jest wolne
				{
					y->_modbox._time = _timestamps.size(); // todo: spr. czy size czy capacity czy co tam; ustawia aktualna wartosc t
					y->_modbox._field = 1;
					y->_modbox._ptr = e;
				}
				else
				{
					// todo: kopiowanie

					// _timestamps.push_back( ... );
				}
			}
		}

		return true;
	}

	bool find(T value)
	{
		shared_ptr<Node> ptr = _root;
		while (ptr != nullptr && !(_cmp(value, ptr->_value) == 0))
		{
			if (_cmp(value, ptr->_value) == -1)
				ptr = ptr->_left;
			else
				ptr = ptr->_right;
		}
		return (ptr != nullptr);
	}

	bool erase(T value)
	{
		// szukanie wezla z wartoscia 'value'		
		shared_ptr<Node> ptr = _root;
		shared_ptr<Node> y = nullptr;

		while (ptr != nullptr && !(_cmp(value, ptr->_value) == 0))
		{
			if (_cmp(value, ptr->_value) == -1)
				ptr = ptr->_left;
			else
				ptr = ptr->_right;
		}
		if (ptr == nullptr)
			return false;	// taki element nie istnieje

		if (ptr->_left == nullptr || ptr->_right == nullptr)
			y = ptr;
		else	// szukanie nastepnika
		{
			if (ptr->_right != nullptr)
			{
				y = ptr->_right;
				while (y->_left != nullptr)
					y = y->_left;
			}
			else
			{
				y = ptr->_parent;
				while (y != nullptr && ptr == y->_right)
				{
					ptr = y;
					y = y->_parent;
				}
			}
			// y wskazuje na nastepnik usuwanego elementu
		}

		shared_ptr<Node> x;
		if (y->_left != nullptr)
			x = y->_left;
		else
			x = y->_right;
		if (x != nullptr)
			x->_parent = y->_parent;
		if (y->_parent == nullptr)
			_root = x;
		else
		{
			if (y == y->_parent->_left)
				y->_parent->_left = x;
			else
				y->_parent->_right = x;
		}
		if (y != ptr)
			ptr->_value = y->_value;
		return true;
	}

	T& min(void)
	{
		if (_root == nullptr)
			throw -1;
		shared_ptr<Node> ptr = _root;
		while (ptr->_left != nullptr)
			ptr = ptr->_left;
		return ptr->_value;
	}

	T& max(void)
	{
		if (_root == nullptr)
			throw -2;
		shared_ptr<Node> ptr = _root;
		while (ptr->_right != nullptr)
			ptr = ptr->_right;
		return ptr->_value;
	}

	// funkcja pomocnicza, wypisuje korzen i jego synow
	void printPart()
	{
		if (_root != nullptr)
		{
			cout << "Root: " << _root->_value << "\n";
			if (_root->_left != nullptr)
				cout << "Left child: " << _root->_left->_value << "\n";
			else
				cout << "Left child: NULL\n";
			if (_root->_right != nullptr)
				cout << "Right child: " << _root->_right->_value << "\n";
			else
				cout << "Right child: NULL\n";
		}
		else
			cout << "Root: NULL\n";
	}
};