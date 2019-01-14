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
#include "beatmap.hpp"

class map_list_screen : public gui::view<>
{
    struct background_drawer: public gfx::triangle_fan_drawer<0, 4> {
        std::shared_ptr<gl::texture_2d> current_tex;

        background_drawer()
        :
        gfx::triangle_fan_drawer<0, 4>(
            gl::program {
                osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim_uv.vs"),
                osu::load<gl::fragment_shader>("shaders/passtrough_tex2.fs")
            }
        ){}

        void draw(glm::mat4 center);
    } background_drawer;

    struct outline_drawer : public gfx::line_loop_drawer<0, 4> {
        outline_drawer()
        :
        gfx::line_loop_drawer<0, 4> (
            gl::program {
                osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim.vs"),
                osu::load<gl::fragment_shader>("shaders/color4.fs")
            }
        ){}

        void draw(glm::mat4 top_left, glm::vec4 color, glm::vec2 dim);
    } outline_drawer;

    class beatmap_diffs_drawer {
        decltype(outline_drawer)& od;
        osu::beatmap_info info;
        gfx::text_drawer map_name;
        std::vector<gfx::text_drawer> diffs;
    public:

        beatmap_diffs_drawer(
            decltype(outline_drawer)& od,
            osu::beatmap_info,
            std::shared_ptr<gl::program> p
        );

        glm::mat4 draw(glm::mat4 left_top, int selected_d);
    };

    std::vector<beatmap_diffs_drawer> diffs_drawers;

    unsigned current_map = 0;
    unsigned current_diff = 0;
    al::source src;
    al::buffer buf;

  public:
    map_list_screen();
    void choose(unsigned map);
    void draw();
};