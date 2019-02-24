#include "freetype/library.hpp"
#include "freetype/freetype_fwd.hpp"
#include "freetype/istream_wrapper.hpp"

#define ft_lib_c static_cast<FT_LibraryRec_*>(ft_lib)
using namespace freetype;

freetype::library::library() {
    FT_Library lib;
    freetype::internal::check_for_errors(FT_Init_FreeType(& lib ));
    ft_lib = lib;
}

freetype::library::~library() {
	faces.clear();
	if(ft_lib)
		internal::check_for_errors(
			FT_Done_FreeType(ft_lib_c)
		);
	ft_lib = nullptr;
}

face& freetype::library::face_from_istream(std::unique_ptr<std::istream> stream) const {
	FT_Face raw;
	FT_Open_Args args;
	args.flags = FT_OPEN_STREAM;
	args.stream = new std_ft_stream(std::move(stream));
    std::cout << "opening face" << "\n";
	internal::check_for_errors(FT_Open_Face(ft_lib_c, &args, 0, &raw));
    faces.push_back({raw});
    return faces.back();
}