#pragma once

#include "glfw/window.hpp"
#include "freetype/library.hpp"
#include <filesystem>
#include <vector>
#include "beatmap.hpp"
#include <fstream>
#include <string>
#include "resourcepack.hpp"
#include "glyph_cache.hpp"

namespace osu {
    extern std::vector<beatmap_info> loaded_beatmaps;
    extern std::vector<resourcepack> loaded_resourcepacks;
    extern std::unique_ptr<glfw::window> window;
    extern freetype::library main_lib;
    extern freetype::face* main_face;
    extern std::unique_ptr<gfx::glyph_cache> glyph_cache;

    void import_beatmap(std::filesystem::path);

    inline void load_beatmap(std::filesystem::path bm_dir) {
        loaded_beatmaps.push_back(parse_beatmap_dir(bm_dir));
    }

    inline std::filesystem::path get_resource_path(std::string p) {
        for(auto r : loaded_resourcepacks) {
            auto path = r.get_relative_path(p);
            if(std::filesystem::exists(path))
                return path;
        }
        throw std::runtime_error("resource \"" + p + "\" not found");
    }
}