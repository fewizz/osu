#pragma once

#include "glfw/window.hpp"
#include "freetype/library.hpp"
#include <filesystem>

namespace osu {
    extern glfw::window* window;
    extern freetype::library main_lib;
    extern void import_beatmap(std::filesystem::path);
}