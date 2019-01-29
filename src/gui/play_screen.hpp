#pragma once

#include "gui.hpp"
#include "size.hpp"
#include "beatmap.hpp"
#include "opengl/program.hpp"
#include "png.hpp"
#include "shaders.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "textures.hpp"
#include "opengl/texture.hpp"
#include "size.hpp"

class play_screen : public gui::screen<> {
    //static constexpr prop::usize2 playfield_size{512, 384};
    const osu::beatmap& bm;
    struct circle_drawer_t : gl::program {
        gl::texture_2d tex;

        circle_drawer_t()
        :gl::program(
            osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim_uv.vs"),
            osu::load<gl::fragment_shader>("shaders/passtrough_u_tex2_a_uv.fs")
        )
        ,
        tex{osu::load<gl::texture_2d>("disc.png")}
        {
            uniform<float, 2>("u_dim", 40, 40);
        }

        void draw(glm::mat4 center) {
            uniform("u_mat", center);
            uniform("u_tex", 0);
            gl::active_texture(tex, 0);
            draw_arrays(gl::primitive_type::triangle_fan, 0, 4);
        }
    } circle_drawer;

public:
    play_screen(const osu::beatmap& bm):bm{bm}{}

    void draw() override {
        auto ws = osu::window->framebuffer_size<prop::size2>();
        auto mat = glm::ortho<float>(
            -ws.w() / 2, ws.w() / 2,
            -ws.h() / 2, ws.h() / 2
        );

        for(auto& ho : bm.objects) {
            ho->draw(
                glm::translate(
                    mat,
                    {-256, -192, 0}
                )
            );
        }
    }
    
    void update() override {

    }
};