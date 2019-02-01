#pragma once

#include "opengl/buffer.hpp"
#include "glm/vec2.hpp"
#include <vector>

namespace osu {
namespace playfield {

struct slider_body_verticies : public gl::array_buffer {

    slider_body_verticies(const std::vector<glm::vec2>& positions);
};

}
}