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

namespace gfx {
	class text_renderer : public verticies_renderer {
		fixed_texture_atlas tex_atlas;
		std::string text;
		std::vector<int> textures_array_texture_units;
		gl::array_buffer positions;
		gl::array_buffer uvs;
		float width;
		size_t chars;
		unsigned atlas_loc;

	public:

		text_renderer(
			std::string str,
			freetype::face& face, 
			std::shared_ptr<gl::program> program
		);

		void render() override;

		float get_width() {
			return width;
		}

		std::shared_ptr<fixed_texture_atlas> texture_atlas() {
			return std::shared_ptr<gfx::fixed_texture_atlas>{&tex_atlas, [](auto){}};
		}
	};
}