#pragma once

#include <vector>
#include "beatmap.hpp"

namespace osu {
    extern std::vector<beatmap_set> beatmap_sets;

    void load_beatmaps();

    void load_beatmap_set(std::filesystem::path);
}
