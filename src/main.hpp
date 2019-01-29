#pragma once

#include "glfw/window.hpp"
#include "freetype/library.hpp"
#include <filesystem>
#include <vector>
#include <fstream>
#include <string>
#include "glyph_cache.hpp"
#include "gui.hpp"

namespace osu {
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
}