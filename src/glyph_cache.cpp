#include "glyph_cache.hpp"

using namespace std;
using namespace freetype;
using namespace gfx;

std::pair<glm::uvec2, std::unique_ptr<uint8_t[]>>
load_glyph_bitmap(glyph_slot& glyph) {
    auto bitmap = glyph.get_bitmap();
    unsigned w = bitmap.width();
    unsigned h = bitmap.rows();
    auto data = make_unique<uint8_t[]>(w * h * 4);

    for (unsigned x = 0; x < w; x++) {
        for (unsigned y = 0; y < h; y++) {
            uint8_t c = bitmap.data<uint8_t>()[x + bitmap.pitch() * y];
            data[(x + y * w) * 4] = 0xFF;
            data[(x + y * w) * 4 + 1] = 0xFF;
            data[(x + y * w) * 4 + 2] = 0xFF;
            data[(x + y * w) * 4 + 3] = c;
        }
    }

    return {{w, h}, std::move(data)};
}

glyph_info gfx::glyph_cache::get_glyph_info(freetype::glyph_index index) {
    if(!index_to_info[index].id.valid()) {
        auto& glyph = face.load_glyph(index);
        auto [dim, data] = load_glyph_bitmap(glyph);
        auto [id, slot] = atlas.add(dim, data.get());
        index_to_info.erase(index);
        index_to_info.insert({index, {id, slot, glyph.get_metrics()}});
    }

    return index_to_info[index];
}