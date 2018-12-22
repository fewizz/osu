#include "library.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <algorithm>

namespace freetype{
	namespace internal {
		extern void check_for_errors(unsigned code);
	}
}

freetype::library::library() {
	FT_Library lib;
	internal::check_for_errors(FT_Init_FreeType(&lib));
	library_rec_ptr = lib;
}

freetype::face* freetype::library::new_memory_face(void* begin, size_t size) {
	FT_Face face_raw;
	FT_Library lib{ (FT_Library)library_rec_ptr };
	unsigned error = FT_New_Memory_Face(lib, (FT_Byte*)begin, size, 0, &face_raw);
	internal::check_for_errors(error);
	faces.push_back(face(face_raw, this));
	return &faces.back();
}

freetype::library::~library() {
	std::destroy(faces.begin(), faces.end());
	FT_Done_FreeType((FT_Library)library_rec_ptr);
	library_rec_ptr = nullptr;
}