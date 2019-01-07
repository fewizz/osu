#pragma once
#include <istream>
#include <memory>

namespace estd {
	template<class T>
	std::unique_ptr<T[]> get(std::istream& istream, size_t count) {
		size_t size = sizeof(T)*count;
		std::unique_ptr<T[]> arr = std::make_unique<T[]>(size);
		istream.read((char*)arr.get(), size);
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

	inline size_t distance(std::ios::seekdir from, std::ios::seekdir to, std::istream& s) {
		auto cur = s.tellg();
		auto a = from == std::ios::cur ? cur : s.seekg(0, from).tellg();
		auto b = to == std::ios::cur ? cur : s.seekg(0, to).tellg();
		s.seekg(cur);
		return b - a;
	}

	inline size_t distance_to(std::ios::seekdir to, std::istream& s) {
		return distance(std::ios::cur, to, s);
	}
}