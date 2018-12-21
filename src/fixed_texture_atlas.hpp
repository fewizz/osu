#pragma once
#include "opengl/texture.hpp"
#include <vector>
#include <cmath>
#include <utility>

namespace gfx {

	class fixed_texture_atlas : public gl::texture_2d {
		/*std::pair<unsigned, unsigned> xy_from_index(unsigned index) {
			unsigned l = (unsigned)ceil(log2(index + 1)) + 1;
			unsigned max = pow(2, l) - 1;
			unsigned start = 2 * l - 1;

			if (index - start <= l)
				return std::make_pair(index - start, l);
			else
				return std::make_pair(l, max - index);
		}

		void resize() {

		}*/

	public:
		int texture_size;
		int textures_per_side;
		int last_index = 0;

		fixed_texture_atlas(int texture_size, int textures_per_side) : texture_size{ texture_size }, textures_per_side{textures_per_side} {
			int size = texture_size * textures_per_side;
			image<uint8_t>(gl::internal_format::rgba8, 0, size, size, gl::pixel_format::rgba, (uint8_t*)nullptr);
		}

		/* Returns right top point */
		std::pair<unsigned, unsigned> add(int w, int h, uint8_t* data) {
			int size = textures_per_side * texture_size;

			auto pair = std::make_pair((last_index % textures_per_side) * texture_size, (last_index / textures_per_side) * texture_size);

			sub_image(pair.first, pair.second, w, h, gl::pixel_format::rgba, data);

			last_index++;
			return pair;
		}
	};
}