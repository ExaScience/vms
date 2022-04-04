#pragma once

template <class T, std::size_t N>
struct arr {
	T& operator[](std::size_t pos) { return elems[pos]; }
	const T& operator[](std::size_t pos) const { return elems[pos]; }

	T elems[N];
};