#pragma once

#include <filesystem>
#include <istream>
#include "beatmap.hpp"

namespace osu {
    void decode(std::istream& stream, beatmap& res);
}