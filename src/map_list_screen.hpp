#pragma once

#include "gui.hpp"
#include "opengl/core.hpp"
#include <vector>
#include <filesystem>
#include <memory>
#include "text_drawer.hpp"
#include "opengl/shader.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "main.hpp"
#include "GLFW/glfw3.h"
#include "jpeg.hpp"
#include "openal/al.hpp"
#include "mp3.hpp"
#include "shaders.hpp"
#include "glyph_cache.hpp"

class map_list_screen : public gui::view<>
{
    /*struct outline_renderer : public gfx::shader_renderer {
        outline_renderer():gfx::shader_renderer( gl::program {
            osu::load<gl::vertex_shader>("shaders/rectangle_mat4_tex2_dim.vs"),
            osu::load<gl::fragment_shader>("shaders/color4.fs")    
        }) {
            program()->uniform<float, 2>(
                program()->uniform_location("u_dim"),
                glm::vec2{100, 600}
            );
        }

        void render() override {

        }
    } outline_renderer;*/

    std::vector<gfx::text_drawer> name_drawers;
    struct : public gfx::with_program {
        //using gfx::shader_renderer::shader_renderer;
        std::shared_ptr<gl::texture_2d> current_tex;

        inline void draw() {
            gl::active_texture(*current_tex, 0);
            program()->uniform<int, 1>(program()->uniform_location("u_tex"), 0);
            program()->uniform<int, 2>(program()->uniform_location("u_dim"), osu::window->get_framebuffer_size());
            program()->draw_arrays(gl::primitive_type::triangle_strip, 0, 4);
        }
    } back_drawer;

    struct s : public gfx::with_program {
        //using gfx::shader_renderer::shader_renderer;
        s()
        :
        gfx::with_program(
            gl::program {
                osu::load<gl::vertex_shader>("shaders/rectangle_mat4_tex2_dim.vs"),
                osu::load<gl::fragment_shader>("shaders/color4.fs")
            }
        ){}

        inline void draw() {
            program()->uniform<float, 2>(
                program()->uniform_location("u_dim"),
                glm::vec2 {
                    5000.0f,
                    80.0f
                }
            );

            program()->uniform<float, 4>(
                program()->uniform_location("u_color"),
                glm::vec4 {
                    1.0f,
                    1.0f,
                    1.0f,
                    1.0f
                }
            );
            program()->draw_arrays(gl::primitive_type::line_loop, 0, 4);
        }
    } outline;

    unsigned current_map = 0;
    al::source src;
    al::buffer buf;

  public:
    map_list_screen()
    :
    name_drawers { create_name_drawers() },
    back_drawer {
        gl::program {
            gl::vertex_shader{R"(
                #version 130
                uniform ivec2 u_dim;
                uniform sampler2D u_tex;
                in vec2 a_pos;
                out vec2 uv_vs;
                void main() {
                    float dimW = vec2(u_dim).x / vec2(u_dim).y;
                    vec2 size = vec2(textureSize(u_tex, 0));
                    float dimT = size.x / size.y;

                    float y = 1;
                    float x = 1;
                    
                    if(dimW < dimT) {
                        x *= dimT / dimW;
                    }
                    if(dimW > dimT)
                        y *= dimW / dimT;

                    vec2 positions[4] = vec2[](
                        vec2(-x, -y),
                        vec2(x, -y),
                        vec2(-x, y),
                        vec2(x, y)
                    );
                    gl_Position = vec4(positions[gl_VertexID], 0, 1);

                    vec2 uvs[4] = vec2[](
                        vec2(0, 0),
                        vec2(1, 0),
                        vec2(0, 1),
                        vec2(1, 1)
                    );
                    uv_vs = uvs[gl_VertexID];
                    uv_vs.y *= -1;
                }
                )"
            },
            osu::load<gl::fragment_shader>("shaders/passtrough_tex2.fs")
        }
    }
    {
        choose(0);

        osu::window->set_key_callback([&](int key, int scancode, int action, int mods) {
            if (current_map > 0 && key == GLFW_KEY_UP)
                choose(current_map-1);
            if (current_map < name_drawers.size() - 1 && key == GLFW_KEY_DOWN)
                choose(current_map+1);
        });
    }

    std::vector<gfx::text_drawer> create_name_drawers()
    {
        std::cout << "creating_name_renderers..." << "\n";
        std::vector<std::string> names;

        for (osu::beatmap_info bm : osu::loaded_beatmaps)
            names.push_back(bm.title());

        std::cout << "creating program..." << "\n";
        auto program = std::make_shared<gl::program>(
            osu::load<gl::vertex_shader>("shaders/passtrough_mat4_pos2_tex2.vs"),
            osu::load<gl::fragment_shader>("shaders/passtrough_tex2.fs")
        );
        std::cout << "program created..." << "\n";

        std::vector<gfx::text_drawer> result;
        for (std::string s : names)
            result.push_back({
                s,
                *osu::glyph_cache,
                program,
                gfx::text_drawer::origin::top_left
            });
        
        std::cout << "done..." << "\n";
        return result;
    }

    void choose(unsigned mp) {
        std::cout << "choosing " << mp << "\n";
        current_map = mp;

        std::vector<uint16_t> data;
        mp3::info info = mp3::decode(
            osu::loaded_beatmaps[mp].get_dir<std::string>()
            + "/"
            + *osu::loaded_beatmaps[mp].diffs[0].audio,
            data
        );
        std::cout << data.size() << " " <<info.channels << " " << info.frequency << "\n"; 
        src.stop();
        src.buffer(nullptr);
        buf.data(info.channels, 16, data, info.frequency);
        src.buffer(buf);
        src.play();
        std::cout << "al: " << al::internal::get_error() << "\n";

        back_drawer.current_tex = std::make_shared<gl::texture_2d>(jpeg::decode(
            std::filesystem::path
            {osu::loaded_beatmaps[mp].get_dir<std::string>()
            + "/"
            + *osu::loaded_beatmaps[mp].diffs[0].back
            }));
    }

    void draw()
    {
        glm::mat mat = glm::ortho<float> (
            0,
            osu::window->get_framebuffer_size().first,
            -osu::window->get_framebuffer_size().second,
            0
        );
        
        back_drawer.draw();

        mat = glm::translate(mat, {0, -100, 0});
        for (int i = 0; i < name_drawers.size(); i++)
        {
            auto& r = name_drawers[i];
            r.program()->uniform<float, 4, 4>(
                r.program()->uniform_location("u_mat"),
                mat);
            r.draw();

            if (i == current_map)
            {
                outline.program()->uniform<float, 4, 4>(
                    outline.program()->uniform_location("u_mat"),
                    glm::translate(mat, {0, -20, 0}));
            }

            mat = glm::translate(mat, {0, -80, 0});
        }
        outline.draw();
    }
};