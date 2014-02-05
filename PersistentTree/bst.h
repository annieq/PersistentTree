#pragma once

#include <memory>
#include <vector>
#include <climits>

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

		Node()
		{
			_left = _right = _parent = nullptr;
			_value = INT_MIN;
			_modbox = ModificationBox();
		}

		Node(shared_ptr<Node> n)	// kopiowanie (bez pola modyfikacji)
		{
			_left = n->_left;
			_right = n->_right;
			_parent = n->_parent;
			_value = n->_value;
			_modbox = ModificationBox();
		}
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

		while (x != nullptr)	// szukamy rodzica elementu do wstawienia
		{
			y = x;	// y - bedzie przechowywac wskaznik na rodzica

			if (_cmp(e->_value, x->_value) == -1) // idziemy w lewo
			{
				if (x->_modbox._field == -1)
					x = x->_modbox._ptr;	// zmodyfikowane lewe dziecko
				else
					x = x->_left;
			}
			else	// idziemy w prawo
			{
				if (x->_modbox._field == 1)
					x = x->_modbox._ptr;	// zmodyfikowane prawe dziecko
				else
					x = x->_right;
			}
		}
		e->_parent = y;
		shared_ptr<Node> parent = y;

		if (y == nullptr) // nasz element jest korzeniem
		{
			_root = e;
			_timestamps.push_back(_root);
		}
		else	// przypisanie rodzicowi (y) dziecka (e)
		{
			if (y->_modbox._time == -1)		// pole modyfikacji jest wolne
			{
				if (_cmp(e->_value, y->_value) == -1)	// lewe dziecko
				{
					y->_modbox._field = -1;
				}
				else	// prawe dziecko
				{
					y->_modbox._field = 1;
				}
				y->_modbox._time = _timestamps.size(); // ustawia aktualna wartosc t
				y->_modbox._ptr = e;	// przypisanie dziecka
				_timestamps.push_back(_root);
			}
			else	// trzeba kopiowaæ
			{
				while (y != nullptr && y->_modbox._time != -1)	// szukamy wezla z pustym polem modyfikacji
				{
					x = y;
					y = y->_parent;
				}
					
				shared_ptr<Node> z = y;
				shared_ptr<Node> zParent = y;

				if (y == nullptr) // doszlismy do korzenia
				{
					// kopiujemy cale drzewo
					z = make_shared<Node>(x);	// kopiowanie roota
					zParent = z;
					_root = z;	// nowy root
					_timestamps.push_back(_root);
					x = x->_modbox._ptr;

					while (x != nullptr)	// kopiowanie drzewa
					{
						if (_cmp(x->_value, x->_parent->_value) == -1)
							z = z->_left;
						else
							z = z->_right;
						z = make_shared<Node>(x);
						z->_parent = zParent;
						if (_cmp(z->_value, zParent->_value) == -1)
							zParent->_left = z;
						else
							zParent->_right = z;

						x = x->_modbox._ptr;	// x = dziecko iksa
						zParent = z; // dla nastepnego przebiegu
					}
					// przypisanie rodzicowi dziecka (e)
					while (_cmp(parent->_value, z->_value) != 0)	// szukamy rodzica
						z = z->_parent;
					if (_cmp(e->_value, z->_value) == -1)	// przypisujemy
						z->_left = e;
					else
						z->_right = e;
					e->_parent = z;

					return true;
				}

				// kopiowanie fragmentu drzewa
				// y - najniszy el. z wolnym polem modyfikacji
				// x - pierwszy el. do skopiowania
				x = (_cmp(e->_value, y->_value) == -1) ? (y->_left) : (y->_right);
				z = make_shared<Node>(x);
				y->_modbox._time = _timestamps.size();
				y->_modbox._field = (_cmp(x->_value, y->_value) == -1) ? -1 : 1;
				y->_modbox._ptr = z;
				zParent = z;
				x = x->_modbox._ptr;

				while (x != nullptr)
				{
					if (_cmp(x->_value, x->_parent->_value) == -1)
						z = z->_left;
					else
						z = z->_right;
					z = make_shared<Node>(x);
					z->_parent = zParent;
					if (_cmp(z->_value, zParent->_value) == -1)
						zParent->_left = z;
					else
						zParent->_right = z;

					x = x->_modbox._ptr;	// x = dziecko x
					zParent = z; // dla nastepnego przebiegu
				}
				// przypisanie rodzicowi dziecka (e)
				while (_cmp(parent->_value, z->_value) != 0)	// szukamy rodzica
					z = z->_parent;
				if (_cmp(e->_value, z->_value) == -1)
					z->_left = e;
				else
					z->_right = e;
				e->_parent = z;

				_timestamps.push_back(_root);
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