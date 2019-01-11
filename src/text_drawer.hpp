#pragma once

#include "freetype/face.hpp"
#include "draw.hpp"
#include <memory>
#include <string>
#include "utf8.h"
#include "texture_atlas.hpp"
#include <algorithm>
#include <cmath>
#include <map>
#include "opengl/core.hpp"
#include "glyph_cache.hpp"

namespace gfx {
	class text_drawer : public gfx::triangles_drawer<> {
		glyph_cache& cache;
		std::string text;
		std::vector<int> textures_array_texture_units;
		gl::array_buffer positions;
		gl::array_buffer uvs;
		float width;
		size_t chars;

	public:

		/*using origin_baseline_start = glm::vec2{};

		static inline glm::vec2 origin_top_left(freetype::face face) {
			return
			{
				0,
				origin_baseline_start -
				face.get_size_metrics().height() / 64.0
			};
		}*/

		enum class origin {
			top_left,
			baseline_start
		};

		text_drawer(
			std::string str,
			glyph_cache& cache,
			std::shared_ptr<gl::program> program,
			origin o
		);

		void draw();

		float get_width() {
			return width;
		}

		texture_atlas& texture_atlas() {
			return cache.get_texture_atlas();
		}
	};


}