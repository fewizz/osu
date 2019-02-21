#pragma once

#include "beatmap.hpp"
#include "glm/vec2.hpp"

namespace osu {
    namespace playfield {
        struct circle {
            const osu::beatmap::circle& circle_info;
            circle(const osu::beatmap::circle& c)
            : circle_info{c}
            {}
            circle(circle&&) = default;
            glm::uvec2 position() { return circle_info.pos; }
        };
    }
}