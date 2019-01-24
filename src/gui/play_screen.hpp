#pragma once

#include "gui.hpp"
#include "size.hpp"
#include "beatmap.hpp"
#include "opengl/program.hpp"
#include "png.hpp"
#include "shaders.hpp"
#include "glm/mat4x4.hpp"

class play_screen : public gui::screen<> {
    //static constexpr prop::usize2 playfield_size{512, 384};
    osu::beatmap& bm;
    struct circle_drawer_t : gl::program {
        circle_drawer_t()
        :gl::program(
            osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim_uv.vs"),
            osu::load<gl::fragment_shader>("shaders/passtrough_u_tex2_a_uv.fs")
        ){
            uniform<unsigned, 2>("u_dim", 40, 40);
        }

        void draw(glm::mat4 center) {
            uniform<float, 4, 4>("u_mat", center);
            draw_arrays(gl::primitive_type::triangle_fan, 0, 4);
        }
    } circle_drawer;

public:
    play_screen(osu::beatmap& bm):bm{bm}{}

    void draw() override {

    }
    
    void update() override {

    }
};