#pragma once

#include "gui.hpp"
#include "opengl/core.hpp"
#include <vector>
#include <filesystem>
#include <memory>
#include "text_renderer.hpp"
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

class map_list_screen : public gui::view<gui::renderable>
{
    gfx::glyph_cache cache;
    std::vector<gfx::text_renderer> name_renderers;
    struct : public gfx::shader_renderer {
        using gfx::shader_renderer::shader_renderer;
        std::shared_ptr<gl::texture_2d> current_tex;

        void render() override {
            gl::active_texture(*current_tex, 0);
            program()->uniform<int, 1>(program()->uniform_location("u_tex"), 0);
            program()->uniform<int, 2>(program()->uniform_location("u_dim"), osu::window->get_framebuffer_size());
            program()->draw_arrays(gl::primitive_type::triangle_strip, 0, 4);
        }
    } back_renderer;

    struct : public gfx::shader_renderer {
        using gfx::shader_renderer::shader_renderer;
        inline void render() override
        {
            program()->draw_arrays(gl::primitive_type::line_loop, 0, 4);
        }
    } outline;
    unsigned current_map = 0;
    al::source src;
    al::buffer buf;

  public:
    map_list_screen(freetype::face &face)
    :
    cache { create_glyph_cache(face) },
    name_renderers { create_name_renderers(face) },
    outline { gl::program {
        gl::vertex_shader{R"(
            #version 130
            #define H 60
            uniform mat4 u_mat;
            void main() {
                vec2 positions[4] = vec2[](
                    vec2(-1, 0),
                    vec2(2, 0),
                    vec2(2, H),
                    vec2(-1, H)
                );
                gl_Position = u_mat * vec4(positions[gl_VertexID] - vec2(0, 10), 0, 1);
                gl_Position.x = positions[gl_VertexID].x;
            }
        )"
        },
        gl::fragment_shader{R"(
            #version 130
            void main() {
                gl_FragColor = vec4(1);
            }
        )"}}},
    back_renderer {
        gl::program {
            gl::vertex_shader{R"(
                #version 130
                uniform ivec2 u_dim;
                uniform sampler2D u_tex;
                in vec2 a_pos;
                out vec2 uv_vs;
                void main() {
                    float dimW = vec2(u_dim).x / vec2(u_dim).y;
                    vec2 size = textureSize(u_tex, 0);
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
            if (current_map < name_renderers.size() - 1 && key == GLFW_KEY_DOWN)
                choose(current_map+1);
        });
    }

    static inline gfx::glyph_cache create_glyph_cache(freetype::face& face) {
        std::cout << "calculate size..." << "\n";

        auto max_size = gfx::get_max_bbox_size(face);

        auto atlas = gfx::texture_atlas {
            {1024, 1024},
            gfx::fixed_slot_container {
                max_size,
                1024 / max_size[0],
                1024 / max_size[1]
            }
        };
        atlas.mag_filter(gl::mag_filter::nearest);
        atlas.min_filter(gl::min_filter::nearest);

        std::cout << "creating cache..." << "\n";
        return { face, std::move(atlas) };
    }

    std::vector<gfx::text_renderer> create_name_renderers(freetype::face &face)
    {
        std::cout << "creating_name_renderers..." << "\n";
        std::vector<std::string> names;

        for (osu::beatmap_info bm : osu::loaded_beatmaps)
            names.push_back(bm.title());

        auto program = std::make_shared<gl::program>(
            osu::load<gl::vertex_shader>("shaders/passtrough_mat4_pos2_tex2.vs"),
            osu::load<gl::fragment_shader>("shaders/passtrough_tex2.fs")
        );

        std::vector<gfx::text_renderer> result;
        for (std::string s : names)
            result.push_back({s, cache, program});
        
        std::cout << "done..." << "\n";
        return result;
    }

    void choose(unsigned mp) {
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

        back_renderer.current_tex = std::make_shared<gl::texture_2d>(jpeg::decode(
            std::filesystem::path
            {osu::loaded_beatmaps[mp].get_dir<std::string>()
            + "/"
            + *osu::loaded_beatmaps[mp].diffs[0].back
            }));
    }

    void render() override
    {
        glm::mat mat = glm::ortho<float>(
            0,
            osu::window->get_framebuffer_size().first,
            -osu::window->get_framebuffer_size().second,
            0);
        
        back_renderer.render();

        mat = glm::translate(mat, {0, -100, 0});
        for (int i = 0; i < name_renderers.size(); i++)
        {
            auto &r = name_renderers[i];
            r.program()->uniform<float, 4, 4>(
                r.program()->uniform_location("u_mat"),
                mat);
            r.render();

            if (i == current_map)
            {
                outline.program()->uniform<float, 4, 4>(
                    outline.program()->uniform_location("u_mat"),
                    mat);
            }

            mat = glm::translate(mat, {0, -80, 0});
        }
        outline.render();
    }
};