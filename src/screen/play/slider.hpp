#pragma once

#include <vector>
#include "glm/vec2.hpp"
#include "beatmap.hpp"

namespace osu {
    namespace playfield {
        struct slider {
            std::vector<glm::vec2> positions;

            slider(const osu::beatmap::slider& s);
            slider(slider&&) = default;
        };
    }
}