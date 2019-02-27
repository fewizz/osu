#pragma once

#include <istream>
#include <memory>
#include <functional>

namespace estd {
	template<class T>
	std::enable_if_t<std::is_pointer_v<T>,
	std::unique_ptr<std::remove_pointer_t<T>[]>>
	get(std::istream& istream, size_t count) {
		size_t size = sizeof(T)*count;
		auto arr = std::make_unique<std::remove_pointer_t<T>[]>(size);
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

	inline size_t distance_to_end(std::istream& s) {
		return distance_to(std::ios::end, s);
	}

	template<class T>
	std::enable_if_t<std::is_pointer_v<T>,
	std::unique_ptr<std::remove_pointer_t<T>[]>>
	get(std::istream& istream) {
		get<T>(istream, distance_to_end(istream));
	}

	template<class T>
	inline bool iterate_for_range_search_while(
		std::function<bool(T* begin, size_t size)> action,
		std::istream& s,
		int range_size,
		T* buffer,
		size_t buffer_size
		) {
			//int off = 0;

			while(true) {
				s.read((char*)buffer, buffer_size);
				if(action(buffer, s.gcount()))
					return true;
				if(!s)
					return false;
				s.seekg(-range_size, std::ios::cur);
			}
		}
}