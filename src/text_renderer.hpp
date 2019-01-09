#pragma once

#include "freetype/face.hpp"
#include "renderer.hpp"
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
	class text_renderer : public verticies_renderer {
		glyph_cache& cache;
		std::string text;
		std::vector<int> textures_array_texture_units;
		gl::array_buffer positions;
		gl::array_buffer uvs;
		float width;
		size_t chars;
		//unsigned atlas_loc;

	public:

		text_renderer(
			std::string str,
			glyph_cache& cache, 
			std::shared_ptr<gl::program> program
		);

		void render() override;

		float get_width() {
			return width;
		}

		texture_atlas& texture_atlas() {
			return cache.get_texture_atlas();
		}
	};
}