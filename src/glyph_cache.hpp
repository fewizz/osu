#pragma once

#include "texture_atlas.hpp"
#include "freetype/face.hpp"
#include <map>
#include <cmath>

namespace gfx {

    static inline glm::uvec2 get_max_bbox_size(freetype::face& face) {
        auto size_metrics = face.get_size_metrics();
        freetype::bbox global_box{ face.get_bbox() };
        double w = (double)(global_box.x_max() - global_box.x_min()) / (double)face.units_per_em() * size_metrics.x_ppem();
        double h = (double)(global_box.y_max() - global_box.y_min()) / (double)face.units_per_em() * size_metrics.y_ppem();
        return {w + 1, h + 1};
    }

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