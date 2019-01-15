#include "lodepng.h"
#include <filesystem>
#include <vector>
#include "opengl/texture.hpp"
#include <fstream>

namespace png {
    gl::texture_2d decode(std::filesystem::path path);
    bool is_png(std::filesystem::path path);
}