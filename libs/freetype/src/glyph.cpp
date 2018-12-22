#include "glyph.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

freetype::glyph_slot::bitmap freetype::glyph_slot::get_bitmap() {
	return freetype::glyph_slot::bitmap{ &((FT_GlyphSlot)glyph_slot_rec)->bitmap };
}

freetype::glyph_slot::metrics freetype::glyph_slot::get_metrics() {
	return freetype::glyph_slot::metrics{ &((FT_GlyphSlot)glyph_slot_rec)->metrics };
}

/* Bitmap */
void* freetype::glyph_slot::bitmap::data_raw() {
	return ((FT_Bitmap*)bitmap_ptr)->buffer;
}

unsigned freetype::glyph_slot::bitmap::rows() {
	return ((FT_Bitmap*)bitmap_ptr)->rows;
}

unsigned freetype::glyph_slot::bitmap::width() {
	return ((FT_Bitmap*)bitmap_ptr)->width;
}

unsigned freetype::glyph_slot::bitmap::pitch() {
	return ((FT_Bitmap*)bitmap_ptr)->pitch;
}

/* Metrics */
long freetype::glyph_slot::metrics::width() {
	return ((FT_Glyph_Metrics*)glyph_metrics_ptr)->width;
}

long freetype::glyph_slot::metrics::height() {
	return ((FT_Glyph_Metrics*)glyph_metrics_ptr)->height;
}

long freetype::glyph_slot::metrics::horizontal_bearing_x() {
	return ((FT_Glyph_Metrics*)glyph_metrics_ptr)->horiBearingX;
}

long freetype::glyph_slot::metrics::horizontal_bearing_y() {
	return ((FT_Glyph_Metrics*)glyph_metrics_ptr)->horiBearingY;
}

long freetype::glyph_slot::metrics::horizontal_advance() {
	return ((FT_Glyph_Metrics*)glyph_metrics_ptr)->horiAdvance;
}

long freetype::glyph_slot::metrics::vertical_bearing_x() {
	return ((FT_Glyph_Metrics*)glyph_metrics_ptr)->vertBearingX;
}

long freetype::glyph_slot::metrics::vertical_bearing_y() {
	return ((FT_Glyph_Metrics*)glyph_metrics_ptr)->vertBearingY;
}

long freetype::glyph_slot::metrics::vertical_advance() {
	return ((FT_Glyph_Metrics*)glyph_metrics_ptr)->vertAdvance;
}