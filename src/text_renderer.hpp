#pragma once

#include "freetype/freetype.hpp"
#include "renderer.hpp"
#include <memory>
#include <string>
#include "utf8.h"
#include "fixed_texture_atlas.hpp"
#include <algorithm>
#include <cmath>
#include <map>
#include "opengl/core.hpp"

namespace gfx {
	class text_renderer : public verticies_renderer {
		gfx::fixed_texture_atlas tex_atlas;
		std::string text;
		std::vector<int> textures_array_texture_units;
		gl::array_buffer positions;
		gl::array_buffer uvs;
		float width;
		size_t chars;
		unsigned atlas_loc;

		static int calculate_max_size(freetype::face& face) {
			freetype::bbox global_box{ face.get_bbox() };
			return std::max(global_box.x_max - global_box.x_min, global_box.y_max - global_box.y_min) / 64 + 1;
		}
	public:
		text_renderer(std::string str, freetype::face& face, std::shared_ptr<gl::program> program)
			:verticies_renderer(program), text{ str }, tex_atlas{ calculate_max_size(face), 30 },
			chars{ str.size() }, atlas_loc{program->uniform_location("a_position")}
		{
			tex_atlas.mag_filter(gl::mag_filter::nearest);
			tex_atlas.min_filter(gl::min_filter::nearest);

			std::vector<float> positions;
			std::vector<float> uvs;

			std::map<uint32_t, std::pair<unsigned, unsigned>> char_uv;
			long penX = 0;
			long penY = 0;
			unsigned last_index = 0;

			float scaleX = 1.0 / 64.0;
			float scaleY = 1.0 / 64.0;

			auto begin = str.begin();
			while(begin != str.end()) {
				uint32_t code_point = utf8::next(begin, str.end());

				if (code_point == '\n') {
					penY -= face.get_size_metrics().height();
					penX = 0;
					continue;
				}

				face.load_glyph(face.get_char_index(code_point));
				freetype::glyph_slot glyph{ face.get_glyph() };
				freetype::glyph_slot::metrics metrics = glyph.get_metrics();

				if (metrics.height() == 0 && metrics.width() == 0) {
					penX += metrics.horizontal_advance();
					continue;
				}

				auto bitmap = glyph.get_bitmap();

				float left = (penX + metrics.horizontal_bearing_x()) * scaleX;
				float right = (penX + metrics.horizontal_bearing_x() + metrics.width()) * scaleX;
				float bot = (penY + metrics.horizontal_bearing_y() - metrics.height()) * scaleY;
				float top = (penY + metrics.horizontal_bearing_y()) * scaleY;

				positions.insert(positions.end(),
					{
						left, bot,
						right, bot,
						right, top,
						left, bot,
						right, top,
						left, top
					});

				if (char_uv.find(code_point) == char_uv.end()) {
					unsigned w = bitmap.width();
					unsigned h = bitmap.rows();
					std::vector<uint8_t> data(w * h * 4);

					for (unsigned x = 0; x < w; x++) {
						for (unsigned y = 0; y < h; y++) {
							uint8_t c = bitmap.data<uint8_t>()[x + bitmap.pitch() * y];
							data[(x + y * w) * 4] = 0xFF;
							data[(x + y * w) * 4 + 1] = 0xFF;
							data[(x + y * w) * 4 + 2] = 0xFF;
							data[(x + y * w) * 4 + 3] = c;
						}
					}

					std::pair<unsigned, unsigned> right_bot = tex_atlas.add(w, h, data.data());
					char_uv[code_point] = right_bot;
				}

				std::pair<unsigned, unsigned> p = char_uv[code_point];
				float x = p.first / (float)tex_atlas.width();
				float y = p.second / (float)tex_atlas.height();
				float w = metrics.width() / 64.0f / (float)tex_atlas.width();
				float h = metrics.height() / 64.0f / (float)tex_atlas.height();

				uvs.insert(uvs.end(),
					{
						x, y+h,
						x+w, y+h,
						x+w, y,
						x, y+h,
						x+w, y,
						x, y,
					});

				penX += metrics.horizontal_advance();
			}

			this->positions.data(positions);
			this->uvs.data(uvs);

			width = penX * scaleX;
		}

		void render() override {
			gl::enable_blending();
			gl::blend_func(gl::blending_factor::src_alpha, gl::blending_factor::one_minus_src_alpha);
			gl::active_texture(tex_atlas, 0);
			program()->uniform<int, 1>(program()->uniform_location("u_atlas"), 0);

			vertex_array->attrib_pointer<float, 2>(program()->attrib_location("a_position"), positions);
			vertex_array->enable_attrib_array(program()->attrib_location("a_position"));

			vertex_array->attrib_pointer<float, 2>(program()->attrib_location("a_uv"), uvs);
			vertex_array->enable_attrib_array(program()->attrib_location("a_uv"));

			program()->draw_arrays(gl::primitive_type::triangles, 0, 6 * chars, *vertex_array);
		}

		float get_width() {
			return width;
		}
	};
}