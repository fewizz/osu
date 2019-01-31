#pragma once

#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"
#include <vector>

namespace osu {
    struct on_field_object {
        virtual void draw(glm::mat4) = 0;
    };

    class circle : public on_field_object {
        glm::uvec2 position;
        uint combo;
    public:
        circle(glm::uvec2 position, uint combo)
        : position{position}, combo{combo}
        {}
        circle(circle&&) = default;

        void draw(glm::mat4 lb) override;
    };

    class slider : public on_field_object {
        std::vector<glm::uvec2> positions;

    public:
        slider(std::vector<glm::uvec2> positions):positions{std::move(positions)}{}
        slider(slider&&) = default;

        void draw(glm::mat4 lb) override;
    };
}