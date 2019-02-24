#pragma once

#include "freetype_fwd.hpp"
#include "internal.hpp"
#include "glyph_slot.hpp"
#include "bbox.hpp"
#include "size_metrics.hpp"
#include <memory>
#include <functional>

namespace freetype {
	typedef unsigned glyph_index;

	class glyph_slot;

	class face {
		friend class library;

		FT_Face ft_face;
		glyph_slot slot;

		face(FT_Face raw)
		:ft_face{ raw } {}
	public:
		face(face&& f) {
			ft_face = std::exchange(f.ft_face, nullptr);
		}

		face& operator=(face&& f) {
			ft_face = std::exchange(f.ft_face, nullptr);
			return *this;
		}

		inline ~face() {
			if(ft_face) {
				internal::check_for_errors(FT_Done_Face(ft_face));
			}
			ft_face = nullptr;
		}

		inline glyph_index get_char_index(unsigned charcode) {
			return FT_Get_Char_Index(ft_face, charcode);
		}

		inline void set_char_size(int w, int h, int hr, int vr) {
			internal::check_for_errors(
				FT_Set_Char_Size(ft_face, w, h, hr, vr)
			);
		}

		inline bbox get_bbox() {
			return ft_face->bbox;
		}

		inline glyph_slot& load_glyph(glyph_index index) {
			internal::check_for_errors (
				FT_Load_Glyph(ft_face, index, FT_LOAD_RENDER)
			);
			slot = ft_face->glyph;
			return glyph();
		}

		inline glyph_slot& glyph() {
			return slot;
		}

		inline unsigned short units_per_em() {
			return ft_face->units_per_EM;
		}

		inline size_metrics get_size_metrics() {
			return ft_face->size->metrics;
		}
	};
}