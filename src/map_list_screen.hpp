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
    class name_drawer : public gfx::text_drawer {
    public:
        using gfx::text_drawer::text_drawer;

        void draw(glm::mat4 off) {
            prog()->uniform<float, 4, 4>(prog()->u_loc("u_mat"), off);
            gfx::text_drawer::draw();
        }
    };

    std::vector<name_drawer> name_drawers;
    struct : public gfx::triangle_fan_drawer<0, 4> {
        std::shared_ptr<gl::texture_2d> current_tex;

        inline void draw(glm::mat4 center) {
            glm::vec2 fb_size = osu::window->framebuffer_size<glm::uvec2>();
            glm::vec2 tex_size = {current_tex->width(), current_tex->height()};

            float w_ratio = fb_size[0] / fb_size[1];
            float t_ratio = tex_size[0] / tex_size[1];

            glm::vec2 scaled_size =
                current_tex->size<glm::vec2>()
                *
                (w_ratio > t_ratio ?
                    fb_size[0] / tex_size[0]
                    :
                    fb_size[1] / tex_size[1]
                );
            
            prog()->uniform<float, 2>(prog()->u_loc("u_dim"), scaled_size);

            prog()->uniform<float, 4, 4>(
                prog()->u_loc("u_mat"),
                translate(center, {-scaled_size[0] / 2, -scaled_size[1] / 2, 0})
            );

            gl::active_texture(*current_tex, 0);
            prog()->uniform<int, 1>(prog()->u_loc("u_tex"), 0);

            gfx::triangle_fan_drawer<0, 4>::draw();
        }
    } back_drawer;

    struct s : public gfx::line_loop_drawer<0, 4> {
        s()
        :
        gfx::line_loop_drawer<0, 4> (
            gl::program {
                osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim_a_uv.vs"),
                osu::load<gl::fragment_shader>("shaders/color4.fs")
            }
        ){}

        inline void draw(glm::mat4 top_left) {
            prog()->uniform<float, 2>(
                prog()->u_loc("u_dim"),
                glm::vec2 { 5000.0f, 80.0f}
            );

            prog()->uniform<float, 4>(prog()->u_loc("u_color"),glm::vec4 {1});
            prog()->uniform<float, 4, 4>(prog()->u_loc("u_mat"),top_left);

            gfx::line_loop_drawer<0, 4>::draw();
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
            osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim_uv.vs"),
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

    std::vector<name_drawer> create_name_drawers()
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

        std::vector<name_drawer> result;
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
        glm::vec2 fb_size = osu::window->framebuffer_size<glm::vec2>();
        glm::mat mat = glm::ortho<float> (
            -fb_size[0] / 2,
            fb_size[0] / 2,
            -fb_size[1] / 2,
            fb_size[1] / 2
        );
        back_drawer.draw(mat);

        mat = glm::ortho<float> (
            0,
            fb_size[0],
            -fb_size[1],
            0
        );

        mat = glm::translate(mat, {0, -100, 0});
        for (int i = 0; i < name_drawers.size(); i++)
        {
            name_drawers[i].draw(mat);

            if (i == current_map)
                outline.draw(glm::translate(mat, {0, -60, 0}));

            mat = glm::translate(mat, {0, -80, 0});
        }
    }
};