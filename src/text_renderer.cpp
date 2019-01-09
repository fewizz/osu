#include <iostream>
#include "text_renderer.hpp"

using namespace std;
using namespace freetype;
using namespace gfx;

/*static fixed_slot_container
get_occupator(freetype::face& face, glm::uvec2 tex_dim) {
    auto size = calculate_max_size(face);
    return fixed_slot_container(
        size,
        tex_dim[0]/size[0],
        tex_dim[1]/size[1]
    );
}*/
        
gfx::text_renderer::text_renderer(string str, glyph_cache& cache, shared_ptr<gl::program> program)
:
verticies_renderer(program),
text{ str },
cache { cache },
chars{ str.size() }
//atlas_loc{program->uniform_location("a_tex")}
{
    freetype::face& face = cache.get_face();
    std::vector<float> positions;
    std::vector<float> uvs;

    long penX = 0;
    long penY = 0;

    float scaleX = 1.0 / 64.0;
    float scaleY = 1.0 / 64.0;

    auto begin = str.begin();
    while(begin != str.end()) {
        uint32_t code_point = utf8::next(begin, str.end());

        if (code_point == '\n') {
            penY -= (double)face.get_size_metrics().height();
            penX = 0;
            continue;
        }

        auto [id, slot, metrics] =
            cache.get_glyph_info(face.get_char_index(code_point));

        if (metrics.height() == 0 && metrics.width() == 0) {
            penX += metrics.horizontal_advance();
            continue;
        }

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
        
        //cout << "met: " << metrics.width() << " " << metrics.height() << "\n";
        float x = slot.position[0] / (float)cache.get_texture_atlas().width();
        float y = slot.position[1] / (float)cache.get_texture_atlas().height();
        float w = metrics.width() / 64.0f / (float)cache.get_texture_atlas().width();
        float h = metrics.height() / 64.0f / (float)cache.get_texture_atlas().height();

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

void gfx::text_renderer::render() {
    gl::enable_blending();
    gl::blend_func(gl::blending_factor::src_alpha, gl::blending_factor::one_minus_src_alpha);
    gl::active_texture(cache.get_texture_atlas(), 0);
    program()->uniform<int, 1>(program()->uniform_location("u_tex"), 0);

    vertex_array->attrib_pointer<float, 2>(program()->attrib_location("a_pos"), positions);
    vertex_array->enable_attrib_array(program()->attrib_location("a_pos"));

    vertex_array->attrib_pointer<float, 2>(program()->attrib_location("a_uv"), uvs);
    vertex_array->enable_attrib_array(program()->attrib_location("a_uv"));

    program()->draw_arrays(gl::primitive_type::triangles, 0, 6 * chars, *vertex_array);
}