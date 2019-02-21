#pragma once

#include <vector>
#include "glm/vec2.hpp"
#include "beatmap.hpp"

namespace osu {
    namespace playfield {
        struct slider {
            osu::beatmap::slider slider_info;
            std::vector<glm::vec2> positions;

            slider(const osu::beatmap::slider& s);
            slider(slider&&) = default;
        };
    }
}