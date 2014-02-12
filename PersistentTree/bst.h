#pragma once

#include <memory>
#include <vector>
#include <climits>

using namespace std;

// domy�lny komparator
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
	struct Node
	{
		struct ModificationBox;

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

		// struktura wewnetrzna - pole modyfikacji wezla
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
		};
	} ;
	
	template <class T>
	class bstIterator
	{
		shared_ptr<Node> _ptr; // wskaznik na aktualny wezel
		int _version; // numer wersji, po ktorej iterujemy

	public:
		bstIterator()
		{
			_ptr = nullptr;
			_version = -1;
		}

		bstIterator(shared_ptr<Node> &p)
		{
			_ptr = p;
			_version = -1;
		}

		bstIterator(shared_ptr<Node> &p, int ver)
		{
			_ptr = p;
			_version = ver;
		}

		// iterator next
		shared_ptr<Node> next()
		{
			if (_version < 0)
				throw -1;
			shared_ptr<Node> x = nullptr, y = nullptr;

			// 1) idziemy raz w prawo i maksymalnie w lewo
			if (_ptr->_modbox._time <= _version && _ptr->_modbox._field == 1)	// prawe dziecko
				x = _ptr->_modbox._ptr;
			else if (_ptr->_right != nullptr)
				x = _ptr->_right;
			if (x != nullptr)
			{
				while (x != nullptr)
				{
					y = x;
					if (x->_modbox._time <= _version && x->_modbox._field == -1)	// lewe dziecko
						x = x->_modbox._ptr;
					else
						x = x->_left;
				}
				_ptr = y;
				return y;
			}
			// 2) nie ma dzieci po prawej; idziemy w gore, sprawdzajac czy jestesmy lewym dzieckiem
			y = _ptr->_parent;
			x = _ptr;
			while (y != nullptr)
			{
				if (y->_modbox._time <= _version && y->_modbox._field == -1)
				{
					if (y->_modbox._ptr == _ptr)
					{
						_ptr = y;
						return y;
					}
				}
				else if (y->_left == x)
				{
					_ptr = y;
					return y;
				}
				else
				{
					x = y;
					y = y->_parent;
				}
			}
			return y;
		}

		void operator++()
		{
			next();
		}

		T value()
		{
			return _ptr->_value;
		}
	};

public:
	typedef bstIterator<T> iterator;

	// iterator begin; ver - wersja drzewa
	iterator begin(int ver)
	{
		if (ver >= _timestamps.size())
			throw -1;
		shared_ptr<Node> e = _timestamps[ver];	// e = root w danej chwili
		shared_ptr<Node> x = nullptr;
		while(e != nullptr)
		{
			x = e;
			if (e->_left != nullptr)
				e = e->_left;
			else
			{
				if (e->_modbox._time != -1 && e->_modbox._time <= ver	// pole by�o zmodyfikowane przed dan� chwil�
							&& e->_modbox._field == -1)					// w polu mod. jest lewe dziecko
					e = e->_modbox._ptr;
				else
					e = nullptr;
			}
		}
		return iterator(x, ver);
	}

	// iterator back; ver - wersja drzewa
	shared_ptr<Node> back(int ver)
	{
		if (ver >= _timestamps.size())
			throw -1;
		shared_ptr<Node> e = _timestamps[ver];	// e = root w danej chwili
		shared_ptr<Node> x = nullptr;
		while(e != nullptr)
		{
			x = e;
			if (e->_right != nullptr)
				e = e->_right;
			else
			{
				if (e->_modbox._time != -1 && e->_modbox._time <= ver	// pole by�o zmodyfikowane przed dan� chwil�
							&& e->_modbox._field == 1)					// w polu mod. jest prawe dziecko
					e = e->_modbox._ptr;
				else
					e = nullptr;
			}
		}
		return iterator(x, ver);
	}

	// iterator end - zwraca wskaznik na element po ostatnim elemencie (czyli nullptr)
	shared_ptr<Node> end(int ver)
	{
		return nullptr;
	}

private:
	vector<shared_ptr<Node>> _timestamps; // lista przechowujaca wskazniki na korzen drzewa w chwili t

	Comparator _cmp;		// wskaznik na obiekt funkcyjny

public:
	BST()
	{
		_timestamps.clear();
		_cmp = Comparator();
	}

	bool insert(T value)
	{
		try
		{
		if (find(value, _timestamps.size() - 1))
			return false;
		}
		catch (int ex) {}
		// nie ma takiego elementu w drzewie

		shared_ptr<Node> y(nullptr);
		shared_ptr<Node> x( (_timestamps.size() > 0) ? _timestamps.back() : nullptr );

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
			_timestamps.push_back(e);
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
				_timestamps.push_back(_timestamps.back());
			}
			else	// trzeba kopiowa�
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
					_timestamps.push_back(z);	// nowy root
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

				_timestamps.push_back(_timestamps.back());
			}
		}

		return true;
	}

	bool find(T value, int ver)
	{	
		if (ver >= _timestamps.size())
			throw -1;
		if (_timestamps.empty())
			return false;
		shared_ptr<Node> ptr = _timestamps[ver];	// root
		while (ptr != nullptr && !(_cmp(value, ptr->_value) == 0))
		{
			// najpierw sprawdzamy, czy byly modyfikacje (w danej chwili lub wczesniej)
			// i czy bylo modyfikowane interesujace nas dziecko
			if (ptr->_modbox._time != -1 && ptr->_modbox._time <= ver
					&& _cmp(value, ptr->_value) == ptr->_modbox._field) // tozsame z warunkami: (_cmp(value, ptr->_value) == 1 && ptr->_modbox._field == 1) 
																		// oraz (_cmp(value, ptr->_value) == -1 && ptr->_modbox._field == -1)
				ptr = ptr->_modbox._ptr;
			// potem sprawdzamy bezposrednie dzieci
			else if (_cmp(value, ptr->_value) == -1 && ptr->_left != nullptr)
				ptr = ptr->_left;
			else if (ptr->_right != nullptr)
				ptr = ptr->_right;
			// jesli zaden z warunkow nie jest spelniony, zwracamy null
			else
				ptr = nullptr;
		}
		return (ptr != nullptr);
	}

	bool erase(T value)
	{
		// szukanie wezla z wartoscia 'value'		
		shared_ptr<Node> ptr( (_timestamps.size() > 0) ? _timestamps.back() : nullptr );
		shared_ptr<Node> y(nullptr);

		while (ptr != nullptr && !(_cmp(value, ptr->_value) == 0))
		{
			// najpierw sprawdzamy, czy byly modyfikacje (w danej chwili lub wczesniej)
			// i czy bylo modyfikowane interesujace nas dziecko
			if (ptr->_modbox._time != -1 && ptr->_modbox._time <= ver
					&& _cmp(value, ptr->_value) == ptr->_modbox._field) // tozsame z warunkami: (_cmp(value, ptr->_value) == 1 && ptr->_modbox._field == 1) 
																		// oraz (_cmp(value, ptr->_value) == -1 && ptr->_modbox._field == -1)
				ptr = ptr->_modbox._ptr;
			// potem sprawdzamy bezposrednie dzieci
			else if (_cmp(value, ptr->_value) == -1 && ptr->_left != nullptr)
				ptr = ptr->_left;
			else if (ptr->_right != nullptr)
				ptr = ptr->_right;
			// jesli zaden z warunkow nie jest spelniony, nie ma takiego elementu
			else
				ptr = nullptr;
		}
		if (ptr == nullptr)
			return false;	// taki element nie istnieje

		// ptr wskazuje na usuwany element
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
					ptr = y;	// UWAGA NA BUGI
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