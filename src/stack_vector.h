
#pragma once

#include <cstddef>
#include <stdexcept>

template<class T, size_t cap>
class stack_vector
{
private:
	T array[cap] { };
	T* position { array };
public:
	size_t capacity() const { return cap; }
	size_t size() const { return position - array; }
	void push_back(T value) { *position++ = value; }
	T pop_back() { return *position--; }

	T& operator[] (int idx) { return array[idx]; }
	const T& operator[] (int idx) const { return array[idx]; }

	T& at (int idx) { if (idx < 0 || idx > size()) throw std::runtime_error("Invalid index"); return (*this)[idx]; }
	const T& at (int idx) const { if (idx < 0 || idx > size()) throw std::runtime_error("Invalid index"); return (*this)[idx]; }

	T* begin() { return array; }
	const T* begin() const { return array; }

	T* end() { return position; }
	const T* end() const { return position; }
};
