#include <iostream>
#include "text_renderer.hpp"

using namespace std;
using namespace freetype;
using namespace gfx;

static glm::uvec2 calculate_max_size(freetype::face& face) {
    auto size_metrics = face.get_size_metrics();
    freetype::bbox global_box{ face.get_bbox() };
    double w = (double)(global_box.x_max() - global_box.x_min()) / (double)face.units_per_em() * size_metrics.x_ppem();
    double h = (double)(global_box.y_max() - global_box.y_min()) / (double)face.units_per_em() * size_metrics.y_ppem();
    return {w + 1, h + 1};
}

static gfx::fixed_slot_occupator
get_occupator(freetype::face& face, glm::uvec2 tex_dim) {
    auto size = calculate_max_size(face);
    return gfx::fixed_slot_occupator(
        size,
        tex_dim[0]/size[0],
        tex_dim[1]/size[1]
    );
}

tuple<slot_id, slot, glyph_metrics> bake_glyph(
    freetype::face& face,
    fixed_texture_atlas& atlas,
    uint32_t code_point) {
    glyph_slot& glyph = face.load_glyph(face.get_char_index(code_point));

    auto bitmap = glyph.get_bitmap();
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

    auto [id, slot] = atlas.add({w, h}, data.data());
    return {id, slot, glyph.get_metrics()};
}
        
gfx::text_renderer::text_renderer(string str, freetype::face& face, shared_ptr<gl::program> program)
:
verticies_renderer(program),
text{ str },
tex_atlas{ {1024, 1024}, get_occupator(face, {1024, 1024}) },
chars{ str.size() },
atlas_loc{program->uniform_location("a_position")}
{
    using namespace freetype;
    tex_atlas.mag_filter(gl::mag_filter::nearest);
    tex_atlas.min_filter(gl::min_filter::nearest);

    std::vector<float> positions;
    std::vector<float> uvs;

    struct info {
        slot_id id;
        const slot s;
        glyph_metrics metrics;
    };

    std::map<uint32_t, info> code_to_info;
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

        if (!code_to_info[code_point].id.valid()) {
            auto [id, s, m] =
                bake_glyph(face, tex_atlas, code_point);
            code_to_info.erase(code_point);
            code_to_info.insert({code_point, {id, s, m}});
            //cout << "pos: " << s.position.x << " " << s.position.y << "\n";
            //cout << "dim: " << s.dimension.x << " " << s.dimension.y << "\n";
            //cout << "id: " << id << "\n";
            //cout << "met: " << m.width() << " " << m.height() << "\n";
        }

        glyph_metrics metrics = code_to_info[code_point].metrics;

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
        slot s = code_to_info[code_point].s;
        float x = s.position[0] / (float)tex_atlas.width();
        float y = s.position[1] / (float)tex_atlas.height();
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

void gfx::text_renderer::render() {
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