#pragma once
#include "library.hpp"
#include "glyph.hpp"
#include "bbox.hpp"

namespace freetype {
	typedef unsigned glyph_index;

	class glyph_slot;

	class face {
		friend class library;

		void* face_rec_ptr;
		library* lib;

		face(void* rec_ptr, library* lib) :face_rec_ptr{ rec_ptr }, lib{lib} {}
	public:
		face(face&& f) :lib{ f.lib }, face_rec_ptr{f.face_rec_ptr} {
			f.face_rec_ptr = nullptr;
			f.lib = nullptr;
		}
		~face();

		glyph_index get_char_index(unsigned charcode);

		void set_char_size(int w, int h, int hr, int vr);

		bbox get_bbox();

		void load_glyph(glyph_index index);

		glyph_slot get_glyph();

		class size_metrics;

		size_metrics get_size_metrics();

		class size_metrics {
			friend class face;
			void* size_rec_ptr;
			size_metrics(void* data) :size_rec_ptr{ data } {}
		public:
			unsigned short x_ppem();
			unsigned short y_ppem();
			signed long height();
		};
	};
}