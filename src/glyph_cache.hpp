#pragma once

#include "texture_atlas.hpp"
#include "freetype/face.hpp"
#include <map>

namespace gfx {

    struct glyph_info {
        gfx::slot_id id;
        gfx::slot slot;
        freetype::glyph_metrics metrics;
        
        operator std::tuple<gfx::slot_id, gfx::slot, freetype::glyph_metrics>() {
            return {id, slot, metrics};
        }
    };

    class glyph_cache {
        texture_atlas atlas;
        freetype::face& face;
        std::map<freetype::glyph_index, glyph_info> index_to_info;

    public:
        glyph_cache(freetype::face& face, texture_atlas&& atlas)
        : atlas{std::move(atlas)}, face{face} {}

        texture_atlas& get_texture_atlas() {
            return atlas;
        }

        freetype::face& get_face() { return face; }

        glyph_info
        get_glyph_info(freetype::glyph_index gi);
    };



}