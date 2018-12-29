#pragma once

#include "glfw/window.hpp"
#include "freetype/library.hpp"
#include <filesystem>
#include <vector>
#include "beatmap.hpp"

namespace osu {
    extern std::vector<beatmap_info> loaded_beatmaps;
    extern glfw::window* window;
    extern freetype::library main_lib;
    extern void import_beatmap(std::filesystem::path);

    inline void load_beatmap(std::filesystem::path bm_dir) {
        loaded_beatmaps.push_back(parse_beatmap_dir(bm_dir));
    }
}