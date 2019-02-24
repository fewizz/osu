#pragma once

#include "opengl/program.hpp"
#include "textures.hpp"
#include "shaders.hpp"
#include "beatmap.hpp"
#include "circle.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <cmath>

namespace osu {
    namespace playfield {
class circles : gl::program {
    std::vector<circle> cs;
    gl::texture_2d tex;
    gl::texture_2d approachcircle;
    float scale;


public:

    circles(float scale)
    :
    scale{scale},
    gl::program(
        osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim_uv.vs"),
        osu::load<gl::fragment_shader>("shaders/passtrough_u_tex2_u_color4_a_uv.fs")
    ),
    approachcircle {
        osu::load<gl::texture_2d>("approachcircle.png")
    },
    tex {
        osu::load<gl::texture_2d>("disc.png")
    }
    {
    }

    void add(const osu::beatmap::circle& c) {
        cs.emplace_back(c);
    }

    void draw(
        glm::mat4 lb,
        std::chrono::milliseconds elapsed_ms,
        std::chrono::milliseconds preempt_ms,
        std::chrono::milliseconds fade_in_ms)
    {
        float circle_rad = 64*scale;
        float circle_d = circle_rad*2;
        gl::active_texture(tex, 0);
        gl::active_texture(approachcircle, 1);

        float preempt = preempt_ms.count();
        float fade_in = fade_in_ms.count();

        for(auto& c : cs) {
            auto time_ms = (c.circle_info.start_time - preempt_ms - elapsed_ms);
            
            float time = time_ms.count();
            
            if(time < 0 || time > preempt)
                continue;
            
            float br = (preempt - time) / fade_in;
            br = std::min(1.0f, br);

            uniform("u_dim", glm::vec2{circle_d});
            uniform(
                "u_color",
                glm::vec4{1.0F, 1.0F, 1.0F, br}
            );
            uniform(
                "u_mat",
                glm::translate(
                    lb,
                    glm::vec3{c.position().x, c.position().y, 0}
                    -
                    glm::vec3{circle_rad, circle_rad, 0.0F}
                )
            );
            uniform("u_tex", 0);
            draw_arrays(gl::primitive_type::triangle_fan, 0, 4);

            float ac_scale = 1.1F + (4.0F - 1.1F)*(time / preempt);

            uniform(
                "u_color",
                glm::vec4{1.0F, 0.0F, 0.0F, 1.0F}
            );
            uniform("u_dim", glm::vec2{circle_d*ac_scale});
            uniform(
                "u_mat",
                glm::translate(
                    lb,
                    glm::vec3{c.position().x, c.position().y, 0}
                    -
                    glm::vec3{circle_rad*ac_scale, circle_rad*ac_scale, 0.0F}
                )
            );
            uniform("u_tex", 1);
            draw_arrays(gl::primitive_type::triangle_fan, 0, 4);
        }
    }
};
}
}