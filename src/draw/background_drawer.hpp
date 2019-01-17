#pragma once

#include "../properties/with_program.hpp"
#include "../properties/with_texture.hpp"
#include "glm/mat4x4.hpp"

namespace gfx {
    struct background_drawer:
    public prop::with_owned_program,
    public prop::with_unique_texture_2d {

        background_drawer(gl::program&& p)
        :
        prop::with_owned_program(std::move(p)),
        {}

        void draw(glm::mat4 center);
    };
}