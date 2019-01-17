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
#include "gui.hpp"
#include "opengl/shader.hpp"

namespace osu {
    extern std::vector<beatmap_set> beatmap_sets;
    extern std::vector<resourcepack> resourcepacks;
    //extern std::map<std::string, gl::shader> compiled_shaders;
    extern std::unique_ptr<glfw::window> window;
    extern std::unique_ptr<
        gui::screen<>
    > current_screen;
    extern freetype::library main_lib;
    extern freetype::face* main_face;
    extern std::unique_ptr<gfx::glyph_cache> glyph_cache;

    namespace worker {
        void add_task(std::function<void()> t);
    }

    void import_beatmap(std::filesystem::path);

    inline void load_beatmap(std::filesystem::path bm_dir) {
        beatmap_sets.push_back(parse_beatmap_dir(bm_dir));
    }

    std::filesystem::path get_resource_path(std::string p);
}