#pragma once

namespace lt {

// Intrusive doubly linked list
template <typename T> class list {
public:
	class root {
		list<T> *_first = nullptr;
		list<T> *_last = nullptr;

	public:
		constexpr void push_front(list<T> *p_elem) {
			p_elem->_root = this;
			p_elem->_next = _first;
			p_elem->_prev = nullptr;

			if (_first)
				_first->_prev = p_elem;
			else
				_last = p_elem;

			_first = p_elem;
		}

		constexpr void push_back(list<T> *p_elem) {
			p_elem->_root = this;
			p_elem->_next = nullptr;
			p_elem->_prev = _last;

			if (_last)
				_last->_next = p_elem;
			else
				_first = p_elem;

			_last = p_elem;
		}

		constexpr void erase(list<T> *p_elem) {
			if (p_elem->_next)
				p_elem->_next->_prev = p_elem->_prev;

			if (p_elem->_prev)
				p_elem->_prev->_next = p_elem->_next;

			if (_first == p_elem)
				_first = p_elem->_next;

			if (_last == p_elem)
				_last = p_elem->_prev;

			p_elem->_next = nullptr;
			p_elem->_prev = nullptr;
			p_elem->_root = nullptr;
		}

		constexpr void clear() {
			while (_first)
				erase(_first);
		}

		constexpr list<T> *front() { return _first; }
		constexpr const list<T> *front() const { return _first; }
		constexpr void pop_front() {
			if (_first)
				erase(_first);
		}

		constexpr list<T> *back() { return _last; }
		constexpr const list<T> *back() const { return _last; }
		constexpr void pop_back() {
			if (_last)
				erase(_last);
		}

		constexpr void operator=(const root &) = delete;
		constexpr root() = default;
		constexpr ~root() = default;
	};

private:
	root *_root = nullptr;
	T *_self = nullptr;
	list<T> *_next = nullptr;
	list<T> *_prev = nullptr;

public:
	constexpr bool in_list() const { return _root; }
	constexpr void erase() { _root->erase(this); }
	constexpr list<T> *next() { return _next; }
	constexpr list<T> *prev() { return _prev; }
	constexpr const list<T> *next() const { return _next; }
	constexpr const list<T> *prev() const { return _prev; }
	constexpr T *self() const { return _self; }

	constexpr void operator=(const list<T> &) = delete;
	constexpr list(T *p_self) :
			_self(p_self) {}

	constexpr ~list() {
		if (_root)
			_root->erase(this);
	}
};

} // namespace lt
