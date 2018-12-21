#pragma once
#include <istream>

namespace estd {
	template<class T>
	std::unique_ptr<T*> get(std::istream& istream, size_t count) {
		size_t size = sizeof(T)*count;
		std::unique_ptr<char*> arr = std::make_unique<char*>(new char[size]);
		istream.read(*arr, size);
		return arr;
	}

	template<class T>
	T get(std::istream& istream) {
		T t;
		istream.read((char*)&t, sizeof(T));
		return t;
	}

	template<class T>
	std::unique_ptr<T*> peek(std::istream& istream, size_t count) {
		std::unique_ptr<char*> t = get<T>(istream, count);
		istream.seekg(-sizeof(T)*count, std::ios_base::cur);
		return t;
	}

	template<class T>
	T peek(std::istream& istream) {
		T t = get<T>(istream);
		istream.seekg(-sizeof(T), std::ios_base::cur);
	}
}