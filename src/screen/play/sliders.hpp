#pragma once

#include "slider.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"
#include <utility>
#include "shaders.hpp"
#include "opengl/vertex_array.hpp"
#include "slider_body_verticies.hpp"
#include <iostream>

namespace osu {
    namespace playfield {
        //using slider_body = gl::vertex_array;

        class sliders {
            std::vector<std::pair<slider, slider_body_verticies>> ss;
            //slider_drawer drawer;
            gl::program prog;
            gl::vertex_array vao;

        public:

            sliders()
            :
            prog {
                load<gl::vertex_shader>("shaders/slider.vs"),
                load<gl::fragment_shader>("shaders/slider.fs")
            }
            {
            }

            void add(const osu::beatmap::slider& c) {
                slider s{c};
                slider_body_verticies vs{s.positions};
                ss.emplace_back(std::move(s), std::move(vs));
            }

            void draw(glm::mat4 lb) {
                prog.uniform("u_color", glm::vec4{1});
                prog.uniform("u_mat", lb);
                for(auto& pair : ss) {
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