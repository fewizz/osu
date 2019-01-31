#pragma once

#include "opengl/program.hpp"
#include "textures.hpp"
#include "shaders.hpp"
#include "beatmap.hpp"
#include "circle.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_transform.hpp"

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
                osu::load<gl::fragment_shader>("shaders/passtrough_u_tex2_a_uv.fs")
            ),
            tex{osu::load<gl::texture_2d>("disc.png")}
            {
                uniform<float, 2>("u_dim", 40, 40);
            }

            void add(const osu::beatmap::circle& c) {
                cs.emplace_back(c);
            }

            void draw(glm::mat4 lb) {
                for(auto& c : cs) {
                    uniform(
                        "u_mat",
                        glm::translate(
                            lb,
                            glm::vec3{c.position().x, c.position().y, 0}
                        )
                    );
                    uniform("u_tex", 0);
                    gl::active_texture(tex, 0);
                    draw_arrays(gl::primitive_type::triangle_fan, 0, 4);
                }
            }
        };
    }
}