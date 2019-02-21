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


        public:

            circles()
            :
            gl::program(
                osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim_uv.vs"),
                osu::load<gl::fragment_shader>("shaders/passtrough_u_tex2_u_color4_a_uv.fs")
            ),
            tex{osu::load<gl::texture_2d>("disc.png")}
            {
                uniform<float, 2>("u_dim", 40, 40);
            }

            void add(const osu::beatmap::circle& c) {
                cs.emplace_back(c);
            }

            void draw(
                glm::mat4 lb,
                std::chrono::milliseconds millis,
                std::chrono::milliseconds preempt,
                std::chrono::milliseconds fade_in)
            {
                uniform("u_tex", 0);
                gl::active_texture(tex, 0);
                for(auto& c : cs) {
                    auto time = (millis - c.circle_info.start_time - preempt);
                    if(time.count() < 0 || time.count() > preempt.count())
                        continue;
                    
                    float br = (float)time.count() / (float)fade_in.count();

                    br = std::min(1.0f, br);

                    uniform(
                        "u_color",
                        glm::vec4{br}
                    );
                    uniform(
                        "u_mat",
                        glm::translate(
                            lb,
                            glm::vec3{c.position().x, c.position().y, 0}
                        )
                    );
                    draw_arrays(gl::primitive_type::triangle_fan, 0, 4);
                }
            }
        };
    }
}