#pragma once

#include "freetype_fwd.hpp"
#include <utility>
#include "glyph_metrics.hpp"
#include "bitmap.hpp"

namespace freetype {
	class face;

	class glyph_slot {
		friend class face;
		
		FT_GlyphSlot ft_glyph_slot;
		
		glyph_slot() = default;
		glyph_slot(FT_GlyphSlot raw): ft_glyph_slot{ raw } {}
	public:
		glyph_slot(glyph_slot&& g) {
			ft_glyph_slot = std::exchange(g.ft_glyph_slot, nullptr);
		}
		glyph_slot& operator=(glyph_slot&&) = default;
	public:
		void render();
		inline bitmap get_bitmap()
		{ return {ft_glyph_slot->bitmap}; }
		inline glyph_metrics get_metrics()
		{ return {ft_glyph_slot->metrics}; }

		inline int bitmap_left() { return ft_glyph_slot->bitmap_left; }
		inline int bitmap_top() { return ft_glyph_slot->bitmap_top; }
	};
}