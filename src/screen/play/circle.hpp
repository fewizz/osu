#pragma once

#include "beatmap.hpp"
#include "glm/vec2.hpp"

namespace osu {
    namespace playfield {
        class circle {
            const osu::beatmap::circle& c;
        public:
            circle(const osu::beatmap::circle& c)
            : c{c}
            {}
            circle(circle&&) = default;
            glm::uvec2 position() { return c.pos; }
        };
    }
}