#pragma once
#include <vector>
#include <istream>
#include <memory>
#include "face.hpp"

namespace freetype {
	class face;

	class library {
		mutable std::vector<face> faces;
		void* library_rec_ptr;
		face* new_memory_face(void* begin, size_t size) const;
	public:
		library();

		template<class Container>
		face* face_from_container(Container container) {
			return new_memory_face(container.data(), container.size());
		}

		face* face_from_istream(std::istream&& stream) const {
			return face_from_istream(stream);
		}

		face* face_from_istream(std::istream& stream) const {
			std::streampos prev = stream.tellg();
			stream.seekg(0, std::ios_base::end);
			std::streampos size = stream.tellg() - prev;
			stream.seekg(prev);
			uint8_t* bytes = new uint8_t[size]; // API Reference -> User Allocarion
			stream.read((char*)bytes, size);
			return new_memory_face(bytes, size);
		}

		~library();
	};
}