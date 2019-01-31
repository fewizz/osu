#pragma once

#include "opengl/program.hpp"
#include "textures.hpp"
#include "shaders.hpp"
#include "glm/mat4x4.hpp"
#include "properties/with_program.hpp"
#include "hit_object.hpp"

struct slider_drawer {
    slider_drawer(osu::slider s)
    /*:prop::with_shared_program (
        osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim_uv.vs"),
        osu::load<gl::fragment_shader>("shaders/passtrough_u_tex2_a_uv.fs")
    )
    //tex{osu::load<gl::texture_2d>("disc.png")}
    {
        uniform<float, 2>("u_dim", 40, 40);
    }*/

    void draw(glm::mat4 lb);/* {
        uniform("u_mat", lb);
        uniform("u_tex", 0);
        gl::active_texture(tex, 0);
        draw_arrays(gl::primitive_type::triangle_fan, 0, 4);
    }*/
};