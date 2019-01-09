#pragma once

#include "freetype_fwd.hpp"
#include "face.hpp"
#include <vector>
#include <memory>
#include "internal.hpp"

namespace freetype {

	class library {
		mutable std::vector<face> faces;
		FT_Library ft_lib;

	public:
		library() {
			internal::check_for_errors(FT_Init_FreeType(&ft_lib));
		}

		inline ~library() {
			faces.clear();
			if(ft_lib)
				internal::check_for_errors(
					FT_Done_FreeType(ft_lib)
				);
			ft_lib = nullptr;
		}

		template<class Container>
		face& face_from_container(Container container) {
			return new_memory_face(container.data(), container.size());
		}

		face& face_from_istream(std::istream&& stream) const {
			return face_from_istream(stream);
		}

		face& face_from_istream(std::istream& stream) const {
			std::streampos prev = stream.tellg();
			stream.seekg(0, std::ios_base::end);
			std::streampos size = stream.tellg() - prev;
			stream.seekg(prev);
			uint8_t* bytes = new uint8_t[size]; // API Reference -> User Allocarion
			stream.read((char*)bytes, size);

			FT_Face face_raw;
			unsigned error = FT_New_Memory_Face(ft_lib, (FT_Byte*)bytes, size, 0, &face_raw);
			internal::check_for_errors(error);
			faces.push_back(face{face_raw});
			return faces.back();
		}
	};
}