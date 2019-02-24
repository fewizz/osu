#pragma once

#include "slider.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"
#include <utility>
#include "shaders.hpp"
#include "opengl/vertex_array.hpp"
#include "slider_body_verticies.hpp"
#include <iostream>
#include <algorithm>

namespace osu {
namespace playfield {

class sliders {
    std::vector<std::pair<slider, slider_body_verticies>> ss;
    gl::program prog;
    gl::vertex_array vao;
    float scale;

public:

    sliders(float scale)
    :
    scale{scale},
    prog {
        load<gl::vertex_shader>("shaders/slider.vs"),
        load<gl::fragment_shader>("shaders/slider.fs")
    }
    {
    }

    void add(const osu::beatmap::slider& c) {
        slider s{c};
        slider_body_verticies vs{s.positions, scale};
        ss.emplace_back(std::move(s), std::move(vs));
    }

    void draw(glm::mat4 lb, std::chrono::milliseconds millis) {
        prog.uniform("u_mat", lb);
        for(auto& pair : ss) {
            float br =
                (millis - pair.first.slider_info.start_time).count() / 1000.0;
            if(br > 1)
                br = 0;

            prog.uniform(
                "u_color",
                glm::vec4{
                    br
                }
            );
            vao.attrib_pointer<float, 2>(
                prog.attrib_location("a_pos"),
                pair.second
            );
            vao.enable_attrib_array(prog.attrib_location("a_pos"));
            prog.draw_arrays(
                gl::primitive_type::triangle_strip,
                0,
                pair.second.size() / sizeof(glm::vec2),
                vao);
        }
    }
};
}
}