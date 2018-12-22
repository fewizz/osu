#pragma once
#include "face.hpp"

namespace freetype {
	class glyph_slot {
		friend class face;
		void* glyph_slot_rec;

	protected:
		glyph_slot(void* glyph) :glyph_slot_rec{ glyph } {}
	public:
		glyph_slot(glyph_slot&& g):glyph_slot_rec{ g.glyph_slot_rec } {
			g.glyph_slot_rec = nullptr;
		}
	public:
		class bitmap {
			void* bitmap_ptr;
			void* data_raw();
		public:
			bitmap(void* raw) : bitmap_ptr{ raw } {}
			template<class T>
			T* data() { return static_cast<T*>(data_raw()); }
			unsigned rows();
			unsigned width();
			unsigned pitch();
		};
		class metrics {
			friend glyph_slot;
			void* glyph_metrics_ptr;
			metrics(void* ptr) : glyph_metrics_ptr{ ptr } {}
		public:
			long width();
			long height();
			long horizontal_bearing_x();
			long horizontal_bearing_y();
			long horizontal_advance();

			long vertical_bearing_x();
			long vertical_bearing_y();
			long vertical_advance();
		};

		void render();
		bitmap get_bitmap();
		metrics get_metrics();

		int bitmap_left();
		int bitmap_top();
	};
}