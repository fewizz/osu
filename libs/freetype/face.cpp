#include "face.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace freetype {
	namespace internal {
		extern void check_for_errors(unsigned code);
	}
}

freetype::face::~face() { 
	if(face_rec_ptr)
		FT_Done_Face((FT_Face)face_rec_ptr);
	face_rec_ptr = nullptr;
}
freetype::face::size_metrics freetype::face::get_size_metrics() {
	return freetype::face::size_metrics(((FT_Face)face_rec_ptr)->size);
}

freetype::glyph_slot freetype::face::get_glyph() {
	return freetype::glyph_slot::glyph_slot(((FT_Face)face_rec_ptr)->glyph);
}

void freetype::face::load_glyph(freetype::glyph_index index) {
	internal::check_for_errors( FT_Load_Glyph(((FT_Face)face_rec_ptr), index, FT_LOAD_RENDER) );
}

void freetype::face::set_char_size(int w, int h, int hr, int vr) {
	FT_Set_Char_Size((FT_Face)face_rec_ptr, w, h, hr, vr);
}

freetype::bbox freetype::face::get_bbox() {
	FT_BBox b = ((FT_Face)face_rec_ptr)->bbox;
	return bbox(b.xMin, b.yMin, b.xMax, b.yMax);
}

freetype::glyph_index freetype::face::get_char_index(unsigned charcode) {
	return FT_Get_Char_Index(((FT_Face)face_rec_ptr), charcode);
}

unsigned short freetype::face::size_metrics::x_ppem() {
	return ((FT_Size)size_rec_ptr)->metrics.x_ppem;
}

unsigned short freetype::face::size_metrics::y_ppem() {
	return ((FT_Size)size_rec_ptr)->metrics.y_ppem;
}

signed long freetype::face::size_metrics::height() {
	return ((FT_Size)size_rec_ptr)->metrics.height;
}